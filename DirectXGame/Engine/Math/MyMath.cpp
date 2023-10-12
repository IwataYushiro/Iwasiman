#include "MyMath.h"
#include <random>

using namespace DirectX;

/*

*	MyMath.cpp

*	便利数学

*/

////零ベクトルを返す
//const XMFLOAT3 MyMathUtility::MySetVector3Zero() {
//	XMFLOAT3 v = { 0.0f, 0.0f, 0.0f };
//
//	return v;
//}
//
//// ノルム(長さ)を求める
//float MyMathUtility::MyVector3Length(const XMFLOAT3& v)
//{
//	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
//}
////正規化する
//XMFLOAT3& MyMath::MyVector3Normalize(XMFLOAT3& v)
//{
//	float len = MyVector3Length(v);
//	if (len != 0)
//	{
//		return v /= len;
//	}
//
//	return v;
//}
////度を求める
//float MyMathUtility::GetDegree(float r) {
//	r = (r * 180.0f) / MyMathUtility::PI;
//
//	return r;
//}
////ラジアンを求める
//float MyMathUtility::GetRadian(float d) {
//	d = d * MyMathUtility::PI / 180.0f;
//
//	return d;
//}
//// 単位行列を求める
//Matrix4 MyMathUtility::MySetMatrix4Identity() {
//	Matrix4 m = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//				 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
//	return m;
//}
//
////逆行列を求める
//
//  // 拡大縮小行列の作成
//Matrix4 MyMathUtility::MyGenMatrix4Scaling(Vector3 scale) {
//
//	//スケーリング行列を宣言
//	Matrix4 matScale = MyMathUtility::MySetMatrix4Identity();
//
//	matScale.m[0][0] = scale.x;
//	matScale.m[1][1] = scale.y;
//	matScale.m[2][2] = scale.z;
//
//	return matScale;
//}
//// 回転行列の作成
//Matrix4 MyMathUtility::MyGenMatrix4RotationX(float angle) {
//
//	//各軸用回転行列を宣言
//	Matrix4 matRotX = MyMathUtility::MySetMatrix4Identity();
//
//	matRotX.m[1][1] = cos(angle);
//	matRotX.m[1][2] = sin(angle);
//	matRotX.m[2][1] = -sin(angle);
//	matRotX.m[2][2] = cos(angle);
//
//	return matRotX;
//}
//
//Matrix4 MyMathUtility::MyGenMatrix4RotationY(float angle) {
//	//各軸用回転行列を宣言
//	Matrix4 matRotY = MyMathUtility::MySetMatrix4Identity();
//
//	matRotY.m[0][0] = cos(angle);
//	matRotY.m[0][2] = -sin(angle);
//	matRotY.m[2][0] = sin(angle);
//	matRotY.m[2][2] = cos(angle);
//
//	return matRotY;
//}
//
//Matrix4 MyMathUtility::MyGenMatrix4RotationZ(float angle) {
//	//各軸用回転行列を宣言
//	Matrix4 matRotZ = MyMathUtility::MySetMatrix4Identity();
//
//	matRotZ.m[0][0] = cos(angle);
//	matRotZ.m[0][1] = sin(angle);
//	matRotZ.m[1][0] = -sin(angle);
//	matRotZ.m[1][1] = cos(angle);
//
//	return matRotZ;
//}
//
//Matrix4 MyMathUtility::MySynMatrix4Rotation(Vector3 rotation) {
//
//	//合成用回転行列を宣言
//	Matrix4 matRot = MyMathUtility::MySetMatrix4Identity();
//	//各軸用回転行列を宣言
//	Matrix4 matRotX = MyMathUtility::MyGenMatrix4RotationX(rotation.x);
//	//各軸用回転行列を宣言
//	Matrix4 matRotY = MyMathUtility::MyGenMatrix4RotationY(rotation.y);
//	//各軸用回転行列を宣言
//	Matrix4 matRotZ = MyMathUtility::MyGenMatrix4RotationZ(rotation.z);
//
//	//各軸の回転行列を合成
//	matRot *= matRotZ *= matRotX *= matRotY; //順番が大事
//
//	return matRot;
//}
//
//Matrix4 MyMathUtility::MyGenMatrix4Translation(Vector3 translation) {
//	Matrix4 matTrans = MyMathUtility::MySetMatrix4Identity();
//
//	matTrans.m[3][0] = translation.x;
//	matTrans.m[3][1] = translation.y;
//	matTrans.m[3][2] = translation.z;
//
//	return matTrans;
//}
//
//Matrix4 MyMathUtility::MySynMatrix4WorldTransform(WorldTransform worldTransform) {
//	Matrix4 matWorld = MyMathUtility::MySetMatrix4Identity();
//
//	Matrix4 matScale = MyMathUtility::MyGenMatrix4Scaling(worldTransform.scale_);
//
//	Matrix4 matRot = MyMathUtility::MySynMatrix4Rotation(worldTransform.rotation_);
//
//	Matrix4 matTrans = MyMathUtility::MyGenMatrix4Translation(worldTransform.translation_);
//
//	//合成
//	matWorld *= matScale *= matRot *= matTrans;
//
//	return matWorld;
//}
//
////視点移動ベクトルを変更
//Vector3 MyMathUtility::MyChanVector3ViewEye(Input* key) {
//	//移動ベクトル
//	Vector3 eye = MyMathUtility::MySetVector3Zero();
//
//	//視点移動の速さ
//	const float kEyeSpeed = 0.2f;
//
//	//押した方向で移動ベクトルを変更
//	if (key->PushKey(DIK_W)) {
//		eye.z += kEyeSpeed;
//	}
//	else if (key->PushKey(DIK_S)) {
//		eye.z -= kEyeSpeed;
//	}
//
//	return eye;
//}
//
////注視点移動ベクトルを変更
//Vector3 MyMathUtility::MyChanVector3ViewTarget(Input* key) {
//	//移動ベクトル
//	Vector3 target = MyMathUtility::MySetVector3Zero();
//
//	//注視点移動の速さ
//	const float kTargetSpeed = 0.2f;
//
//	//注視点移動処理
//	// //押した方向で移動ベクトルを変更
//	if (key->PushKey(DIK_D)) {
//		target.x += kTargetSpeed;
//		target.x += kTargetSpeed;
//	}
//	else if (key->PushKey(DIK_A)) {
//		target.x -= kTargetSpeed;
//	}
//
//	return target;
//}
//
////上方向ベクトルを変更
//Vector3 MyMathUtility::MyChanVector3ViewUp(Input* key, float& upAngle) {
//	//移動ベクトル
//	Vector3 up = { cosf(upAngle), sinf(upAngle), 0.0f };
//
//	//上方向の回転速さ[ラジアン/frame]
//	const float kUpRotSpeed = 0.05f;
//
//	//上方向回転処理
//	if (key->PushKey(DIK_SPACE)) {
//		upAngle += kUpRotSpeed;
//		// 2πを超えたら元に戻す
//		upAngle = fmodf(upAngle, MyMathUtility::PI * 2.0f);
//	}
//
//	return up;
//}
//// FoV変更
//float MyMathUtility::MyChanProjectionFovAngleY(Input* key, float fovY) {
//	//視野角変更の速さ
//	float fovYspeed = 0.03f;
//	//上キーで視野角が広がる
//	if (key->PushKey(DIK_UP)) {
//		fovY += fovYspeed;
//		if (fovY > MyMathUtility::PI) {
//			fovY = MyMathUtility::PI;
//		}
//	}
//	//下キーで視野角が狭まる
//	else if (key->PushKey(DIK_DOWN)) {
//		fovY -= fovYspeed;
//		if (fovY < 0.01f) {
//			fovY = 0.01f;
//		}
//	}
//
//	return fovY;
//}
//
//// ニアクリップ距離の変更
//float MyMathUtility::MyChanProfectionNearClipZ(Input* key, float nearZ) {
//	//ニアクリップ距離の速さ
//	float nearZspeed = 0.2f;
//
//	// WSキーでニアクリップ距離を増減
//	if (key->PushKey(DIK_W)) {
//		nearZ += nearZspeed;
//	}
//	else if (key->PushKey(DIK_S)) {
//		nearZ -= nearZspeed;
//	}
//
//	return nearZ;
//}
//
//// ベクトル変換
//Vector3 MyMathUtility::MyVector3TransformNormal(Vector3 vector, Matrix4 matrix) {
//
//	Matrix4 matVec = MyMathUtility::MySetMatrix4Identity();
//	matVec.m[0][0] = vector.x;
//	matVec.m[0][1] = vector.y;
//	matVec.m[0][2] = vector.z;
//	matVec.m[0][3] = 0.0f;
//
//	matVec *= matrix;
//
//	vector.x = matVec.m[0][0];
//	vector.y = matVec.m[0][1];
//	vector.z = matVec.m[0][2];
//
//	return vector;
//}

float MyMath::RandomMTFloat(float min, float max)
{
	std::random_device seed;
	std::mt19937_64 engine(seed());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(engine);
}

int MyMath::RandomMTInt(int min, int max)
{
	std::random_device seed;
	std::mt19937_64 engine(seed());
	std::uniform_real_distribution<> dist(min, max);

	return (int)dist(engine);
}
