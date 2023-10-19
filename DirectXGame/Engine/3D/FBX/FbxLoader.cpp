#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

/*

*	FbxLoader.cpp

*	FBXローダー

*/

//静的メンバ変数の実体
const std::string FbxLoader::BASE_DIRECTORY = "Resources/";
const std::string FbxLoader::defaultTextureFileName_ = "white1x1.png";

FbxLoader* FbxLoader::GetInstance()
{
	static FbxLoader instance;
	return &instance;
}

void FbxLoader::ConvertMatrixFromFBX(DirectX::XMMATRIX* dst, const FbxAMatrix& src)
{
	//行
	for (int i = 0; i < 4; i++)
	{
		//列
		for (int j = 0; j < 4; j++)
		{
			//1要素をコピー
			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
		}
	}
}

void FbxLoader::Initialize(ID3D12Device* device)
{
	//再初期化チェック
	assert(fbxManager_ == nullptr);
	//引数からメンバ変数に代入
	this->device_ = device;

	//FBXマネージャー生成
	fbxManager_ = FbxManager::Create();

	//FBXマネージャーの入出力
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager_, IOSROOT);
	fbxManager_->SetIOSettings(ios);

	//FBXインポーター生成
	fbxImporter_ = FbxImporter::Create(fbxManager_, "");

}

void FbxLoader::Finalize()
{
	//インスタンス放棄
	fbxImporter_->Destroy();
	fbxManager_->Destroy();
}

ModelFbx* FbxLoader::LoadModelFromFile(const string& modelName)
{
	//モデルと同名のフォルダから読み込む
	const string directoryPath = BASE_DIRECTORY + modelName + "/";
	//拡張子.fbxを追加
	const string filename = modelName + ".fbx";
	//連結してフルパスを得る
	const string fullPath = directoryPath + filename;
	//ファイル名を指定してFBXファイルを読み込む
	if (!fbxImporter_->Initialize(fullPath.c_str(), -1,
		fbxManager_->GetIOSettings()))
	{
		assert(0);
	}
	//シーン生成
	FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxScene");

	//ファイルからロードしたFBXの情報をシーンにインポート
	fbxImporter_->Import(fbxScene);

	//モデル生成
	ModelFbx* modelF = new ModelFbx();
	modelF->name_ = modelName;
	//FBXノードの数を取得
	int nodeCount = fbxScene->GetNodeCount();
	// あらかじめ必要数分のメモリを開放することでアドレスがズレるのを予防
	modelF->nodes_.reserve(nodeCount);
	//ルートノードから順に解析してモデルに流し込む
	ParseNodeRecursive(modelF, fbxScene->GetRootNode());
	modelF->fbxScene_ = fbxScene;
	//バッファ生成
	modelF->CreateBuffers(device_);

	return modelF;
}

void FbxLoader::ParseNodeRecursive(ModelFbx* modelF, FbxNode* fbxNode, Node* parent)
{
	//ノード名を取得
	string name = fbxNode->GetName();

	//モデルにノードを追加
	modelF->nodes_.emplace_back();
	Node& node = modelF->nodes_.back();
	//ノード名を取得
	node.name = fbxNode->GetName();

	// FBXノードのローカル移動情報等
	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();
	// 形式変換して代入
	node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
	node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
	node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };
	// 回転角を度からラジアンに
	node.rotation.m128_f32[0] = XMConvertToRadians(node.rotation.m128_f32[0]);
	node.rotation.m128_f32[1] = XMConvertToRadians(node.rotation.m128_f32[1]);
	node.rotation.m128_f32[2] = XMConvertToRadians(node.rotation.m128_f32[2]);
	// 三種行列の計算
	XMMATRIX matScale, matRot, matTrans;
	matScale = XMMatrixScalingFromVector(node.scaling);
	matRot = XMMatrixRotationRollPitchYawFromVector(node.rotation);
	matTrans = XMMatrixTranslationFromVector(node.translation);
	// ローカル変形行列の計算
	node.transform = XMMatrixIdentity();
	node.transform *= matScale;
	node.transform *= matRot;
	node.transform *= matTrans;
	// グローバル変形行列の計算
	node.globalTransform = node.transform;
	if (parent)
	{
		node.parent = parent;
		//親の変形を乗算
		node.globalTransform *= parent->globalTransform;
	}
	//FBXノードの情報を解析してノードに記録

	//FBXノードのメッシュ情報を解析    
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType()== FbxNodeAttribute::eMesh)
		{
			modelF->meshNode_ = &node;
			ParseMesh(modelF, fbxNode);
		}
	}
	//子ノードに対して再帰呼び出し
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(modelF, fbxNode->GetChild(i), &node);
	}
}

void FbxLoader::ParseMesh(ModelFbx* modelF, FbxNode* fbxNode)
{
	//ノードのメッシュを取得
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//頂点座標読み取り
	ParseMeshVertices(modelF, fbxMesh);
	//面を構成するデータの読み取り
	ParseMeshFaces(modelF, fbxMesh);
	//マテリアルの読み取り
	ParseMaterial(modelF, fbxNode);
	//スキニング情報読み取り
	ParseSkin(modelF, fbxMesh);
}

void FbxLoader::ParseMeshVertices(ModelFbx* modelF, FbxMesh* fbxMesh)
{
	auto& vertices = modelF->vertices_;

	//頂点座標データの数
	const int controlPointsCount = fbxMesh->GetControlPointsCount();
	//必要数だけ頂点データ配列を確保
	ModelFbx::VertexPosNormalUvSkin vert{};
	modelF->vertices_.resize(controlPointsCount, vert);

	//FBXメッシュの頂点座標配列を取得
	FbxVector4* pCoord = fbxMesh->GetControlPoints();
	//FBXメッシュの全頂点座標をモデル内の配列にコピーする
	for (int i = 0; i < controlPointsCount; i++)
	{
		ModelFbx::VertexPosNormalUvSkin& vertex = vertices[i];
		//座標のコピー
		vertex.pos.x = (float)pCoord[i][0];
		vertex.pos.y = (float)pCoord[i][1];
		vertex.pos.z = (float)pCoord[i][2];
	}
}

void FbxLoader::ParseMeshFaces(ModelFbx* modelF, FbxMesh* fbxMesh)
{
	auto& vertices = modelF->vertices_;
	auto& indices = modelF->indices_;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);
	//面の数
	const int polygonCount = fbxMesh->GetPolygonCount();
	//UVデータ数
	const int textureCount = fbxMesh->GetTextureUVCount();
	//UV名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);
	//面ごとの情報読み取り
	for (int i = 0; i < polygonCount; i++)
	{
		//面を構成する頂点の数を取得(3なら三角形ポリゴン)
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		//1頂点ずつ処理
		for (int j = 0; j < polygonSize; j++)
		{
			//FBX頂点配列のインデックス
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			//頂点法線読み込み
			ModelFbx::VertexPosNormalUvSkin& vertex = vertices[index];
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal.x = static_cast<float>(normal[0]);
				vertex.normal.y = static_cast<float>(normal[1]);
				vertex.normal.z = static_cast<float>(normal[2]);
			}
			//テクスチャUV読み込み
			if (textureCount > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;
				//0番決め打ちで代入
				if (fbxMesh->GetPolygonVertexUV(i, j,
					uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = static_cast<float>(uvs[0]);
					vertex.uv.y = static_cast<float>(uvs[1]);
				}
			}
			//インデックス配列に頂点インデックスを追加
			//3頂点目までなら
			if (j < 3)
			{
				//1点追加し、他の2点と三角形を構築
				indices.push_back(static_cast<unsigned short>(index));
			}
			//4頂点目
			else
			{
				//3点追加
				//四角形の0,1,2,3の内 2,3,0で三角形を構築する
				int index2 = indices[indices.size() - 1];
				int index3 = static_cast<unsigned short>(index);
				int index0 = indices[indices.size() - 3];
				indices.push_back(static_cast<unsigned short>(index2));
				indices.push_back(static_cast<unsigned short>(index3));
				indices.push_back(static_cast<unsigned short>(index0));
			}
		}
	}

}

void FbxLoader::ParseMaterial(ModelFbx* modelF, FbxNode* fbxNode)
{
	const int materialCount = fbxNode->GetMaterialCount();
	if (materialCount > 0)
	{
		//先頭のマテリアルを取得
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		//テクスチャを読み込んだかどうかを表すフラグ
		bool textureLoaded = false;
		if (material)
		{
			//FbxSurfaceLambertクラスかどうかを調べる
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert =
				static_cast<FbxSurfaceLambert*>(material);
				//アンビエント係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				modelF->ambient_.x = (float)ambient.Get()[0];
				modelF->ambient_.y = (float)ambient.Get()[1];
				modelF->ambient_.z = (float)ambient.Get()[2];
				//ディフューズ係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				modelF->diffuse_.x = (float)diffuse.Get()[0];
				modelF->diffuse_.y = (float)diffuse.Get()[1];
				modelF->diffuse_.z = (float)diffuse.Get()[2];
			}
			//ディフューズテクスチャを取り出す
			const FbxProperty diffuseProperty =
			material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid())
			{
				const FbxFileTexture* texture =
				diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char* filePath = texture->GetFileName();
					//ファイルパス名からファイル名を検出
					string path_str(filePath);
					string name = ExtractFileName(path_str);
					//テクスチャを読み込む
					LoadTexture(modelF, BASE_DIRECTORY + modelF->name_ + "/" + name);
					textureLoaded = true;
				}
			}
		}
		//テクスチャがない場合はデフォルトテクスチャ
		if (!textureLoaded)
		{
			LoadTexture(modelF, BASE_DIRECTORY + defaultTextureFileName_);
		}
	}
}

void FbxLoader::LoadTexture(ModelFbx* modelF, const std::string& fullpath)
{
	HRESULT result = S_FALSE;
	//WICテクスチャのロード
	TexMetadata& metadata = modelF->metadata_;
	ScratchImage& scratchImg = modelF->scratchImg_;
	//ユニコード文字列に変換
	wchar_t wfilePath[128];
	MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilePath, _countof(wfilePath));
	result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(result))
	{
		assert(0);
	}
}

void FbxLoader::ParseSkin(ModelFbx* modelF, FbxMesh* fbxMesh)
{
	//スキニング情報
	FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	//スキニング情報が無ければ終了
	if (fbxSkin == nullptr)
	{
		//各頂点について処理
		for (int i = 0; i < modelF->vertices_.size(); i++)
		{
			//最初のボーンの影響を100％にする
			modelF->vertices_[i].boneIndex[0] = 0;
			modelF->vertices_[i].boneWeight[0] = 1.0f;
		}
		return;
	}
	//ボーン配列参照
	std::vector<ModelFbx::Bone>& bones = modelF->bones_;
	//ボーン数
	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	//初期姿勢座標読み取り
	//全ボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		//ボーン自体のノード名を取得
		const char* boneName = fbxCluster->GetLink()->GetName();
		//新しくボーンを追加し、追加したボーンの参照を得る
		bones.emplace_back(ModelFbx::Bone(boneName));
		ModelFbx::Bone& bone = bones.back();
		//自作ボーンとFBXのボーンを紐づける
		bone.fbxCluster = fbxCluster;

		//FBXから初期姿勢行列を取得
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);
		//XMMATRIX型に変換
		XMMATRIX initialPose;
		ConvertMatrixFromFBX(&initialPose, fbxMat);
		//初期姿勢行列の逆行列を取得
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
	}
	//ボーン番号とスキンウェイト
	struct WeightSet
	{
		UINT index;
		float weight;
	};
	//二次元配列(ジャグ配列)
	//list=頂点が影響を受けるボーンのリスト
	//vecter＝そんなリストの全頂点分
	std::vector<std::list<WeightSet>>
		weightLists(modelF->vertices_.size());

	//全ボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		//このボーンに影響を受ける頂点の数
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
		//このボーンに影響を受ける頂点の配列
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeight = fbxCluster->GetControlPointWeights();
		//影響を受ける全頂点について
		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			//頂点取得
			int vertIndex = controlPointIndices[j];
			//スキンウェイト
			float weight = (float)controlPointWeight[j];
			//その頂点の影響を受けるボーンリストにボーンとウェイトのペアを追加
			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}
	}
	//頂点配列書き換え用の参照
	auto& vertices = modelF->vertices_;
	//各頂点についての処理
	for (int i = 0; i < vertices.size(); i++)
	{
		//頂点のウェイトから最も大きい4つを選択
		auto& weightList = weightLists[i];
		//大小比較用のラムダ式を指定して降順にソート
		weightList.sort(
			[](auto const& lhs, auto const& rhs) {
				//先の要素の方が大きければtrue　それ以外はfalseを返す
				return lhs.weight > rhs.weight;
			}
		);
		int weightArrayIndex = 0;
		//降順ソート済みのウェイトリストから
		for (auto& WeightSet : weightList)
		{
			//頂点データに書き込み
			vertices[i].boneIndex[weightArrayIndex] = WeightSet.index;
			vertices[i].boneWeight[weightArrayIndex] = WeightSet.weight;
			//4つに達したら終了
			if (++weightArrayIndex >= ModelFbx::MAX_BONE_INDICES)
			{
				float weight = 0.0f;
				//2番目以降のウェイトを合計
				for (int j = 0; j < ModelFbx::MAX_BONE_INDICES; j++)
				{
					weight += vertices[i].boneWeight[j];
				}
				//合計で1.0f(100％)になるように調整
				vertices[i].boneWeight[0] = 1.0f - weight;
				break;
			}
		}
	}

}

std::string FbxLoader::ExtractFileName(const std::string path)
{
	size_t pos1;
	//区切り文字‘\\’が出てくる一番最後の文字を検索
	pos1 = path.rfind('\\');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	//区切り文字‘/’が出てくる一番最後の文字を検索
	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	return path;
}
