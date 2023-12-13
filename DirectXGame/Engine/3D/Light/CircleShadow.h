#pragma once
#include <DirectXMath.h>
/*

*	CircleShadow.h (cpp X)

*	丸影

*/
class CircleShadow
{
private://エイリアステンプレート
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
		XMVECTOR dir;
		XMFLOAT3 casterPos;
		float distanceCasterLight;
		XMFLOAT3 atten;
		float pad;
		XMFLOAT2 factorAngleCos;
		unsigned int active;
		float pad2;

	};
private://メンバ変数
	//方向(単位ベクトル)
	const XMVECTOR presetDir_ = { 1.0f,0.0f,0.0f,0.0f };//プリセット
	XMVECTOR dir_ = presetDir_;
	//キャスターとライトの距離
	const float presetDistanceCasterLight_ = 300.0f;//プリセット
	float distanceCasterLight_ = presetDistanceCasterLight_;
	//キャスター座標(ワールド座標系)
	const XMFLOAT3 presetCasterPos_ = { 0.0f,0.0f,0.0f };//プリセット
	XMFLOAT3 casterPos_ = presetCasterPos_;
	
	//距離減衰係数
	const XMFLOAT3 presetAtten_ = { 0.5f,0.6f,0.0f };//プリセット
	XMFLOAT3 atten_ = presetAtten_;
	//減衰角度
	const XMFLOAT2 presetFactorAngleCos_ = { 0.2f,0.5f };//プリセット
	XMFLOAT2 factorAngleCos_ = presetFactorAngleCos_;
	//有効フラグ
	bool active_ = false;

public://メンバ関数
	//方向セット(方向)
	inline void SetDir(const XMVECTOR& dir){this->dir_ = DirectX::XMVector3Normalize(dir);}
	//方向ゲット
	inline const XMVECTOR& GetDir() { return dir_; }
	//キャスター座標セット(座標)
	inline void SetCasterPos(const XMFLOAT3& casterPos) { this->casterPos_ = casterPos; }
	//キャスター座標ゲット
	inline const XMFLOAT3& GetCasterPos() { return casterPos_; }
	//キャスターとライトの距離セット(距離)
	inline void SetDistanceCasterLight(const float distanceCasterLight)
	{ this->distanceCasterLight_ = distanceCasterLight; }
	//キャスターとライトの距離ゲット
	inline const float GetDistanceCasterLight() { return distanceCasterLight_; }
	//減衰係数セット(減衰係数)
	inline void SetAtten(const XMFLOAT3& atten) { this->atten_ = atten; }
	//減衰係数ゲット
	inline const XMFLOAT3& GetAtten() { return atten_; }
	//減衰角度セット(減衰角度)
	inline void SetFactorAngleCos(const XMFLOAT2& factorAngle)
	{
		this->factorAngleCos_.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		this->factorAngleCos_.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}
	//減衰角度ゲット
	inline const XMFLOAT2& GetFactorAngleCos() { return factorAngleCos_; }
	//丸影セット(有効化フラグ)
	inline void SetActive(const bool active) { this->active_ = active; }
	//丸影ゲット
	inline bool IsActive() { return active_; }
};
