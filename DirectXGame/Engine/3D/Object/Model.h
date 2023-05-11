#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

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


public://静的メンバ関数
	//OBJファイルから3Dモデルを読み込む
	static Model* LoadFromOBJ(const std::string& modelName, bool smoothing = false);
	
public://メンバ関数
	// デスクリプタヒープの初期化
	void InitializeDescriptorHeap();

	// マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// テクスチャ読み込み
	void LoadTexture(const std::string& directoryPath, const std::string& filename);
	
	//各種バッファ生成
	void CreateBuffers();

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList,UINT rootParamIndexMaterial);

private://メンバ変数
	// デバイス
	static ID3D12Device* device;
	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize;
	
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	
	
public://アクセッサ置き場
	//デバイス
	static void SetDevice(ID3D12Device* device) { Model::device = device; }

private://メンバ関数(カプセル化)
	//OBJファイルから3Dモデルを読み込む(非公開)
	void LoadFromOBJInternal(const std::string& modelName, bool smoothing = false);
	
};

