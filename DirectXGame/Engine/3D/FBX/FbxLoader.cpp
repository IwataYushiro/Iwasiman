#include "FbxLoader.h"
#include <cassert>
#include "XYZ.h"

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
	for (int i = 0; i < XYZW_Num; i++)
	{
		//列
		for (int j = 0; j < XYZW_Num; j++)
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
	const int32_t fileFormat = -1;
	//ファイル名を指定してFBXファイルを読み込む
	if (!fbxImporter_->Initialize(fullPath.c_str(), fileFormat,
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

	const float rotW = 0.0f;
	const float scaleW = 0.0f;
	const float positionW = 1.0f;
	// 形式変換して代入
	node.rotation = { (float)rotation[XYZ_X],(float)rotation[XYZ_Y],(float)rotation[XYZ_Z],rotW };
	node.scaling = { (float)scaling[XYZ_X],(float)scaling[XYZ_Y],(float)scaling[XYZ_Z],scaleW};
	node.translation = { (float)translation[XYZ_X],(float)translation[XYZ_Y],(float)translation[XYZ_Z],positionW };
	// 回転角を度からラジアンに
	node.rotation.m128_f32[XYZ_X] = XMConvertToRadians(node.rotation.m128_f32[XYZ_X]);
	node.rotation.m128_f32[XYZ_Y] = XMConvertToRadians(node.rotation.m128_f32[XYZ_Y]);
	node.rotation.m128_f32[XYZ_Z] = XMConvertToRadians(node.rotation.m128_f32[XYZ_Z]);
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
		vertex.pos.x = (float)pCoord[i][XYZ_X];
		vertex.pos.y = (float)pCoord[i][XYZ_Y];
		vertex.pos.z = (float)pCoord[i][XYZ_Z];
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
		const int porigonSizeSquare = 4;
		assert(polygonSize <= porigonSizeSquare);

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
				vertex.normal.x = static_cast<float>(normal[XYZ_X]);
				vertex.normal.y = static_cast<float>(normal[XYZ_Y]);
				vertex.normal.z = static_cast<float>(normal[XYZ_Z]);
			}
			//テクスチャUV読み込み
			if (textureCount > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;
				//0番決め打ちで代入
				if (fbxMesh->GetPolygonVertexUV(i, j,
					uvNames[XY_X], uvs, lUnmappedUV))
				{
					vertex.uv.x = static_cast<float>(uvs[XY_X]);
					vertex.uv.y = static_cast<float>(uvs[XY_Y]);
				}
			}
			//インデックス配列に頂点インデックスを追加
			//3頂点目までなら
			const int inPoligonVertexTriangle = 3;
			if (j < inPoligonVertexTriangle)
			{
				//1点追加し、他の2点と三角形を構築
				indices.push_back(static_cast<unsigned short>(index));
			}
			//4頂点目
			else
			{
				//インデックス参照用
				enum TriangleIndexNum
				{
					TIN_Index2 = 1,
					TIN_Index0 = 3,
				};
				//3点追加
				//四角形の0,1,2,3の内 2,3,0で三角形を構築する
				int index2 = indices[indices.size() - TIN_Index2];
				int index3 = static_cast<unsigned short>(index);
				int index0 = indices[indices.size() - TIN_Index0];
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
				modelF->ambient_.x = (float)ambient.Get()[XYZ_X];
				modelF->ambient_.y = (float)ambient.Get()[XYZ_Y];
				modelF->ambient_.z = (float)ambient.Get()[XYZ_Z];
				//ディフューズ係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				modelF->diffuse_.x = (float)diffuse.Get()[XYZ_X];
				modelF->diffuse_.y = (float)diffuse.Get()[XYZ_Y];
				modelF->diffuse_.z = (float)diffuse.Get()[XYZ_Z];
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
	const int maxPathNum = 128;
	wchar_t wfilePath[maxPathNum];
	const int cbMultiByte = -1;
	MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), cbMultiByte, wfilePath, _countof(wfilePath));
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
		//インデックスと重さの初期値
		struct DefaultBoneIndexWeight
		{
			const int startIndex = 0;
			const int index = 0;
			const float weight = 1.0f;
		};
		DefaultBoneIndexWeight defaultBIW;
		//各頂点について処理
		for (int i = 0; i < modelF->vertices_.size(); i++)
		{
			//最初のボーンの影響を100％にする
			modelF->vertices_[i].boneIndex[defaultBIW.startIndex] = defaultBIW.index;
			modelF->vertices_[i].boneWeight[defaultBIW.startIndex] = defaultBIW.weight;
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
				const int32_t index0 = 0;
				const float weightCalculation = 1.0f - weight;

				vertices[i].boneWeight[index0] = weightCalculation;
				break;
			}
		}
	}

}

std::string FbxLoader::ExtractFileName(const std::string path)
{
	size_t pos1;
	const int32_t offsetNum = 1;
	//区切り文字‘\\’が出てくる一番最後の文字を検索
	pos1 = path.rfind('\\');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + offsetNum, path.size() - pos1 - offsetNum);
	}
	//区切り文字‘/’が出てくる一番最後の文字を検索
	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + offsetNum, path.size() - pos1 - offsetNum);
	}
	return path;
}
