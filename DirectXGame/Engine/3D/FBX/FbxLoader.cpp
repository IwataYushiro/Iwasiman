#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

//静的メンバ変数の実体
const std::string FbxLoader::baseDirectory = "Resources/";

FbxLoader* FbxLoader::GetInstance()
{
	static FbxLoader instance;
	return &instance;
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

void FbxLoader::LoadModelFromFile(const string& modelName)
{
	//モデルと同名のフォルダから読み込む
	const string directoryPath = baseDirectory + modelName + "/";
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
	modelF->name = modelName;
	//FBXノードの数を取得
	int nodeCount = fbxScene->GetNodeCount();
	// あらかじめ必要数分のメモリを開放することでアドレスがズレるのを予防
	modelF->nodes.reserve(nodeCount);
	//ルートノードから順に解析してモデルに流し込む
	ParseNodeRecursive(modelF, fbxScene->GetRootNode());
	//FBXシーン解放
	fbxScene->Destroy();

}

void FbxLoader::ParseNodeRecursive(ModelFbx* modelF, FbxNode* fbxNode, Node* parent)
{
	//ノード名を取得
	string name = fbxNode->GetName();

	//モデルにノードを追加
	modelF->nodes.emplace_back();
	Node& node = modelF->nodes.back();
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

	//子ノードに対して再帰呼び出し
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(modelF, fbxNode->GetChild(i), &node);
	}
}
