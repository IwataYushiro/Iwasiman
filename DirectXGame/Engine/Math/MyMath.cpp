//#include "MyMath.h"
//
//using namespace DirectX;
//
////��x�N�g����Ԃ�
//const XMFLOAT3 MyMathUtility::MySetVector3Zero() {
//	XMFLOAT3 v = { 0.0f, 0.0f, 0.0f };
//
//	return v;
//}
//
//// �m����(����)�����߂�
//float MyMathUtility::MyVector3Length(const XMFLOAT3& v)
//{
//	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
//}
////���K������
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
////�x�����߂�
//float MyMathUtility::GetDegree(float r) {
//	r = (r * 180.0f) / MyMathUtility::PI;
//
//	return r;
//}
////���W�A�������߂�
//float MyMathUtility::GetRadian(float d) {
//	d = d * MyMathUtility::PI / 180.0f;
//
//	return d;
//}
//// �P�ʍs������߂�
//Matrix4 MyMathUtility::MySetMatrix4Identity() {
//	Matrix4 m = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//				 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
//	return m;
//}
//
////�t�s������߂�
//
//  // �g��k���s��̍쐬
//Matrix4 MyMathUtility::MyGenMatrix4Scaling(Vector3 scale) {
//
//	//�X�P�[�����O�s���錾
//	Matrix4 matScale = MyMathUtility::MySetMatrix4Identity();
//
//	matScale.m[0][0] = scale.x;
//	matScale.m[1][1] = scale.y;
//	matScale.m[2][2] = scale.z;
//
//	return matScale;
//}
//// ��]�s��̍쐬
//Matrix4 MyMathUtility::MyGenMatrix4RotationX(float angle) {
//
//	//�e���p��]�s���錾
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
//	//�e���p��]�s���錾
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
//	//�e���p��]�s���錾
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
//	//�����p��]�s���錾
//	Matrix4 matRot = MyMathUtility::MySetMatrix4Identity();
//	//�e���p��]�s���錾
//	Matrix4 matRotX = MyMathUtility::MyGenMatrix4RotationX(rotation.x);
//	//�e���p��]�s���錾
//	Matrix4 matRotY = MyMathUtility::MyGenMatrix4RotationY(rotation.y);
//	//�e���p��]�s���錾
//	Matrix4 matRotZ = MyMathUtility::MyGenMatrix4RotationZ(rotation.z);
//
//	//�e���̉�]�s�������
//	matRot *= matRotZ *= matRotX *= matRotY; //���Ԃ��厖
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
//	//����
//	matWorld *= matScale *= matRot *= matTrans;
//
//	return matWorld;
//}
//
////���_�ړ��x�N�g����ύX
//Vector3 MyMathUtility::MyChanVector3ViewEye(Input* key) {
//	//�ړ��x�N�g��
//	Vector3 eye = MyMathUtility::MySetVector3Zero();
//
//	//���_�ړ��̑���
//	const float kEyeSpeed = 0.2f;
//
//	//�����������ňړ��x�N�g����ύX
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
////�����_�ړ��x�N�g����ύX
//Vector3 MyMathUtility::MyChanVector3ViewTarget(Input* key) {
//	//�ړ��x�N�g��
//	Vector3 target = MyMathUtility::MySetVector3Zero();
//
//	//�����_�ړ��̑���
//	const float kTargetSpeed = 0.2f;
//
//	//�����_�ړ�����
//	// //�����������ňړ��x�N�g����ύX
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
////������x�N�g����ύX
//Vector3 MyMathUtility::MyChanVector3ViewUp(Input* key, float& upAngle) {
//	//�ړ��x�N�g��
//	Vector3 up = { cosf(upAngle), sinf(upAngle), 0.0f };
//
//	//������̉�]����[���W�A��/frame]
//	const float kUpRotSpeed = 0.05f;
//
//	//�������]����
//	if (key->PushKey(DIK_SPACE)) {
//		upAngle += kUpRotSpeed;
//		// 2�΂𒴂����猳�ɖ߂�
//		upAngle = fmodf(upAngle, MyMathUtility::PI * 2.0f);
//	}
//
//	return up;
//}
//// FoV�ύX
//float MyMathUtility::MyChanProjectionFovAngleY(Input* key, float fovY) {
//	//����p�ύX�̑���
//	float fovYspeed = 0.03f;
//	//��L�[�Ŏ���p���L����
//	if (key->PushKey(DIK_UP)) {
//		fovY += fovYspeed;
//		if (fovY > MyMathUtility::PI) {
//			fovY = MyMathUtility::PI;
//		}
//	}
//	//���L�[�Ŏ���p�����܂�
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
//// �j�A�N���b�v�����̕ύX
//float MyMathUtility::MyChanProfectionNearClipZ(Input* key, float nearZ) {
//	//�j�A�N���b�v�����̑���
//	float nearZspeed = 0.2f;
//
//	// WS�L�[�Ńj�A�N���b�v�����𑝌�
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
//// �x�N�g���ϊ�
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