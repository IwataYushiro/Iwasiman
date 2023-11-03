#pragma once
#include <DirectXMath.h>
#include "XYZ.h"
/*

*	CollisionPrimitive.h

*	当たり判定プリミティブ形状

*/
//AABB(箱)
struct AABB
{
	const DirectX::XMVECTOR presetCenter = { 0.0f,0.0f,0.0f,1.0f };//プリセット
	DirectX::XMVECTOR center = presetCenter;
	float radius[XYZ_Num];
};

//球
struct Sphere
{
	//中心座標
	const DirectX::XMVECTOR presetCenter = { 0.0f,0.0f,0.0f,1.0f };//プリセット
	DirectX::XMVECTOR center = presetCenter;
	//半径
	const float presetRadius = 1.0f;//プリセット
	float radius = presetRadius;
};
//平面
struct Plane
{
	//法線ベクトル
	const DirectX::XMVECTOR presetNormal = { 0.0f,1.0f,0.0f,0.0f };//プリセット
	DirectX::XMVECTOR normal = presetNormal;
	//原点からの座標
	const float presetDistance = 0.0f;//プリセット
	float distance = presetDistance;
};
//三角形
struct Triangle
{
	//頂点座標3つ
	DirectX::XMVECTOR	p0;
	DirectX::XMVECTOR	p1;
	DirectX::XMVECTOR	p2;

	//法線ベクトル
	DirectX::XMVECTOR	normal;

	//法線の計算
	void ComputeNormal();
};
//レイ(半直線)
struct Ray
{
	//始点座標
	const DirectX::XMVECTOR	presetStart = { 0.0f,0.0f,0.0f,1.0f };//プリセット
	DirectX::XMVECTOR	start = presetStart;
	//方向
	const DirectX::XMVECTOR	presetDir = { 1.0f,0.0f,0.0f,0.0f };//プリセット
	DirectX::XMVECTOR	dir = presetDir;
};