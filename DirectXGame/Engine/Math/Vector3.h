#pragma once
#include <vector>

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	Vector3.h

	*	x,y,z�̃x�N�g��3��

	*/
	class Vector3
	{
	public:
		float x;//x����
		float y;//y����
		float z;//z����

	public:
		//�R���X�g���N�^
		Vector3();								//��x�N�g���Ƃ��Đ���
		Vector3(const float x, const float y, const float z);		//x�����Ay�����Az�������w�肵�Đ���

		//�����o�֐�
		float length() const;					//�m����(����)�����߂�
		Vector3& normalize();					//���K������
		float dot(const Vector3& v) const;		//���ς����߂�
		Vector3 cross(const Vector3& v) const;	//�O�ς����߂�
		Vector3 cross2(const Vector3& v1, const Vector3& v2);	//�O�ς����߂�
		//��Ԋ֐�
		//���`���(1���֐����)

		//�P�����Z�q�I�[�o�[���[�h
		Vector3 operator+() const;
		Vector3 operator-() const;

		//������Z�q�I�[�o�[���[�h
		Vector3& operator+=(const Vector3& v);
		Vector3& operator-=(const Vector3& v);
		Vector3& operator*=(float s);
		Vector3& operator/=(float s);

	private:

	};
}
	//2�����Z�q�I�[�o�[���[�h
	//�p�^�[���̏���
	const IwasiEngine::Vector3 operator+(const IwasiEngine::Vector3& v1, const IwasiEngine::Vector3& v2);
	const IwasiEngine::Vector3 operator-(const IwasiEngine::Vector3& v1, const IwasiEngine::Vector3& v2);
	const IwasiEngine::Vector3 operator*(const IwasiEngine::Vector3& v, float s);
	const IwasiEngine::Vector3 operator*(float s, const IwasiEngine::Vector3& v);
	const IwasiEngine::Vector3 operator*(const IwasiEngine::Vector3& v1, const IwasiEngine::Vector3& v2);
	const IwasiEngine::Vector3 operator/(const IwasiEngine::Vector3& v, float s);

	const IwasiEngine::Vector3 lerp(const IwasiEngine::Vector3& start, const IwasiEngine::Vector3& end, const float t);
	const IwasiEngine::Vector3 Bezier2(const IwasiEngine::Vector3& p0, const IwasiEngine::Vector3& p1, const IwasiEngine::Vector3& p2, const float t);
	const IwasiEngine::Vector3 Bezier3(const IwasiEngine::Vector3& p0, const IwasiEngine::Vector3& p1, const IwasiEngine::Vector3& p2, const IwasiEngine::Vector3& p3, const float t);
	//����_�̏W��(Vector�R���e�i)�A��Ԃ����Ԃ̓Y���A���Ԍo�ߗ�
	IwasiEngine::Vector3 splinePosition(const std::vector<IwasiEngine::Vector3>& points, const size_t startIndex, const float t);
