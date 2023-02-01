#pragma once

#include "Vector3.h"

//Matrix4 構造体
struct Matrix4
{
	float m[4][4];
};

//単位行列を求める
Matrix4 identity();

//拡大縮小の設定
Matrix4 scale(const Vector3& s);

//回転
Matrix4 rotateX(float angle);
Matrix4 rotateY(float angle);
Matrix4 rotateZ(float angle);

//平行移動
Matrix4 translate(const Vector3& t);

//座標変換
Vector3 transform(const Vector3& v, const Matrix4& m);

//代入演算子オーバーロード
Matrix4& operator*=(Matrix4& m1, Matrix4& m2);

//2項演算子オーバーロード
const Matrix4 operator* (const Matrix4& m1, const Matrix4& m2);
const Vector3 operator* (const Vector3& v, const Matrix4& m);