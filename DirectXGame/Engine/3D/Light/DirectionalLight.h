#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
namespace IwasiEngine//IwasiEngineのネームスペース
{
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

	private://メンバ変数
		//ライト光線方向(単位ベクトル)
		const XMVECTOR presetLightDir_ = { 1.0f,0.0f,0.0f,0.0f };//プリセット
		XMVECTOR lightDir_ = presetLightDir_;
		//ライトカラー
		const XMFLOAT3 presetLightColor_ = { 1.0f,1.0f,1.0f };//プリセット
		XMFLOAT3 lightColor_ = presetLightColor_;
		//有効フラグ
		bool active_ = false;
		//定数バッファ
		ComPtr<ID3D12Resource> constBuff_;


	public://メンバ関数

		//ライト方向セット
		void SetLightDir(const XMVECTOR& lightdir) { this->lightDir_ = DirectX::XMVector3Normalize(lightdir); }
		//ライト方向ゲット
		const XMVECTOR& GetLightDir() { return lightDir_; }

		//ライトカラーのセット
		void SetLightColor(const XMFLOAT3& lightcolor) { this->lightColor_ = lightcolor; }
		//ライトカラーのゲット
		const XMFLOAT3& GetLightColor() { return lightColor_; }

		//有効フラグのセット
		inline void SetActive(bool active) { this->active_ = active; }
		//有効かどうか
		inline bool IsActive() { return active_; }
	};
}
