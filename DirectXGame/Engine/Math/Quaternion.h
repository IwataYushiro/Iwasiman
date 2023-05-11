#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class Quaternion
{
public:
	float x;//x成分
	float y;//y成分
	float z;//z成分
	float w;//w成分

public:
	//コンストラクタ
	Quaternion();								//零ベクトルとして生成
	Quaternion(Vector3 v,float w);
	Quaternion(float x, float y, float z, float w);		//x成分、y成分、z成分を指定して生成

	//内積(Vector3)
	float dotV(const Vector3& v1, const Vector3& v2);
	//内積
	float dotQ(const Quaternion& q1, const Quaternion& q2);
	//内積(w含む)
	float dotQW(const Quaternion& q1, const Quaternion& q2);
	//外積 
	Vector3 cross(const Vector3& q1, const Vector3& q2);
	//Quaternionの積
	Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
	//単位Quaternionを返す
	Quaternion Identity();
	//共役Quaternionを返す
	Quaternion Conjugate(const Quaternion& q);
	//Quaternionのnormを返す
	float norm(const Quaternion& q);
	//正規化したVector3を返す
	Vector3 Normalize(const Vector3& v);
	//正規化したQuaternionを返す
	Quaternion Normalize(const Quaternion& q);
	//逆Quaternionを返す
	Quaternion Inverse(const Quaternion& q);
	//任意軸回転を表すQuaternionの生成(angleの正規化は必須)
	Quaternion MakeAxisAngle(const Vector3& axis, float angle);
	//ベクトルをQuaternionで回転させた結果のベクトルを求める
	Vector3 RotateVector(const Vector3& v, const Quaternion& q);
	//Quaternionから回転行列を求める
	Matrix4 MakeRotateMatrix(const Quaternion& q);
	//球面線形補間
	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
	//uからvへの回転
	Quaternion DirectionToDirection(const Vector3& u, const Vector3& v);
	//単項演算子オーバーロード
	Quaternion operator+() const;
	Quaternion operator-() const;

	//代入演算子オーバーロード
	Quaternion& operator+=(const Quaternion& v);
	Quaternion& operator-=(const Quaternion& v);
	Quaternion& operator*=(float s);
	Quaternion& operator/=(float s);
};

//2項演算子オーバーロード
//パターンの準備
const Quaternion operator+(const Quaternion& v1, const Quaternion& v2);
const Quaternion operator-(const Quaternion& v1, const Quaternion& v2);
const Quaternion operator*(const Quaternion& v, float s);
const Quaternion operator*(const Quaternion& q, const Vector3& v);
const Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator*(const Vector3& v, const Quaternion& q);
const Quaternion operator*(float s, const Quaternion& v);
const Quaternion operator/(const Quaternion& v, float s);
