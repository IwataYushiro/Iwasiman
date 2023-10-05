#include "Camera.h"
#include "MyMath.h"
#include "WinApp.h"
#include "Input.h"
#include "ImGuiManager.h"

using namespace DirectX;
Camera::Camera()
{
	// �r���[�s��̐���
	UpdateViewMatrix();

	// �������e�ɂ��ˉe�s��̐���
	UpdateProjectionMatrix();

	matViewProjection_ = matView_ * matProjection_;

}

Camera::~Camera()
{

}

void Camera::Reset()
{
	// ���_���W
	eye_ = { -10.0f, 1.0f, -100.0f };
	// �����_���W
	target_ = { -10.0f,0.0f,0.0f };
	// ������x�N�g��
	up_ = { 0.0f,1.0f,0.0f };

	// �r���[�s��̐���
	UpdateViewMatrix();

	// �������e�ɂ��ˉe�s��̐���
	UpdateProjectionMatrix();

	matViewProjection_ = matView_ * matProjection_;
}

XMFLOAT3 Camera::ShakeEye(XMFLOAT3 eye, int count, XMFLOAT3 min, XMFLOAT3 max)
{
	const XMFLOAT3 nowEye = eye;
	//�v����A
	// ���_���W
	for (int i = 0; i < count; i++)
	{
		//if (i > count - 1)eye_ = { 0.0f, 6.0f, -115.0f };
		eye_ = { MyMath::RandomMTFloat(min.x,max.x), MyMath::RandomMTFloat(min.y,max.y), MyMath::RandomMTFloat(min.z,max.z) };
	}

	/*
	//�v����B
	// ���_���W
	eye_ = { 0.0f, 81.0f, -115.0f };
	// �����_���W
	target_ = { 0.0f,-35.0f,0.0f };
	*/

	// �r���[�s��̐���
	UpdateViewMatrix();

	// �������e�ɂ��ˉe�s��̐���
	UpdateProjectionMatrix();

	matViewProjection_ = matView_ * matProjection_;


	return nowEye;
}

XMFLOAT3 Camera::ShakeTarget(XMFLOAT3 target, int count, XMFLOAT3 min, XMFLOAT3 max)
{
	const XMFLOAT3 nowTarget = target;
	//�v����A
	// ���_���W
	for (int i = 0; i < count; i++)
	{
		//if (i > count - 1)target_ = { 0.0f, 5.0f, 0.0f };
		target_ = { MyMath::RandomMTFloat(min.x,max.x), MyMath::RandomMTFloat(min.y,max.y), MyMath::RandomMTFloat(min.z,max.z) };
	}

	/*
	//�v����B
	// ���_���W
	eye_ = { 0.0f, 81.0f, -115.0f };
	// �����_���W
	target_ = { 0.0f,-35.0f,0.0f };
	*/
	// �r���[�s��̐���
	UpdateViewMatrix();

	// �������e�ɂ��ˉe�s��̐���
	UpdateProjectionMatrix();

	matViewProjection_ = matView_ * matProjection_;

	return nowTarget;
}

void Camera::Update()
{
	// �r���[�s��̐���
	UpdateViewMatrix();

	// �������e�ɂ��ˉe�s��̐���
	UpdateProjectionMatrix();

	matViewProjection_ = matView_ * matProjection_;
}

void Camera::UpdateViewMatrix()
{
	// �r���[�s��̍X�V
	matView_ = XMMatrixLookAtLH(XMLoadFloat3(&eye_), XMLoadFloat3(&target_), XMLoadFloat3(&up_));
	//���_���W
	XMVECTOR eyePosition = XMLoadFloat3(&eye_);
	//�����_���W
	XMVECTOR targetPosition = XMLoadFloat3(&target_);
	//(����)�����
	XMVECTOR upVector = XMLoadFloat3(&up_);

	//�J����Z��(��������)
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	//0�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	//�x�N�g���𐳋K��
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//�J������X��(�E����)
	XMVECTOR cameraAxisX;
	//X���͏������Z���̊O�ςŌ��܂�
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//�x�N�g���𐳋K��
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	//�J������Y��(�����)
	XMVECTOR cameraAxisY;
	//Y����Z����X���̊O�ςŌ��܂�
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	//�J������]�s��
	XMMATRIX matCameraRot;
	//�J�������W�n�����[���h���W�n�̕ϊ��s��
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	//�]�u�ɂ��t�s��(�t��])���v�Z
	matView_ = XMMatrixTranspose(matCameraRot);

	//���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);	//X����
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);	//Y����
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);	//Z����
	//��̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//�r���[�s��ɕ��s�ړ�������ݒ�
	matView_.r[3] = translation;

#pragma region �S�����r���{�[�h�s��̌v�Z
	//�r���{�[�h�s��
	matBillboard_.r[0] = cameraAxisX;
	matBillboard_.r[1] = cameraAxisY;
	matBillboard_.r[2] = cameraAxisZ;
	matBillboard_.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
#pragma endregion

#pragma region Y�����r���{�[�h�s��̌v�Z
	//�J����X,Y,Z��
	XMVECTOR yBillCameraAxisX, yBillCameraAxisY, yBillCameraAxisZ;

	//X���͋���
	yBillCameraAxisX = cameraAxisX;
	//Y���̓��[���h���W�n��Y��
	yBillCameraAxisY = XMVector3Normalize(upVector);
	//Z����X����Y���̊O�ςŌ��܂�
	yBillCameraAxisZ = XMVector3Cross(yBillCameraAxisX, yBillCameraAxisY);

	//Y�����r���{�[�h�s��
	matBillboardY_.r[0] = yBillCameraAxisX;
	matBillboardY_.r[1] = yBillCameraAxisY;
	matBillboardY_.r[2] = yBillCameraAxisZ;
	matBillboardY_.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

#pragma endregion

}
void Camera::UpdateProjectionMatrix()
{
	matProjection_ = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)WinApp::window_width / WinApp::window_height,
		0.1f, 1000.0f
	);
}
void Camera::CameraMoveVector(const XMFLOAT3& move)
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

void Camera::CameraMoveVectorEye(const XMFLOAT3& move)
{
	XMFLOAT3 eye_moved = eye_;

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	SetEye(eye_moved);
}

void Camera::CameraMoveVectorTarget(const XMFLOAT3& move)
{
	XMFLOAT3 target_moved = target_;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	SetTarget(target_moved);
}

void Camera::DebugCamera(bool eyeTargetMix)
{
	Input* input_ = Input::GetInstance();
	const int VECTOR3COUNT = 3;

	//ImGui�ɓn���p�̕ϐ�
	float ieye[VECTOR3COUNT] = { eye_.x,eye_.y,eye_.z };
	float itarget[VECTOR3COUNT] = { target_.x,target_.y,target_.z };
	if (!eyeTargetMix)
	{
		//���_
		if (input_->PushKey(DIK_W))CameraMoveVectorEye({ 0.0f,1.0f,0.0f });
		else if (input_->PushKey(DIK_S))CameraMoveVectorEye({ 0.0f,-1.0f,0.0f });
		else if (input_->PushKey(DIK_A))CameraMoveVectorEye({ -1.0f,0.0f,0.0f });
		else if (input_->PushKey(DIK_D))CameraMoveVectorEye({ 1.0f,0.0f,0.0f });
		else if (input_->PushKey(DIK_Z))CameraMoveVectorEye({ 0.0f,0.0f,1.0f });
		else if (input_->PushKey(DIK_X))CameraMoveVectorEye({ 0.0f,0.0f,-1.0f });

		//�����_
		if (input_->PushKey(DIK_UP))CameraMoveVectorTarget({ 0.0f,1.0f,0.00001f });
		else if (input_->PushKey(DIK_DOWN))CameraMoveVectorTarget({ 0.0f,-1.0f,0.00001f });
		else if (input_->PushKey(DIK_LEFT))CameraMoveVectorTarget({ -1.0f,0.0f,0.00001f });
		else if (input_->PushKey(DIK_RIGHT))CameraMoveVectorTarget({ 1.0f,0.0f,0.00001f });
		else if (input_->PushKey(DIK_Q))CameraMoveVectorTarget({ 0.0f,0.0f,1.0f });
		else if (input_->PushKey(DIK_E))CameraMoveVectorTarget({ 0.0f,0.0f,-1.0f });

		//imguiManager_->Begin();
		ImGui::Begin("Camera");
		ImGui::SetWindowPos(ImVec2(700, 0));
		ImGui::SetWindowSize(ImVec2(560, 150));
		ImGui::InputFloat3("Eye", ieye);
		ImGui::Text("eye W += x S -= x  D += y A -= y Z += z X -= z");
		ImGui::InputFloat3("Target", itarget);
		ImGui::Text("target RIGHT += x LEFT -= x  UP += y DOWN -= y Q += z E -= z");
		ImGui::Text("WARNING eye.x = target.x & eye.y = target.y & eye.z = target.z -> SYSTEM DOWN");
		ImGui::End();
		//imguiManager_->End();
	}
	else
	{
		//���_
		if (input_->PushKey(DIK_W))
		{
			CameraMoveVectorEye({ 0.0f,1.0f,0.0f });
			CameraMoveVectorTarget({ 0.0f,1.0f,0.00001f });
		}
		else if (input_->PushKey(DIK_S))
		{
			CameraMoveVectorEye({ 0.0f,-1.0f,0.0f });
			CameraMoveVectorTarget({ 0.0f,-1.0f,0.00001f });
		}
		else if (input_->PushKey(DIK_A))
		{
			CameraMoveVectorEye({ -1.0f,0.0f,0.0f });
			CameraMoveVectorTarget({ -1.0f,0.0f,0.00001f });
		}
		else if (input_->PushKey(DIK_D))
		{
			CameraMoveVectorEye({ 1.0f,0.0f,0.0f });
			CameraMoveVectorTarget({ 1.0f,0.0f,0.00001f });
		}
		else if (input_->PushKey(DIK_Z))
		{
			CameraMoveVectorEye({ 0.0f,0.0f,1.0f });
			CameraMoveVectorTarget({ 0.0f,0.0f,1.00001f });
		}
		else if (input_->PushKey(DIK_X))
		{
			CameraMoveVectorEye({ 0.0f,0.0f,-1.0f });
			CameraMoveVectorTarget({ 0.0f,0.0f,-1.00001f });
		}

		//imguiManager_->Begin();
		ImGui::Begin("Camera Mix Mode");
		ImGui::SetWindowPos(ImVec2(700, 0));
		ImGui::SetWindowSize(ImVec2(560, 150));
		ImGui::InputFloat3("Eye", ieye);
		ImGui::InputFloat3("Target", itarget);
		ImGui::Text("W += x S -= x  D += y A -= y Z += z X -= z");
		ImGui::End();
		//imguiManager_->End();
	}
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

Camera* Camera::GetInstance()
{
	static Camera instance;
	return &instance;
}
