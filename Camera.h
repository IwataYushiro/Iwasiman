#pragma once
#include "ViewProjection.h"
#include <DirectXMath.h>

class Camera
{
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public://メンバ関数
	//コンストラクタ等
	Camera();
	virtual ~Camera();
	//更新
	virtual void Update();

	//ベクトルによる移動
	void CameraMoveVector(const XMFLOAT3& move);
	//ベクトルによる視点移動
	void CameraMoveEyeVector(const XMFLOAT3& move);

protected://メンバ変数
	//ビュプロ
	ViewProjection viewProjection_;
	//ビュプロ行列
	XMMATRIX matViewProjection_;

	//ダーティフラグ類
	bool viewDirty_ = false;
	bool projectionDirty_ = false;

public://ゲッター、セッター等
	//ビュー行列の取得
	const XMMATRIX& GetView() { return viewProjection_.matView_; }
	//プロジェクション行列の取得
	const XMMATRIX& GetProjection() { return viewProjection_.matProjection_; }
	//ビュプロ行列の取得
	const XMMATRIX& GetViewProjection() { return viewProjection_.matViewProjection_; }
	//ビルボード行列の取得
	const XMMATRIX& GetBillboard() { return viewProjection_.matBillboard_; }
	//Y軸回りビルボード行列の取得
	const XMMATRIX& GetBillboardY() { return viewProjection_.matBillboardY_; }

	//視点座標
	const XMFLOAT3& GetEye() { return viewProjection_.eye_; }
	void SetEye(XMFLOAT3 eye);
	//注視点座標
	const XMFLOAT3& GetTarget() { return viewProjection_.target_; }
	void SetTarget(XMFLOAT3 target);
	//上方向ベクトル座標
	const XMFLOAT3& GetUp() { return viewProjection_.up_; }
	void SetUp(XMFLOAT3 up);
};

