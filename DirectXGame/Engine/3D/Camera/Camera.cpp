#include "Camera.h"
#include "MyMath.h"
#include "WinApp.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "XYZ.h"

using namespace DirectX;

/*

*	Camera.cpp

*	�J����

*/

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
	eye_ = presetEye_;
	// �����_���W
	target_ = presetTarget_;
	// ������x�N�g��
	up_ = presetUp_;

	// �r���[�s��̐���
	UpdateViewMatrix();

	// �������e�ɂ��ˉe�s��̐���
	UpdateProjectionMatrix();

	matViewProjection_ = matView_ * matProjection_;
}

XMFLOAT3 Camera::ShakeEye(const XMFLOAT3& eye, const int count, const XMFLOAT3& min, const XMFLOAT3& max)
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

XMFLOAT3 Camera::ShakeTarget(const XMFLOAT3& target, const int count, const XMFLOAT3& min, const XMFLOAT3& max)
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
	
	//W���Ƃ������̌Œ�ʒu
	const XMVECTOR cameraAxisW = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	//�J������]�s��
	XMMATRIX matCameraRot;
	
	//�J�������W�n�����[���h���W�n�̕ϊ��s��
	matCameraRot.r[XYZW_X] = cameraAxisX;
	matCameraRot.r[XYZW_Y] = cameraAxisY;
	matCameraRot.r[XYZW_Z] = cameraAxisZ;
	matCameraRot.r[XYZW_W] = cameraAxisW;

	//�]�u�ɂ��t�s��(�t��])���v�Z
	matView_ = XMMatrixTranspose(matCameraRot);

	//���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);	//X����
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);	//Y����
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);	//Z����
	const float translationW = 1.0f;
	//��̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[XYZW_X], tY.m128_f32[XYZW_Y],
		tZ.m128_f32[XYZW_Z], translationW);

	//�r���[�s��ɕ��s�ړ�������ݒ�
	matView_.r[XYZW_W] = translation;

#pragma region �S�����r���{�[�h�s��̌v�Z
	//�r���{�[�h�s��
	matBillboard_.r[XYZW_X] = cameraAxisX;
	matBillboard_.r[XYZW_Y] = cameraAxisY;
	matBillboard_.r[XYZW_Z] = cameraAxisZ;
	matBillboard_.r[XYZW_W] = cameraAxisW;
#pragma endregion

#pragma region Y�����r���{�[�h�s��̌v�Z
	//�J����X,Y,Z,W��
	XMVECTOR yBillCameraAxisX, yBillCameraAxisY, yBillCameraAxisZ, yBillCameraAxisW;

	//X���͋���
	yBillCameraAxisX = cameraAxisX;
	//Y���̓��[���h���W�n��Y��
	yBillCameraAxisY = XMVector3Normalize(upVector);
	//Z����X����Y���̊O�ςŌ��܂�
	yBillCameraAxisZ = XMVector3Cross(yBillCameraAxisX, yBillCameraAxisY);
	//W���͂��������Œ�Ȃ̂ŋ���(�}�W�b�N�i���o�[�������Ă����l)
	yBillCameraAxisW = cameraAxisW;
	//Y�����r���{�[�h�s��
	matBillboardY_.r[XYZW_X] = yBillCameraAxisX;
	matBillboardY_.r[XYZW_Y] = yBillCameraAxisY;
	matBillboardY_.r[XYZW_Z] = yBillCameraAxisZ;
	matBillboardY_.r[XYZW_W] = yBillCameraAxisW;

#pragma endregion

}
void Camera::UpdateProjectionMatrix()
{
	struct ProjectionPreset
	{
		const float degree = 45.0f;
		const float fovAngleY = XMConvertToRadians(degree);
		const float aspectRatio = (float)WinApp::WINDOW_WIDTH / WinApp::WINDOW_HEIGHT;
		const float nearZ = 0.1f;
		const float farZ = 1000.0f;
	};
	ProjectionPreset projectionPreset;
	matProjection_ = XMMatrixPerspectiveFovLH(
		projectionPreset.fovAngleY, projectionPreset.aspectRatio,
		projectionPreset.nearZ, projectionPreset.farZ
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

void Camera::DebugCamera(const bool eyeTargetMix)
{
	Input* input_ = Input::GetInstance();
	const int VECTOR3COUNT = 3;

	//ImGui�ɓn���p�̕ϐ�
	float ieye[VECTOR3COUNT] = { eye_.x,eye_.y,eye_.z };
	float itarget[VECTOR3COUNT] = { target_.x,target_.y,target_.z };
	//�X�s�[�h
	struct CameraSpeed
	{
		const XMFLOAT3 eyeXPlus = { 1.0f,0.0f,0.0f };
		const XMFLOAT3 eyeYPlus = { 0.0f,1.0f,0.0f };
		const XMFLOAT3 eyeZPlus = { 0.0f,0.0f,1.0f };
		const XMFLOAT3 eyeXMinus = { -1.0f,0.0f,0.0f };
		const XMFLOAT3 eyeYMinus = { 0.0f,-1.0f,0.0f };
		const XMFLOAT3 eyeZMinus = { 0.0f,0.0f,-1.0f };

		const XMFLOAT3 targetXPlus = { 1.0f,0.0f,0.00001f };
		const XMFLOAT3 targetYPlus = { 0.0f,1.0f,0.00001f };
		const XMFLOAT3 targetZPlus = { 0.0f,0.0f,1.00001f };
		const XMFLOAT3 targetXMinus = { -1.0f,0.0f,-0.00001f };
		const XMFLOAT3 targetYMinus = { 0.0f,-1.0f,-0.00001f };
		const XMFLOAT3 targetZMinus = { 0.0f,0.0f,-1.00001f };
	};
	CameraSpeed speed;

	//�E�B���h�E�|�W�V����
	struct ImGuiWindowPosition
	{
		const float X = 700.0f;
		const float Y = 0.0f;
	};
	ImGuiWindowPosition iPos;
	//�E�B���h�E�T�C�Y
	struct ImguiWindowSize
	{
		const float width = 560.0f;
		const float height = 150.0f;
	};
	ImguiWindowSize iSize;
	
	if (!eyeTargetMix)
	{
		//���_
		if (input_->PushKey(DIK_W))CameraMoveVectorEye(speed.eyeYPlus);
		else if (input_->PushKey(DIK_S))CameraMoveVectorEye(speed.eyeYMinus);
		else if (input_->PushKey(DIK_A))CameraMoveVectorEye(speed.eyeXMinus);
		else if (input_->PushKey(DIK_D))CameraMoveVectorEye(speed.eyeXPlus);
		else if (input_->PushKey(DIK_Z))CameraMoveVectorEye(speed.eyeZPlus);
		else if (input_->PushKey(DIK_X))CameraMoveVectorEye(speed.eyeZMinus);

		//�����_
		if (input_->PushKey(DIK_UP))CameraMoveVectorTarget(speed.targetYPlus);
		else if (input_->PushKey(DIK_DOWN))CameraMoveVectorTarget(speed.targetYMinus);
		else if (input_->PushKey(DIK_LEFT))CameraMoveVectorTarget(speed.targetXMinus);
		else if (input_->PushKey(DIK_RIGHT))CameraMoveVectorTarget(speed.targetXPlus);
		else if (input_->PushKey(DIK_Q))CameraMoveVectorTarget(speed.targetZPlus);
		else if (input_->PushKey(DIK_E))CameraMoveVectorTarget(speed.targetZMinus);

		//imguiManager_->Begin();
		ImGui::Begin("Camera");
		ImGui::SetWindowPos(ImVec2(iPos.X, iPos.Y));
		ImGui::SetWindowSize(ImVec2(iSize.width, iSize.height));
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
			CameraMoveVectorEye(speed.eyeYPlus);
			CameraMoveVectorTarget(speed.targetYPlus);
		}
		else if (input_->PushKey(DIK_S))
		{
			CameraMoveVectorEye(speed.eyeYMinus);
			CameraMoveVectorTarget(speed.targetYMinus);
		}
		else if (input_->PushKey(DIK_A))
		{
			CameraMoveVectorEye(speed.eyeXMinus);
			CameraMoveVectorTarget(speed.targetXMinus);
		}
		else if (input_->PushKey(DIK_D))
		{
			CameraMoveVectorEye(speed.eyeXPlus);
			CameraMoveVectorTarget(speed.targetXPlus);
		}
		else if (input_->PushKey(DIK_Z))
		{
			CameraMoveVectorEye(speed.eyeZPlus);
			CameraMoveVectorTarget(speed.targetZPlus);
		}
		else if (input_->PushKey(DIK_X))
		{
			CameraMoveVectorEye(speed.eyeZMinus);
			CameraMoveVectorTarget(speed.targetZMinus);
		}

		//imguiManager_->Begin();
		ImGui::Begin("Camera Mix Mode");
		ImGui::SetWindowPos(ImVec2(iPos.X, iPos.Y));
		ImGui::SetWindowSize(ImVec2(iSize.width, iSize.height));
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
