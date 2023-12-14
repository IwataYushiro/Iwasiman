#include "Matrix4.h"
#include "Vector3.h"
#include <cmath>
using namespace IwasiEngine;
/*

*	Matrix4.cpp

*	Matrix4(4x4�s��)�\����

*/

//�P�ʍs������߂�
Matrix4 identity()
{
	static const Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

//�g��k���̐ݒ�
Matrix4 scale(const Vector3& s)
{
	//�v���Z�b�g
	const Matrix4 preset
	{
		s.x,0.0f,0.0f,0.0f,
		0.0f,s.y,0.0f,0.0f,
		0.0f,0.0f,s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	Matrix4 result = preset;

	return result;
}

//��]
Matrix4 rotateX(const float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	//�v���Z�b�g
	const Matrix4 preset
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos,sin,0.0f,
		0.0f,-sin,cos,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	Matrix4 result = preset;

	return result;
}

Matrix4 rotateY(const float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	//�v���Z�b�g
	const Matrix4 preset
	{
		cos,0.0f,-sin,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		sin,0.0f,cos,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	Matrix4 result = preset;

	return result;
}


Matrix4 rotateZ(const float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	//�v���Z�b�g
	const Matrix4 preset
	{
		cos,sin,0.0f,0.0f,
		-sin,cos,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	Matrix4 result = preset;

	return result;
}


//���s�ړ�
Matrix4 translate(const Vector3& t)
{
	//�v���Z�b�g
	const Matrix4 preset
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		t.x,t.y,t.z,1.0f
	};
	Matrix4 result = preset;

	return result;
}

//���W�ϊ�
Vector3 transform(const Vector3& v, const Matrix4& m)
{
	float w = v.x * m.m[XYZW_X][XYZW_W] + v.y * m.m[XYZW_Y][XYZW_W] + v.z * m.m[XYZW_Z][XYZW_W] + m.m[XYZW_W][XYZW_W];

	Vector3 result
	{
		(v.x * m.m[XYZW_X][XYZW_X] + v.y * m.m[XYZW_Y][XYZW_X] + v.z * m.m[XYZW_Z][XYZW_X] + m.m[XYZW_W][XYZW_X]) / w,
		(v.x * m.m[XYZW_X][XYZW_Y] + v.y * m.m[XYZW_Y][XYZW_Y] + v.z * m.m[XYZW_Z][XYZW_Y] + m.m[XYZW_W][XYZW_Y]) / w,
		(v.x * m.m[XYZW_X][XYZW_Z] + v.y * m.m[XYZW_Y][XYZW_Z] + v.z * m.m[XYZW_Z][XYZW_Z] + m.m[XYZW_W][XYZW_Z]) / w
	};

	return result;
}


//������Z�q�I�[�o�[���[�h
Matrix4& operator*=(Matrix4& m1, Matrix4& m2)
{
	Matrix4 result;

	for (int i = 0; i < XYZW_Num; i++)
	{
		for (int j = 0; j < XYZW_Num; j++)
		{

			for (int k = 0; k < XYZW_Num; k++)
			{
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}

	m1 = result;

	return m1;
}

//2�����Z�q�I�[�o�[���[�h
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result = m1;
	Matrix4 result2 = m2;

	return result *= result2;
}

const Vector3 operator*(const Vector3& v, const Matrix4& m)
{
	return transform(v, m);
}