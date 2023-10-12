#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

/*

*	Material.h

*	マテリアル管理

*/
class Material
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス
	//定数バッファ
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;	// アンビエント係数
		float pad1;			// パディング
		XMFLOAT3 diffuse;	// ディフューズ係数
		float pad2;			// パディング
		XMFLOAT3 specular;	// スペキュラー係数
		float alpha;		// アルファ値
	};

public://静的メンバ関数
	//生成
	static Material* Create();
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);

private://静的メンバ変数
	static ID3D12Device* device_;

public://共有メンバ変数
	std::string name;				//マテリアル名
	XMFLOAT3 ambient;				//アンビエント影響度
	XMFLOAT3 diffuse;				//ディフューズ影響度
	XMFLOAT3 specular;				//スペキュラー影響度
	float alpha;					//アルファ値
	std::string textureFilename;	//テクスチャファイル名

public://メンバ関数	
	
	// テクスチャ読み込み
	void LoadTexture(
		const std::string& directoryPath, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	// 更新
	void Update();

private://メンバ変数
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	// シェーダリソースビューのハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	//マッピング
	ConstBufferDataB1* constMap = nullptr;

private://カプセル化メンバ関数
	//コンストラクタ
	Material() {
		ambient = { 0.3f,0.3f,0.3f };
		diffuse = { 0.0f,0.0f,0.0f };
		specular = { 0.0f,0.0f,0.0f };
		alpha = 1.0f;
		cpuDescHandleSRV = {};
		gpuDescHandleSRV = {};
	}

	//初期化
	void Initialize();
	//定数バッファ生成
	void CreateConstBuffer();

public://アクセッサ置き場
	// 定数バッファの取得
	ID3D12Resource* GetConstantBuffer() { return constBuff.Get(); }
	//ハンドル関係
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV; }


};
