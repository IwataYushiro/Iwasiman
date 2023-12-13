#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace DirectX;
using namespace std;

/*

*	Model.cpp

*	3Dモデル(.obj)

*/

//静的メンバ変数の実体
ID3D12Device* Model::device_ = nullptr;
// デスクリプタサイズ
UINT Model::descriptorHandleIncrementSize_;

const std::string Model::BASE_DIRECTORY = "Resources/";

Model::~Model()
{
	for (auto m : meshes_)
	{
		delete m;
	}
	meshes_.clear();

	for (auto m: materials_)
	{
		delete m.second;
	}
	materials_.clear();
}

void Model::StaticInitialize(ID3D12Device* device)
{
	Model::device_ = device;
	//メッシュ初期化
	Mesh::StaticInitialize(device_);
}

//OBJファイルから3Dモデルを読み込む
std::unique_ptr<Model> Model::LoadFromOBJ(const std::string& modelName, bool smoothing)
{
	//新たなModel型のインスタンスのメモリを確保
	std::unique_ptr<Model> model = std::make_unique<Model>();
	//OBJファイルからのデータ読み込み
	model->LoadFromOBJInternal(modelName, smoothing);

	//デスクリプタヒープ生成
	model->InitializeDescriptorHeap();
	//テクスチャ読み込み
	model->LoadTextures();
	
	return model;
}

//デスクリプタヒープ
void Model::InitializeDescriptorHeap() {

	HRESULT result = S_FALSE;

	//マテリアル数
	size_t materialCount = materials_.size();
	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = (UINT)materialCount; // シェーダーリソースビュー=マテリアル数
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//生成
	if (FAILED(result)) {
		assert(0);
	}

	// デスクリプタサイズを取得
	descriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

//マテリアル読み込み
void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//ファイルストリーム
	std::ifstream file;
	//マテリアルファイルを開く
	file.open(directoryPath + filename);
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	Material* material = nullptr;

	//1行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭のタブ文字は無視
		const int32_t startKeyNum = 0;
		if (key[startKeyNum] == '\t')
		{
			key.erase(key.begin());//先頭の文字を削除
		}

		//先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl")
		{
			//既にマテリアルがある場合
			if (material)
			{
				//マテリアルをコンテナに登録
				AddMaterial(material);
			}
			//新しいマテリアルの生成
			material = Material::Create();
			//マテリアル名読み込み
			line_stream >> material->name;
		}
		//先頭文字列がKaならアンビエント色
		if (key == "Ka")
		{
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;
		}
		//先頭文字列がKdならディフューズ色
		if (key == "Kd")
		{
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;
		}
		//先頭文字列がKsならスペキュラー色
		if (key == "Ks")
		{
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;
		}
		////先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd")
		{
			//テクスチャのファイル名読み込み
			line_stream >> material->textureFilename;
			//フルパスからファイル名取り出し
			size_t pos1;
			const int32_t offsetPos1Num = 1;
			pos1 = material->textureFilename.rfind('\\');
			if (pos1!=string::npos)
			{
				material->textureFilename = material->textureFilename.substr(
					pos1 + offsetPos1Num, material->textureFilename.size() - pos1 - offsetPos1Num);
			}

			pos1 = material->textureFilename.rfind('/');
			if (pos1 != string::npos)
			{
				material->textureFilename = material->textureFilename.substr(
					pos1 + offsetPos1Num, material->textureFilename.size() - pos1 - offsetPos1Num);
			}
		}
	}
	//ファイルを閉じる
	file.close();

	if (material)
	{
		AddMaterial(material);
	}
}

void Model::LoadTextures()
{
	int texIndex = 0;
	string directoryPath = BASE_DIRECTORY + name_ + "/";
	for (auto& m : materials_)
	{
		Material* material = m.second;
		D3D12_CPU_DESCRIPTOR_HANDLE CDHSRV = descHeap_->GetCPUDescriptorHandleForHeapStart();
		CDHSRV.ptr += (descriptorHandleIncrementSize_ * texIndex);
		
		D3D12_GPU_DESCRIPTOR_HANDLE GDHSRV = descHeap_->GetGPUDescriptorHandleForHeapStart();
		GDHSRV.ptr += (descriptorHandleIncrementSize_ * texIndex);
		//テクスチャ無しの場合
		if (material->textureFilename.size() <= 0)
		{
			directoryPath = BASE_DIRECTORY;
		}

		//テクスチャ読み込み
		material->LoadTexture(directoryPath, CDHSRV, GDHSRV);
		texIndex++;
	}
}

// 描画
void Model::Draw(ID3D12GraphicsCommandList* cmdList) const {
	// nullptrチェック
	assert(device_);
	assert(cmdList);

	if (descHeap_)
	{
		// デスクリプタヒープの配列
		ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}
	for (auto& mesh : meshes_)
	{
		mesh->Draw(cmdList);
	}
	
}

//OBJファイルから3Dモデルを読み込む(非公開)
void Model::LoadFromOBJInternal(const std::string& modelName,const bool smoothing) {
	//ファイルストリーム
	std::ifstream file;
	//objファイルを開く
	const string filename = modelName + ".obj";					
	const string directoryPath = BASE_DIRECTORY + modelName + "/";

	file.open(directoryPath + filename);//"Resources/triangle_mat/triangle_mat.obj"
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}
	name_ = modelName;
	//メッシュ生成
	Mesh* mesh = new Mesh();
	int indexCountTex = 0;
	//int indexCountNoTex = 0;

	vector<XMFLOAT3> positions;					//頂点座標
	vector<XMFLOAT3> normals;					//法線ベクトル
	vector<XMFLOAT2> texcoords;					//テクスチャUV
	//1行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);
		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭文字列がmtllibならマテリアル
		if (key == "mtllib")
		{
			//マテリアルのファイル名読み込み
			string fileNameMat;
			line_stream >> fileNameMat;
			//マテリアル読み込み
			LoadMaterial(directoryPath, fileNameMat);
		}
		//先頭文字列がgならグループの開始
		if (key == "g")
		{
			if (mesh->GetName().size()>0)
			{
				//頂点法線の平均によるエッジの平滑化
				if (smoothing)
				{
					mesh->CalculateSmoothedVertexNormals();
				}
				//コンテナに登録
				meshes_.emplace_back(mesh);
				//次のメッシュ生成
				mesh = new Mesh();
				indexCountTex = 0;
			}

			//グループ名読み込み
			string groupName;
			line_stream >> groupName;

			mesh->SetName(groupName);
		}
		//先頭文字列がvなら頂点座標
		if (key == "v")
		{
			//X,Y,Z座標読み込み
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//座標データに追加
			positions.emplace_back(position);

		}
		//先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			//U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			//V方向反転
			const float reverseCalculation = 1.0f - texcoord.y;
			texcoord.y = reverseCalculation;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);

		}
		//先頭文字列がvnなら法線ベクトル
		if (key == "vn")
		{
			//X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			//法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		// 先頭文字列がusemtlならマテリアルを割り当てる
		if (key == "usemtl") 
		{
			if (mesh->GetMaterial() == nullptr)
			{
				//マテリアル名読み込み
				string materialName;
				line_stream >> materialName;

				//マテリアル名で検索し割り当てる
				auto itr = materials_.find(materialName);
				if (itr!=materials_.end())
				{
					mesh->SetMaterial(itr->second);
				}
			}
		}
		//先頭文字列がfならポリゴン(三角形)
		if (key == "f")
		{
			//インデックスカウント
			int faceIndexCount = 0;
			//半角スペース区切りで行の続きを読み込む
			string index_string;
			//スラッシュのスキップするためのオフセット
			const int32_t seekSkipSlashOffset = 1;
			//頂点インデックスのオフセット
			const int32_t indexOffset = 1;
			//デフォルトの法線ベクトルとUV
			struct DefaultNormalAndUV
			{
				const XMFLOAT3 normal = { 0.0f,0.0f,1.0f };
				const XMFLOAT2 uv = { 0.0f,0.0f };
			};
			DefaultNormalAndUV defaultNormalAndUV;

			while (getline(line_stream, index_string, ' '))
			{
				//頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexTexcoord, indexNormal;
				//頂点番号
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				//スラッシュを飛ばす
				index_stream.seekg(seekSkipSlashOffset, ios_base::cur);
				//マテリアル、テクスチャがある場合
				if (material && material->textureFilename.size() > 0)
				{
					index_stream >> indexTexcoord;
					index_stream.seekg(seekSkipSlashOffset, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexNormal;

					//頂点データの追加
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - indexOffset];
					vertex.normal = normals[indexNormal - indexOffset];
					vertex.uv = texcoords[indexTexcoord - indexOffset];
					mesh->AddVertex(vertex);
					//エッジ平滑化データの追加
					if (smoothing)
					{
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - indexOffset);
					}
				}
				else
				{
					char c;
					index_stream >> c;
					// スラッシュ二連続なら頂点番号のみ
					if (c == '/')
					{
						//頂点データ追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - indexOffset];
						vertex.normal = defaultNormalAndUV.normal;
						vertex.uv = defaultNormalAndUV.uv;
						mesh->AddVertex(vertex);
						//エッジ平滑化データの追加
						if (smoothing)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - indexOffset);
						}
					}
					else
					{
						index_stream.seekg(-seekSkipSlashOffset, ios_base::cur);//ヒトマスモドル
						index_stream >> indexTexcoord;
						index_stream.seekg(seekSkipSlashOffset, ios_base::cur);//スラッシュを飛ばす
						index_stream >> indexNormal;
						//頂点データの追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - indexOffset];
						vertex.normal = normals[indexNormal - indexOffset];
						vertex.uv = defaultNormalAndUV.uv;
						mesh->AddVertex(vertex);
						//エッジ平滑化データの追加
						if (smoothing)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - indexOffset);
						}
					}
				}
				//インデックスデータの追加
				//3頂点目
				const int inPoligonVertexTriangle = 3;
				if (faceIndexCount >= inPoligonVertexTriangle)
				{
					
					// 四角形ポリゴンの4点目なので、
					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex - TIN_Index2));
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex));
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex - TIN_Index0));
				}
				else
				{
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex));
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	//ファイルを閉じる
	file.close();

	//頂点法線の平均によるエッジの平滑化
	if (smoothing)
	{
		mesh->CalculateSmoothedVertexNormals();
	}
	//メッシュコンテナに登録
	meshes_.emplace_back(mesh);

	//メッシュのマテリアルチェック
	for (auto& m : meshes_)
	{
		//ない場合
		if (m->GetMaterial() == nullptr)
		{
			if (defaultMaterial_ == nullptr)
			{
				//デフォルトのマテリアルを生成
				defaultMaterial_= Material::Create();
				defaultMaterial_->name = "no material";
				materials_.emplace(defaultMaterial_->name, defaultMaterial_);
			}
			//それをセット
			m->SetMaterial(defaultMaterial_);
		}
	}

	//メッシュのバッファ生成
	for (auto& m : meshes_)
	{
		m->CreateBuffers();
	}
	//マテリアル数値を定数バッファに反映
	for (auto& m : materials_)
	{
		m.second->Update();
	}
}

