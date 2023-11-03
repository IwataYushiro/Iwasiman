#pragma once
#include <DirectXMath.h>
#include <wrl.h>

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
	XMFLOAT3 ShakeEye(XMFLOAT3 eye, int count, XMFLOAT3 min, XMFLOAT3 max);
	//注視点シェイク(注視点、シェイク時間、最小幅、最大幅)
	XMFLOAT3 ShakeTarget(XMFLOAT3 target, int count, XMFLOAT3 min, XMFLOAT3 max);
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
	void DebugCamera(bool eyeTargetMix = false);
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
	const XMFLOAT3 presetEye_ = { -10.0f, 0.0f, -100.0f };//プリセット
	XMFLOAT3 eye_ = presetEye_;
	// 注視点座標
	const XMFLOAT3 presetTarget_ = { -10.0f,0.0f,0.0f };//プリセット
	XMFLOAT3 target_ = presetTarget_;
	// 上方向ベクトル
	const XMFLOAT3 presetUp_ = { 0.0f,1.0f,0.0f };//プリセット
	XMFLOAT3 up_ = presetUp_;

public://アクセッサ
	//ビュー行列ゲット
	const XMMATRIX& GetMatView() { return matView_; }
	
	//プロジェクション行列ゲット
	const XMMATRIX& GetMatProjection() { return matProjection_; }
	//ビュプロ行列ゲット
	const XMMATRIX& GetMatViewProjection() { return matViewProjection_; }

	
	//ビルボード行列ゲット
	const XMMATRIX& GetMatBillboard() { return matBillboard_; }
	//Y軸回りビルボード行列ゲット
	const XMMATRIX& GetMatBillboardY() { return matBillboardY_; }

	//視点ゲット
	const XMFLOAT3& GetEye() { return eye_; }
	//視点セット
	void SetEye(const XMFLOAT3& eye);

	//注視点ゲット
	const XMFLOAT3& GetTarget() { return target_; }
	//注視点セット
	void SetTarget(const XMFLOAT3& target);

	//上方向ベクトルゲット
	const XMFLOAT3& GetUp() { return up_; }
	//上方向ベクトルセット
	void SetUp(const XMFLOAT3& up);

};
