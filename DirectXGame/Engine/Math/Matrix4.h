#pragma once

#include "Vector3.h"

//Matrix4 �\����
struct Matrix4
{
	float m[4][4];
};

//�P�ʍs������߂�
Matrix4 identity();

//�g��k���̐ݒ�
Matrix4 scale(const Vector3& s);

//��]
Matrix4 rotateX(float angle);
Matrix4 rotateY(float angle);
Matrix4 rotateZ(float angle);

//���s�ړ�
Matrix4 translate(const Vector3& t);

//���W�ϊ�
Vector3 transform(const Vector3& v, const Matrix4& m);

//������Z�q�I�[�o�[���[�h
Matrix4& operator*=(Matrix4& m1, Matrix4& m2);

//2�����Z�q�I�[�o�[���[�h
const Matrix4 operator* (const Matrix4& m1, const Matrix4& m2);
const Vector3 operator* (const Vector3& v, const Matrix4& m);