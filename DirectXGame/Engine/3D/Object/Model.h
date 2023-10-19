#pragma once
#include "Mesh.h"
#include <DirectXMath.h>
#include <unordered_map>

/*

*	Model.h

*	3Dモデル(.obj)

*/
class Model
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://コンストラクタ等
	// デストラクタ
	~Model();

public://静的メンバ関数
	//静的初期化(デバイス)
	static void StaticInitialize(ID3D12Device* device);
	//OBJファイルから3Dモデルを読み込む(モデル名、スムージングするか)
	static Model* LoadFromOBJ(const std::string& modelName, bool smoothing = false);
	
public://メンバ関数
	
	// デスクリプタヒープの初期化
	void InitializeDescriptorHeap();

	// マテリアル読み込み(ディレクトリパス、モデル名)
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	//マテリアル登録(マテリアル)
	void AddMaterial(Material* material) { materials_.emplace(material->name, material); }
	// テクスチャ読み込み
	void LoadTextures();	

	// 描画(コマンドリスト)
	void Draw(ID3D12GraphicsCommandList* cmdList);

private://静的メンバ変数
	// デバイス
	static ID3D12Device* device_;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize_;
	//ベースディレクトリ
	static const std::string BASE_DIRECTORY;
	
private:
	//名前
	std::string name_;
	//メッシュコンテナ
	std::vector<Mesh*> meshes_;
	//マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials_;
	//デフォルトマテリアル
	Material* defaultMaterial_ = nullptr;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap_;
	
	
public://アクセッサ置き場
	//デバイスセット
	static void SetDevice(ID3D12Device* device) { Model::device_ = device; }
	//メッシュコンテナゲット
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

private://メンバ関数(カプセル化)
	//OBJファイルから3Dモデルを読み込む(非公開)(モデル名、スムージングするか)
	void LoadFromOBJInternal(const std::string& modelName, bool smoothing = false);
	
};

