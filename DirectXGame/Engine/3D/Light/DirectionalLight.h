#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

/*

*	DirectionalLight.h

*	ライト

*/
class DirectionalLight
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR lightV;		//ライトへの方向を表すベクトル
		XMFLOAT3 lightColor;	//ライトの色
		unsigned int active;
	};

public://静的メンバ関数
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);

	//インスタンス生成
	static DirectionalLight* Create();

private://静的メンバ変数
	//デバイス
	static ID3D12Device* device_;

private://メンバ変数
	//ライト光線方向(単位ベクトル)
	XMVECTOR lightDir = { 1.0f,0.0f,0.0f,0.0f };
	//ライト色
	XMFLOAT3 lightColor = { 1.0f,1.0f,1.0f };
	//ダーティフラグ
	bool dirty = false;
	//有効フラグ
	bool active_ = false;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	

public://メンバ関数
	//初期化
	void Initialize();

	//更新
	void Update();

	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	//定数バッファ転送
	void TransferConstBuffer();

	//ライト方向のセット
	void SetLightDir(const XMVECTOR& lightdir);
	const XMVECTOR& GetLightDir() { return lightDir; }

	//ライト色のセット
	void SetLightColor(const XMFLOAT3& lightcolor);
	const XMFLOAT3& GetLightColor() { return lightColor; }

	//有効フラグのセット
	inline void SetActive(bool active) { this->active_ = active; }
	//有効かどうか
	inline bool IsActive() { return active_; }
};

