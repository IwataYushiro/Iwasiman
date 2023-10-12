#pragma once

#include "fbxsdk.h"
#include "ModelFbx.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

/*

*	FbxLoader.h

*	FBXローダー

*/
class FbxLoader
{
private://エイリアス
	//std::を省略
	using string = std::string;

public://定数
	static const string baseDirectory;

public://静的メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	//FBXの行列をXMMATRIXに変換(XM行列、FBX行列)
	static void ConvertMatrixFromFBX(DirectX::XMMATRIX* dst, const FbxAMatrix& src);
public://メンバ関数
	//初期化(デバイス)
	void Initialize(ID3D12Device* device);

	//終了
	void Finalize();

	//ファイルからFBXモデル読み込み(モデルパス)
	ModelFbx* LoadModelFromFile(const string& modelName);

	//再帰的にノード構成を解析(FBXモデル、FBXノード、親子関係)
	void ParseNodeRecursive(ModelFbx* modelF, FbxNode* fbxNode, Node* parent = nullptr);
	//メッシュ読み込み(FBXモデル、FBXノード)
	void ParseMesh(ModelFbx* modelF, FbxNode* fbxNode);
	//頂点座標読み取り(FBXモデル、FBXメッシュ)
	void ParseMeshVertices(ModelFbx* modelF, FbxMesh* fbxMesh);
	//面情報読み取り(FBXモデル、FBXメッシュ)
	void ParseMeshFaces(ModelFbx* modelF, FbxMesh* fbxMesh);
	//マテリアル読み取り(FBXモデル、FBXノード)
	void ParseMaterial(ModelFbx* modelF, FbxNode* fbxNode);
	//テクスチャ読み取り(FBXモデル、テクスチャプルパス)
	void LoadTexture(ModelFbx* modelF, const std::string& fullpath);
	//スキニング情報読み取り(FBXモデル、FBXメッシュ)
	void ParseSkin(ModelFbx* modelF, FbxMesh* fbxMesh);
	//ディレクトリを含んだファイルパスからファイル名を抽出する(ファイルパス)
	std::string ExtractFileName(const std::string path);

private:
	// コンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// デストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;
	//デフォルトテクスチャ名
	static const string defaultTextureFileName;
private:
	//デバイス
	ID3D12Device* device_ = nullptr;
	//FBXマネージャー
	FbxManager* fbxManager_ = nullptr;
	//FBXインポーター
	FbxImporter* fbxImporter_ = nullptr;
};