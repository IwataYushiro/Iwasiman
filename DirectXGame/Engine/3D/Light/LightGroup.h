#pragma once
#include <DirectXMath.h>
#include "DirectionalLight.h"
#include "PointLight.h"

/*

*	LightGroup.h

*	ライトグループ

*/
class LightGroup
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

public://定数
	static const int DirLightNum = 3;
	//点光源の数
	static const int PointLightNum = 3;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		//環境光の色
		XMFLOAT3 ambientColor;
		float pad1;
		//平行光源用
		DirectionalLight::ConstBufferData dirLights[DirLightNum];
		//点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
	};

private://静的メンバ変数
	//デバイス
	static ID3D12Device* device_;

public://静的メンバ関数
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//インスタンス生成
	static LightGroup* Create();

private://メンバ変数
	
	//環境光の色
	XMFLOAT3 ambientColor_ = { 1.0f,1.0f,1.0f };
	//平行光源の配列
	DirectionalLight dirLights_[DirLightNum];
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	//ダーティフラグ
	bool dirty_ = false;

	//点光源の配列
	PointLight pointLights_[PointLightNum];

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	//定数バッファ転送
	void TransferConstBuffer();
	//標準のライト設定
	void DefaultLightSetting();

	//環境光のライト色をセット
	void SetAmbientColor(const XMFLOAT3& color);
	//平行光源の有効フラグのセット
	void SetDirLightActive(int index, bool active);
	//平行光源のライト方向のセット
	void SetDirLightDir(int index, const XMVECTOR& lightdir);
	//平行光源のライト色のセット
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);

	//点光源
	//ライト座標
	void SetPointLightPos(int index,const XMFLOAT3& lightPos);
	//ライトの色
	void SetPointLightColor(int index, const XMFLOAT3& lightColor);
	//ライト距離の減衰係数
	void SetPointLightAtten(int index, const XMFLOAT3& lightAtten);
	//平行光源のライト色のセット
	void SetPointLightActive(int index, bool active);
};


