#pragma once
#include <DirectXMath.h>

//ポイントライト
class PointLight
{
private: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://定数
	static const int DirLightNum = 3;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMFLOAT3 lightPos;
		float pad1;
		XMFLOAT3 lightColor;
		float pad2;
		XMFLOAT3 lightatten;
		unsigned int active;

	};

private://メンバ変数
	//ライト座標
	XMFLOAT3 lightPos_ = { 0.0f,0.0f,0.0f };
	//ライトの色
	XMFLOAT3 lightColor_ = { 1.0f,1.0f,1.0f };
	//ライト距離の減衰係数
	XMFLOAT3 lightAtten_ = { 1.0f,1.0f,1.0f };
	//アクティブフラグ
	bool active_ = false;

public://アクセッサ置き場
	
	//ライト座標
	inline void SetLightPos(const XMFLOAT3& lightPos) { this->lightPos_ = lightPos; }
	inline const XMFLOAT3& GetLightPos() { return lightPos_; }
	//ライトの色
	inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor_ = lightColor; }
	inline const XMFLOAT3& GetLightColor() { return lightColor_; }
	//ライト距離の減衰係数
	inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten_ = lightAtten; }
	inline const XMFLOAT3& GetLightAtten() { return lightAtten_; }
	//平行光源のライト色のセット
	inline void SetActive(bool active) { this->active_ = active; }
	inline bool IsActive() { return active_; }
};