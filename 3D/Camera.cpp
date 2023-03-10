#include "Camera.h"

Camera::Camera()
{
	//初期化
	viewProjection_.Initialize();

	//行列計算
	viewProjection_.Update();

	//合成
	matViewProjection_ = viewProjection_.matView_ * viewProjection_.matProjection_;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	//ダーティフラグが有効なら
	if (viewDirty_ || projectionDirty_)
	{
		//再計算が必要なら
		if (viewDirty_)
		{
			//ビュー行列計算
			viewProjection_.UpdateView();
			viewDirty_ = false;
		}
		//再計算が必要なら
		if (projectionDirty_)
		{
			//プロジェクション行列計算
			viewProjection_.UpdateProjection();
			projectionDirty_ = false;
		}
		//合成
		matViewProjection_ = viewProjection_.matView_ * viewProjection_.matProjection_;
	}

	//転送
	viewProjection_.Trans();
}

void Camera::CameraMoveVector(const XMFLOAT3& move)
{
	XMFLOAT3 eye_moved = GetEye();
	XMFLOAT3 target_moved = GetTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	SetEye(eye_moved);
	SetTarget(target_moved);
}
void Camera::CameraMoveEyeVector(const XMFLOAT3& move)
{
	XMFLOAT3 eye_moved = GetEye();
	
	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	SetEye(eye_moved);
}

void Camera::SetEye(const XMFLOAT3& eye)
{
	viewProjection_.eye_ = eye;

	//変更したらtrue
	viewDirty_ = true;
}

void Camera::SetTarget(const XMFLOAT3& target)
{
	viewProjection_.target_ = target;

	//変更したらtrue
	viewDirty_ = true;
}

void Camera::SetUp(const XMFLOAT3& up)
{
	viewProjection_.up_ = up;

	//変更したらtrue
	viewDirty_ = true;
}
