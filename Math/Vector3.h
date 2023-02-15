#pragma once
#include <vector>

class Vector3
{
public:
	float x;//x成分
	float y;//y成分
	float z;//z成分

public:
	//コンストラクタ
	Vector3();								//零ベクトルとして生成
	Vector3(float x, float y, float z);		//x成分、y成分、z成分を指定して生成

	//メンバ関数
	float length() const;					//ノルム(長さ)を求める
	Vector3& normalize();					//正規化する
	float dot(const Vector3& v) const;		//内積を求める
	Vector3 cross(const Vector3& v) const;	//外積を求める
	Vector3 cross2(const Vector3& v1,const Vector3& v2);	//外積を求める
	//補間関数
	//線形補間(1次関数補間)

	//単項演算子オーバーロード
	Vector3 operator+() const;
	Vector3 operator-() const;

	//代入演算子オーバーロード
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);

private:

};

//2項演算子オーバーロード
//パターンの準備
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);
const Vector3 operator/(const Vector3& v, float s);

const Vector3 lerp(const Vector3& start, const Vector3& end, const float t);
const Vector3 Bezier2(const Vector3& p0, const Vector3& p1, const Vector3& p2, const float t);
const Vector3 Bezier3(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float t);
//制御点の集合(Vectorコンテナ)、補間する区間の添字、時間経過率
Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float t);