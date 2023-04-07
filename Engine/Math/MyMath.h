#pragma once
#include "Input.h"
#include <DirectXMath.h>

namespace MyMath {
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	const float PI = 3.141592654f;

	// ��x�N�g����Ԃ�
	const XMFLOAT3 MySetVector3Zero();
	// 2�x�N�g������v���Ă��邩���ׂ�
	bool MyVector3Equal(const XMFLOAT3& v1, const XMFLOAT3& v2);
	// �m����(����)�����߂�
	float MyVector3Length(const XMFLOAT3& v);
	// ���K������
	XMFLOAT3& MyVector3Normalize(XMFLOAT3& v);
	// ���ς����߂�
	float MyVector3Dot(const XMFLOAT3& v1, const XMFLOAT3& v2);
	// �O�ς����߂�
	XMFLOAT3 MyVector3Cross(const XMFLOAT3& v1, const XMFLOAT3& v2);
	//�x�����߂�
	float GetDegree(float r);
	//���W�A�������߂�
	float GetRadian(float d);

	// 2�����Z�q�I�[�o�[���[�h
	const XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2);
	const XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2);
	const XMFLOAT3 operator*(const XMFLOAT3& v, float s);
	const XMFLOAT3 operator*(float s, const XMFLOAT3& v);
	const XMFLOAT3 operator/(const XMFLOAT3& v, float s);

	// �P�ʍs������߂�
	XMMATRIX MySetMatrix4Identity();
	// �]�u�s������߂�
	XMMATRIX MyMatrix4Transpose(const XMMATRIX& m);
	//�t�s������߂�
	XMMATRIX MyMatrix4Inverse(const XMMATRIX& m, float* det = nullptr);

	// �g��k���s��̍쐬
	XMMATRIX MyGenMatrix4Scaling(XMFLOAT3 scale);

	// ��]�s��̍쐬
	XMMATRIX MyGenMatrix4RotationX(float angle);
	XMMATRIX MyGenMatrix4RotationY(float angle);
	XMMATRIX MyGenMatrix4RotationZ(float angle);

	//�����ς݉�]�s��̍쐬
	XMMATRIX MySynMatrix4Rotation(XMFLOAT3 rotation);

	// ���s�ړ��s��̍쐬
	XMMATRIX MyGenMatrix4Translation(XMFLOAT3 translation);

	//���[���h�s��̍쐬
	XMMATRIX MySynMatrix4WorldTransform(WorldTransform worldTransform);

	//���_�ړ��x�N�g����ύX
	XMFLOAT3 MyChanVector3ViewEye(Input* key);
	//�����_�ړ��x�N�g����ύX
	XMFLOAT3 MyChanVector3ViewTarget(Input* key);
	//������x�N�g����ύX
	XMFLOAT3 MyChanVector3ViewUp(Input* key, float& upAngle);
	// FoV�ύX
	float MyChanProjectionFovAngleY(Input* key, float fovY);
	// �j�A�N���b�v�����̕ύX
	float MyChanProfectionNearClipZ(Input* key, float nearZ);

	// �r���[�s��̍쐬
	XMMATRIX MyMatrix4LookAtLH(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up);
	// ���s���e�s��̍쐬
	XMMATRIX MyMatrix4Orthographic(
		float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ);
	// �������e�s��̍쐬
	XMMATRIX MyMatrix4Perspective(float fovAngleY, float aspectRatio, float nearZ, float farZ);

	// ���W�ϊ��iw���Z�Ȃ��j
	XMFLOAT3 MyVector3Transform(const XMFLOAT3& v, const XMMATRIX& m);
	// ���W�ϊ��iw���Z����j
	XMFLOAT3 MyVector3TransformCoord(const XMFLOAT3& v, const XMMATRIX& m);
	// �x�N�g���ϊ�
	XMFLOAT3 MyVector3TransformNormal(XMFLOAT3 vector, XMMATRIX matrix);

	// 2�����Z�q�I�[�o�[���[�h
	XMMATRIX operator*(const XMMATRIX& m1, const XMMATRIX& m2);
	XMFLOAT3 operator*(const XMFLOAT3& v, const XMMATRIX& m);
}
