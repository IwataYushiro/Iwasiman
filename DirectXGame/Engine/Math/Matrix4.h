#pragma once

#include "Vector3.h"
#include "XYZ.h"

namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	Matrix4.h

	*	Matrix4(4x4行列)構造体

	*/
	struct Matrix4
	{
		float m[XYZW_Num][XYZW_Num];
	};
}
	//単位行列を求める
	IwasiEngine::Matrix4 identity();

	//拡大縮小の設定
	IwasiEngine::Matrix4 scale(const IwasiEngine::Vector3& s);

	//回転
	IwasiEngine::Matrix4 rotateX(const float angle);//X軸
	IwasiEngine::Matrix4 rotateY(const float angle);//Y軸
	IwasiEngine::Matrix4 rotateZ(const float angle);//Z軸

	//平行移動
	IwasiEngine::Matrix4 translate(const IwasiEngine::Vector3& t);

	//座標変換
	IwasiEngine::Vector3 transform(const IwasiEngine::Vector3& v, const IwasiEngine::Matrix4& m);

	//代入演算子オーバーロード
	IwasiEngine::Matrix4& operator*=(IwasiEngine::Matrix4& m1, IwasiEngine::Matrix4& m2);

	//2項演算子オーバーロード
	const IwasiEngine::Matrix4 operator* (const IwasiEngine::Matrix4& m1, const IwasiEngine::Matrix4& m2);
	const IwasiEngine::Vector3 operator* (const IwasiEngine::Vector3& v, const IwasiEngine::Matrix4& m);
