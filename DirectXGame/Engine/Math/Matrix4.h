#pragma once

#include "Vector3.h"
#include "XYZ.h"

/*

*	Matrix4.h

*	Matrix4(4x4�s��)�\����

*/
struct Matrix4
{
	float m[XYZW_Num][XYZW_Num];
};

//�P�ʍs������߂�
Matrix4 identity();

//�g��k���̐ݒ�
Matrix4 scale(const Vector3& s);

//��]
Matrix4 rotateX(const float angle);//X��
Matrix4 rotateY(const float angle);//Y��
Matrix4 rotateZ(const float angle);//Z��

//���s�ړ�
Matrix4 translate(const Vector3& t);

//���W�ϊ�
Vector3 transform(const Vector3& v, const Matrix4& m);

//������Z�q�I�[�o�[���[�h
Matrix4& operator*=(Matrix4& m1, Matrix4& m2);

//2�����Z�q�I�[�o�[���[�h
const Matrix4 operator* (const Matrix4& m1, const Matrix4& m2);
const Vector3 operator* (const Vector3& v, const Matrix4& m);