#pragma once
#include <DirectXMath.h>
#include "XYZ.h"
/*

*	CollisionPrimitive.h

*	�����蔻��v���~�e�B�u�`��

*/
//AABB(��)
struct AABB
{
	const DirectX::XMVECTOR presetCenter = { 0.0f,0.0f,0.0f,1.0f };//�v���Z�b�g
	DirectX::XMVECTOR center = presetCenter;
	float radius[XYZ_Num];
};

//��
struct Sphere
{
	//���S���W
	const DirectX::XMVECTOR presetCenter = { 0.0f,0.0f,0.0f,1.0f };//�v���Z�b�g
	DirectX::XMVECTOR center = presetCenter;
	//���a
	const float presetRadius = 1.0f;//�v���Z�b�g
	float radius = presetRadius;
};
//����
struct Plane
{
	//�@���x�N�g��
	const DirectX::XMVECTOR presetNormal = { 0.0f,1.0f,0.0f,0.0f };//�v���Z�b�g
	DirectX::XMVECTOR normal = presetNormal;
	//���_����̍��W
	const float presetDistance = 0.0f;//�v���Z�b�g
	float distance = presetDistance;
};
//�O�p�`
struct Triangle
{
	//���_���W3��
	DirectX::XMVECTOR	p0;
	DirectX::XMVECTOR	p1;
	DirectX::XMVECTOR	p2;

	//�@���x�N�g��
	DirectX::XMVECTOR	normal;

	//�@���̌v�Z
	void ComputeNormal();
};
//���C(������)
struct Ray
{
	//�n�_���W
	const DirectX::XMVECTOR	presetStart = { 0.0f,0.0f,0.0f,1.0f };//�v���Z�b�g
	DirectX::XMVECTOR	start = presetStart;
	//����
	const DirectX::XMVECTOR	presetDir = { 1.0f,0.0f,0.0f,0.0f };//�v���Z�b�g
	DirectX::XMVECTOR	dir = presetDir;
};