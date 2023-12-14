#pragma once
#include <DirectXMath.h>
namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	SpotLight.h (cpp X)

	*	スポットライト

	*/
	class SpotLight
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
			XMVECTOR lightv;
			XMFLOAT3 lightPos;
			float pad1;
			XMFLOAT3 lightColor;
			float pad2;
			XMFLOAT3 lightatten;
			float pad3;
			XMFLOAT2 lightfactoranglecos;
			unsigned int active;
			float pad4;

		};
	private://メンバ変数
		//ライト方向(単位ベクトル)
		const XMVECTOR presetLightDir_ = { 1.0f,0.0f,0.0f,0.0f };//プリセット
		XMVECTOR lightDir_ = presetLightDir_;
		//ライト座標(ワールド座標系)
		const XMFLOAT3 presetLightPos_ = { 0.0f,0.0f,0.0f };//プリセット
		XMFLOAT3 lightPos_ = presetLightPos_;
		//ライト色
		const XMFLOAT3 presetLightColor_ = { 1.0f,1.0f,1.0f };//プリセット
		XMFLOAT3 lightColor_ = presetLightColor_;
		//ライト距離減衰係数
		const XMFLOAT3 presetLightAtten_ = { 1.0f,1.0f,1.0f };//プリセット
		XMFLOAT3 lightAtten_ = presetLightAtten_;
		//ライト減衰角度(開始角度、終了角度)
		const XMFLOAT2 presetLightFactorAngleCos_ = { 0.5f,0.2f };//プリセット
		XMFLOAT2 lightFactorAngleCos_ = presetLightFactorAngleCos_;
		//有効フラグ
		bool active_ = false;

	public://メンバ関数
		//ライト方向セット(方向)
		inline void SetLightDir(const XMVECTOR& lightDir)
		{
			this->lightDir_ = DirectX::XMVector3Normalize(lightDir);
		}
		//ライト方向ゲット
		inline const XMVECTOR& GetLightDir() { return lightDir_; }
		//ライト座標セット(座標)
		inline void SetLightPos(const XMFLOAT3& lightPos) { this->lightPos_ = lightPos; }
		//ライト座標ゲット
		inline const XMFLOAT3& GetLightPos() { return lightPos_; }
		//ライトカラーセット(カラー)
		inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor_ = lightColor; }
		//ライトカラーゲット
		inline const XMFLOAT3& GetLightColor() { return lightColor_; }
		//ライト距離の減衰係数セット(減衰係数)
		inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten_ = lightAtten; }
		//ライト距離の減衰係数ゲット
		inline const XMFLOAT3& GetLightAtten() { return lightAtten_; }
		//ライト減衰角度セット(減衰角度)
		inline void SetLightFactorAngleCos(const XMFLOAT2& lightFactorAngle)
		{
			this->lightFactorAngleCos_.x = cosf(DirectX::XMConvertToRadians(lightFactorAngle.x));
			this->lightFactorAngleCos_.y = cosf(DirectX::XMConvertToRadians(lightFactorAngle.y));
		}
		//ライト減衰角度ゲット
		inline const XMFLOAT2& GetLightFactorAngleCos() { return lightFactorAngleCos_; }
		//スポットライトのライトセット(有効化フラグ)
		inline void SetActive(const bool active) { this->active_ = active; }
		//スポットライトのライトゲット
		inline bool IsActive() { return active_; }
	};
}