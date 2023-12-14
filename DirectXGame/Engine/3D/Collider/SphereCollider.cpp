#include "SphereCollider.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	SphereCollider.cpp

*	���Փ˔���I�u�W�F�N�g

*/

void SphereCollider::Update()
{
	//���[���h�s�񂩂���W�𒊏o
	const XMMATRIX& matWorld = object3d_->GetWorld();

	//���̃����o�ϐ����X�V
	Sphere::center = matWorld.r[XYZW_W] + offset_;
	Sphere::radius = radius_;
}
