#pragma once
#include <DirectXMath.h>
#include <memory>
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"

namespace IwasiEngine//IwasiEngineのネームスペース
{
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
		enum LightNum
		{
			LN_0,	//0番ライト
			LN_1,	//1番ライト
			LN_2,	//2番ライト
			LN_NUM,	//配列用
		};
		//ライト数
		static const int32_t DirLightNum = 3;
		//点光源の数
		static const int32_t PointLightNum = 3;
		//スポットライトの数
		static const int32_t SpotLightNum = 3;
		//丸影の数
		static const int32_t CircleShadowNum = 1;

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
			//スポットライト用
			SpotLight::ConstBufferData spotLights[SpotLightNum];
			//丸影用
			CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
		};

	private://静的メンバ変数
		//デバイス
		static ID3D12Device* device_;

	public://静的メンバ関数
		//静的初期化(デバイス)
		static void StaticInitialize(ID3D12Device* device);
		//インスタンス生成
		static std::unique_ptr<LightGroup> Create();

	private://メンバ変数

		//環境光の色
		const XMFLOAT3 presetAmbientColor_ = { 1.0f,1.0f,1.0f };//プリセット
		XMFLOAT3 ambientColor_ = presetAmbientColor_;
		//平行光源の配列
		DirectionalLight dirLights_[DirLightNum];
		//定数バッファ
		ComPtr<ID3D12Resource> constBuff_;
		//ダーティフラグ
		bool dirty_ = false;

		//点光源の配列
		PointLight pointLights_[PointLightNum];

		//スポットライトの配列
		SpotLight spotLights_[SpotLightNum];

		//丸影の配列
		CircleShadow circleShadows_[CircleShadowNum];

	public://メンバ関数
		//初期化
		void Initialize();
		//更新
		void Update();
		//描画(コマンドリスト、ルートパラメータインデックス)
		void Draw(ID3D12GraphicsCommandList* cmdList, const UINT rootParameterIndex);

		//定数バッファ転送
		void TransferConstBuffer();
		//標準のライト設定
		void DefaultLightSetting();

		//環境光のライト色をセット(カラー)
		void SetAmbientColor(const XMFLOAT3& color);
		//平行光源の有効フラグのセット(何番ライト、起動フラグ)
		void SetDirLightActive(const int index, const bool active);
		//平行光源のライト方向のセット(何番ライト、方向)
		void SetDirLightDir(const int index, const XMVECTOR& lightdir);
		//平行光源のライト色のセット(何番ライト、カラー)
		void SetDirLightColor(const int index, const XMFLOAT3& lightcolor);

		//点光源
		//ライト座標セット(何番ライト、座標)
		void SetPointLightPos(const int index, const XMFLOAT3& lightPos);
		//ライトの色セット(何番ライト、カラー)
		void SetPointLightColor(const int index, const XMFLOAT3& lightColor);
		//ライト距離の減衰係数セット(何番ライト、減衰係数)
		void SetPointLightAtten(const int index, const XMFLOAT3& lightAtten);
		//点光源のライトセット(何番ライト、起動フラグ)
		void SetPointLightActive(const int index, const bool active);

		//スポットライト
		//ライト方向セット(何番ライト、方向)
		void SetSpotLightDir(const int index, const XMVECTOR& lightDir);
		//ライト座標セット(何番ライト、座標)
		void SetSpotLightPos(const int index, const XMFLOAT3& lightPos);
		//ライトの色セット(何番ライト、カラー)
		void SetSpotLightColor(const int index, const XMFLOAT3& lightColor);
		//ライト距離の減衰係数セット(何番ライト、減衰係数)
		void SetSpotLightAtten(const int index, const XMFLOAT3& lightAtten);
		//ライト減衰角度セット(何番ライト、減衰角度)
		void SetSpotLightFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos);
		//スポットライトのライトセット(何番ライト、起動フラグ)
		void SetSpotLightActive(const int index, const bool active);

		//丸影
		//丸影の方向セット(何番シャドウ、方向)
		void SetCircleShadowDir(const int index, const XMVECTOR& lightDir);
		//丸影のキャスター座標セット(何番シャドウ、座標)
		void SetCircleShadowCasterPos(const int index, const XMFLOAT3& casterPos);
		//丸影のキャスターとライトの距離セット(何番シャドウ、距離)
		void SetCircleShadowDistanceCasterLight(const int index, const float distanceCasterLight);
		//丸影の減衰係数セット(何番シャドウ、減衰係数)
		void SetCircleShadowAtten(const int index, const XMFLOAT3& lightAtten);
		//丸影の減衰角度セット(何番シャドウ、減衰角度)
		void SetCircleShadowFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos);
		//丸影セット(何番シャドウ、起動フラグ)
		void SetCircleShadowActive(const int index, const bool active);
	};

}