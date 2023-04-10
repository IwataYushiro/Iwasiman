#pragma once
#include <vector>

class Vector3
{
public:
	float x;//x����
	float y;//y����
	float z;//z����

public:
	//�R���X�g���N�^
	Vector3();								//��x�N�g���Ƃ��Đ���
	Vector3(float x, float y, float z);		//x�����Ay�����Az�������w�肵�Đ���

	//�����o�֐�
	float length() const;					//�m����(����)�����߂�
	Vector3& normalize();					//���K������
	float dot(const Vector3& v) const;		//���ς����߂�
	Vector3 cross(const Vector3& v) const;	//�O�ς����߂�
	Vector3 cross2(const Vector3& v1,const Vector3& v2);	//�O�ς����߂�
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

//2�����Z�q�I�[�o�[���[�h
//�p�^�[���̏���
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);
const Vector3 operator/(const Vector3& v, float s);

const Vector3 lerp(const Vector3& start, const Vector3& end, const float t);
const Vector3 Bezier2(const Vector3& p0, const Vector3& p1, const Vector3& p2, const float t);
const Vector3 Bezier3(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float t);
//����_�̏W��(Vector�R���e�i)�A��Ԃ����Ԃ̓Y���A���Ԍo�ߗ�
Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float t);