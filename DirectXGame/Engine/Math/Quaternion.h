#pragma once
#include "Vector3.h"
#include "Matrix4.h"

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	Quaternion.h

	*	�N�I�[�^�j�I��

	*/
	class Quaternion
	{
	public:
		float x;//x����
		float y;//y����
		float z;//z����
		float w;//w����

	public:
		//�R���X�g���N�^
		Quaternion();								//��x�N�g���Ƃ��Đ���
		Quaternion(const Vector3& v, const float w);
		Quaternion(const float x, const float y, const float z, const float w);		//x�����Ay�����Az�������w�肵�Đ���

		//����(Vector3)
		float dotV(const Vector3& v1, const Vector3& v2);
		//����
		float dotQ(const Quaternion& q1, const Quaternion& q2);
		//����(w�܂�)
		float dotQW(const Quaternion& q1, const Quaternion& q2);
		//�O�� 
		Vector3 cross(const Vector3& q1, const Vector3& q2);
		//Quaternion�̐�
		Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
		//�P��Quaternion��Ԃ�
		Quaternion Identity();
		//����Quaternion��Ԃ�
		Quaternion Conjugate(const Quaternion& q);
		//Quaternion��norm��Ԃ�
		float norm(const Quaternion& q);
		//���K������Vector3��Ԃ�
		Vector3 Normalize(const Vector3& v);
		//���K������Quaternion��Ԃ�
		Quaternion Normalize(const Quaternion& q);
		//�tQuaternion��Ԃ�
		Quaternion Inverse(const Quaternion& q);
		//�C�ӎ���]��\��Quaternion�̐���(angle�̐��K���͕K�{)
		Quaternion MakeAxisAngle(const Vector3& axis, float angle);
		//�x�N�g����Quaternion�ŉ�]���������ʂ̃x�N�g�������߂�
		Vector3 RotateVector(const Vector3& v, const Quaternion& q);
		//Quaternion�����]�s������߂�
		Matrix4 MakeRotateMatrix(const Quaternion& q);
		//���ʐ��`���
		Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
		//u����v�ւ̉�]
		Quaternion DirectionToDirection(const Vector3& u, const Vector3& v);
		//�P�����Z�q�I�[�o�[���[�h
		Quaternion operator+() const;
		Quaternion operator-() const;

		//������Z�q�I�[�o�[���[�h
		Quaternion& operator+=(const Quaternion& v);
		Quaternion& operator-=(const Quaternion& v);
		Quaternion& operator*=(float s);
		Quaternion& operator/=(float s);
	};
}
	//2�����Z�q�I�[�o�[���[�h
	//�p�^�[���̏���
	const IwasiEngine::Quaternion operator+(const IwasiEngine::Quaternion& v1, const IwasiEngine::Quaternion& v2);
	const IwasiEngine::Quaternion operator-(const IwasiEngine::Quaternion& v1, const IwasiEngine::Quaternion& v2);
	const IwasiEngine::Quaternion operator*(const IwasiEngine::Quaternion& v, float s);
	const IwasiEngine::Quaternion operator*(const IwasiEngine::Quaternion& q, const IwasiEngine::Vector3& v);
	const IwasiEngine::Quaternion operator*(const IwasiEngine::Quaternion& q1, const IwasiEngine::Quaternion& q2);
	const IwasiEngine::Quaternion operator*(const IwasiEngine::Vector3& v, const IwasiEngine::Quaternion& q);
	const IwasiEngine::Quaternion operator*(float s, const IwasiEngine::Quaternion& v);
	const IwasiEngine::Quaternion operator/(const IwasiEngine::Quaternion& v, float s);
