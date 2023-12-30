#pragma once
#include <DirectXMath.h>
#include <wrl.h>

namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	Camera.h

	*	カメラ

	*/
	class Camera
	{
	private:
		// Microsoft::WRL::を省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::を省略
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMMATRIX = DirectX::XMMATRIX;
	public:
		//コンストラクタ
		Camera();
		//デストラクタ
		virtual ~Camera();
		//初期値にリセット
		void Reset();
		//シェイク
		//視点シェイク(視点、シェイク時間、最小幅、最大幅)
		XMFLOAT3 ShakeEye(const XMFLOAT3& eye, const int count, const XMFLOAT3& min, const XMFLOAT3& max);
		//注視点シェイク(注視点、シェイク時間、最小幅、最大幅)
		XMFLOAT3 ShakeTarget(const XMFLOAT3& target, const int count, const XMFLOAT3& min, const XMFLOAT3& max);
		//更新
		virtual void Update();
		//ビュー行列更新
		void UpdateViewMatrix();
		//プロジェクション行列更新
		void UpdateProjectionMatrix();
		//カメラ移動(移動値)
		void CameraMoveVector(const XMFLOAT3& move);
		//カメラ視点移動(移動値)
		void CameraMoveVectorEye(const XMFLOAT3& move);
		//カメラ注視点移動(移動値)
		void CameraMoveVectorTarget(const XMFLOAT3& move);
		//デバッグカメラ(視点と注視点を一緒に動かすか)
		void DebugCamera(const bool eyeTargetMix = false);
	protected:
		// ビュー行列
		XMMATRIX matView_ = {};
		// 射影行列
		XMMATRIX matProjection_ = {};
		//ビュプロ行列
		XMMATRIX matViewProjection_ = {};

		// ビルボード行列
		XMMATRIX matBillboard_ = {};
		// Y軸回りビルボード行列
		XMMATRIX matBillboardY_ = {};
		// 視点座標
		const XMFLOAT3& presetEye_ = { -10.0f, 1.0f, -100.0f };//プリセット
		XMFLOAT3 eye_ = { presetEye_.x,presetEye_.y,presetEye_.z };
		// 注視点座標
		const XMFLOAT3& presetTarget_ = { -10.0f,0.0f,0.0f };//プリセット
		XMFLOAT3 target_ = { presetTarget_.x,presetTarget_.y,presetTarget_.z };
		// 上方向ベクトル
		const XMFLOAT3& presetUp_ = { 0.0f,1.0f,0.0f };//プリセット
		XMFLOAT3 up_ = { presetUp_.x,presetUp_.y,presetUp_.z };

	public://アクセッサ
		//ビュー行列ゲット
		const XMMATRIX& GetMatView() const { return matView_; }

		//プロジェクション行列ゲット
		const XMMATRIX& GetMatProjection() const { return matProjection_; }
		//ビュプロ行列ゲット
		const XMMATRIX& GetMatViewProjection()const { return matViewProjection_; }


		//ビルボード行列ゲット
		const XMMATRIX& GetMatBillboard() const { return matBillboard_; }
		//Y軸回りビルボード行列ゲット
		const XMMATRIX& GetMatBillboardY()const { return matBillboardY_; }

		//視点ゲット
		const XMFLOAT3& GetEye()const { return eye_; }
		//視点セット
		void SetEye(const XMFLOAT3& eye);
		//初期視点ゲット
		const XMFLOAT3& GetStartEye()const { return presetEye_; }
		//注視点ゲット
		const XMFLOAT3& GetTarget()const { return target_; }
		//注視点セット
		void SetTarget(const XMFLOAT3& target);
		//初期注視点ゲット
		const XMFLOAT3& GetStartTarget()const { return presetTarget_; }
		//上方向ベクトルゲット
		const XMFLOAT3& GetUp()const { return up_; }
		//上方向ベクトルセット
		void SetUp(const XMFLOAT3& up);

	};
}