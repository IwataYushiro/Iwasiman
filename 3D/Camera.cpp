#include "Camera.h"
#include "WinApp.h"

using namespace DirectX;
Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Initialize()
{
	// ビュー行列の生成
	UpdateViewMatrix();

	// 透視投影による射影行列の生成
	UpdateProjectionMatrix();
}

void Camera::Update()
{
	// ビュー行列の生成
	UpdateViewMatrix();

	// 透視投影による射影行列の生成
	UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix()
{
	// ビュー行列の更新
	matView_ = XMMatrixLookAtLH(XMLoadFloat3(&eye_), XMLoadFloat3(&target_), XMLoadFloat3(&up_));

}
void Camera::UpdateProjectionMatrix()
{
	matProjection_ = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)WinApp::window_width / WinApp::window_height,
		0.1f, 1000.0f
	);
}
void Camera::CameraMoveVector(XMFLOAT3 move)
{
	XMFLOAT3 eye_moved = eye_;
	XMFLOAT3 target_moved = target_;

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	SetEye(eye_moved);
	SetTarget(target_moved);
}



void Camera::SetEye(const XMFLOAT3& eye)
{
	this->eye_ = eye;
}

void Camera::SetTarget(const XMFLOAT3& target) {
	this->target_ = target;
}

void Camera::SetUp(const XMFLOAT3& up)
{
	this->up_ = up; 
}
