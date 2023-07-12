#pragma once
#include <DirectXMath.h>

//��
struct Sphere
{
	//���S���W
	DirectX::XMVECTOR center = { 0.0f,0.0f,0.0f,1.0f };
	//���a
	float radius = 1.0f;
};
//����
struct Plane
{
	//�@���x�N�g��
	DirectX::XMVECTOR normal = { 0.0f,1.0f,0.0f,0.0f };
	//���_����̍��W
	float distance = 0.0f;
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
	DirectX::XMVECTOR	start = { 0.0f,0.0f,0.0f,1.0f };
	//����
	DirectX::XMVECTOR	dir = { 1.0f,0.0f,0.0f,0.0f };
};