#pragma once

#include "fbxsdk.h"
#include "ModelFbx.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

class FbxLoader
{
private://エイリアス
	//std::を省略
	using string = std::string;

public://定数
	static const string baseDirectory;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

public:
	//初期化
	void Initialize(ID3D12Device* device);

	//終了
	void Finalize();

	//ファイルからFBXモデル読み込み
	void LoadModelFromFile(const string& modelName);

	//再帰的にノード構成を解析
	void ParseNodeRecursive(ModelFbx* modelF, FbxNode* fbxNode, Node* parent = nullptr);
	//メッシュ読み込み
	void ParseMesh(ModelFbx* modelF, FbxNode* fbxNode);
	//頂点座標読み取り
	void ParseMeshVertices(ModelFbx* modelF, FbxMesh* fbxMesh);
	//面情報読み取り
	void ParseMeshFaces(ModelFbx* modelF, FbxMesh* fbxMesh);
	//マテリアル読み取り
	void ParseMaterial(ModelFbx* modelF, FbxNode* fbxNode);
	//テクスチャ読み取り
	void LoadTexture(ModelFbx* modelF, const std::string& fullpath);
private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

private:
	//デバイス
	ID3D12Device* device_ = nullptr;
	//FBXマネージャー
	FbxManager* fbxManager_ = nullptr;
	//FBXインポーター
	FbxImporter* fbxImporter_ = nullptr;
};