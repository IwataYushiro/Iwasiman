#pragma once
#include "Input.h"
#include <DirectXMath.h>

namespace MyMath {
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	const float PI = 3.141592654f;

	// 零ベクトルを返す
	const XMFLOAT3 MySetVector3Zero();
	// 2ベクトルが一致しているか調べる
	bool MyVector3Equal(const XMFLOAT3& v1, const XMFLOAT3& v2);
	// ノルム(長さ)を求める
	float MyVector3Length(const XMFLOAT3& v);
	// 正規化する
	XMFLOAT3& MyVector3Normalize(XMFLOAT3& v);
	// 内積を求める
	float MyVector3Dot(const XMFLOAT3& v1, const XMFLOAT3& v2);
	// 外積を求める
	XMFLOAT3 MyVector3Cross(const XMFLOAT3& v1, const XMFLOAT3& v2);
	//度を求める
	float GetDegree(float r);
	//ラジアンを求める
	float GetRadian(float d);

	// 2項演算子オーバーロード
	const XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2);
	const XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2);
	const XMFLOAT3 operator*(const XMFLOAT3& v, float s);
	const XMFLOAT3 operator*(float s, const XMFLOAT3& v);
	const XMFLOAT3 operator/(const XMFLOAT3& v, float s);

	// 単位行列を求める
	XMMATRIX MySetMatrix4Identity();
	// 転置行列を求める
	XMMATRIX MyMatrix4Transpose(const XMMATRIX& m);
	//逆行列を求める
	XMMATRIX MyMatrix4Inverse(const XMMATRIX& m, float* det = nullptr);

	// 拡大縮小行列の作成
	XMMATRIX MyGenMatrix4Scaling(XMFLOAT3 scale);

	// 回転行列の作成
	XMMATRIX MyGenMatrix4RotationX(float angle);
	XMMATRIX MyGenMatrix4RotationY(float angle);
	XMMATRIX MyGenMatrix4RotationZ(float angle);

	//合成済み回転行列の作成
	XMMATRIX MySynMatrix4Rotation(XMFLOAT3 rotation);

	// 平行移動行列の作成
	XMMATRIX MyGenMatrix4Translation(XMFLOAT3 translation);

	//ワールド行列の作成
	XMMATRIX MySynMatrix4WorldTransform(WorldTransform worldTransform);

	//視点移動ベクトルを変更
	XMFLOAT3 MyChanVector3ViewEye(Input* key);
	//注視点移動ベクトルを変更
	XMFLOAT3 MyChanVector3ViewTarget(Input* key);
	//上方向ベクトルを変更
	XMFLOAT3 MyChanVector3ViewUp(Input* key, float& upAngle);
	// FoV変更
	float MyChanProjectionFovAngleY(Input* key, float fovY);
	// ニアクリップ距離の変更
	float MyChanProfectionNearClipZ(Input* key, float nearZ);

	// ビュー行列の作成
	XMMATRIX MyMatrix4LookAtLH(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up);
	// 並行投影行列の作成
	XMMATRIX MyMatrix4Orthographic(
		float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ);
	// 透視投影行列の作成
	XMMATRIX MyMatrix4Perspective(float fovAngleY, float aspectRatio, float nearZ, float farZ);

	// 座標変換（w除算なし）
	XMFLOAT3 MyVector3Transform(const XMFLOAT3& v, const XMMATRIX& m);
	// 座標変換（w除算あり）
	XMFLOAT3 MyVector3TransformCoord(const XMFLOAT3& v, const XMMATRIX& m);
	// ベクトル変換
	XMFLOAT3 MyVector3TransformNormal(XMFLOAT3 vector, XMMATRIX matrix);

	// 2項演算子オーバーロード
	XMMATRIX operator*(const XMMATRIX& m1, const XMMATRIX& m2);
	XMFLOAT3 operator*(const XMFLOAT3& v, const XMMATRIX& m);
}
