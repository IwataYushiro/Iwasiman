#include "Matrix4.h"
#include "Vector3.h"
#include <cmath>
using namespace IwasiEngine;
/*

*	Matrix4.cpp

*	Matrix4(4x4行列)構造体

*/

//単位行列を求める
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

//拡大縮小の設定
Matrix4 scale(const Vector3& s)
{
	//プリセット
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

//回転
Matrix4 rotateX(const float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	//プリセット
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

	//プリセット
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

	//プリセット
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


//平行移動
Matrix4 translate(const Vector3& t)
{
	//プリセット
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

//座標変換
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


//代入演算子オーバーロード
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

//2項演算子オーバーロード
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