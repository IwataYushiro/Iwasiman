#pragma once
#include <DirectXMath.h>

/*

*	PointLight.h (cpp X)

*	ポイントライト

*/
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
	const XMFLOAT3 presetLightPos_ = { 0.0f,0.0f,0.0f };//プリセット
	XMFLOAT3 lightPos_ = presetLightPos_;
	//ライトの色
	const XMFLOAT3 presetLightColor_ = { 1.0f,1.0f,1.0f };//プリセット
	XMFLOAT3 lightColor_ = presetLightColor_;
	//ライト距離の減衰係数
	const XMFLOAT3 presetLightAtten_ = { 1.0f,1.0f,1.0f };//プリセット
	XMFLOAT3 lightAtten_ = presetLightAtten_;
	//アクティブフラグ
	bool active_ = false;

public://アクセッサ置き場

	//ライト座標セット
	inline void SetLightPos(const XMFLOAT3& lightPos) { this->lightPos_ = lightPos; }
	//ライト座標ゲット
	inline const XMFLOAT3& GetLightPos() { return lightPos_; }
	//ライトカラーセット
	inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor_ = lightColor; }
	//ライトカラーゲット
	inline const XMFLOAT3& GetLightColor() { return lightColor_; }
	//ライト距離の減衰係数セット
	inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten_ = lightAtten; }
	//ライト距離の減衰係数ゲット
	inline const XMFLOAT3& GetLightAtten() { return lightAtten_; }
	//点光源のライトセット
	inline void SetActive(const bool active) { this->active_ = active; }
	//点光源のライトゲット
	inline bool IsActive() { return active_; }
};