#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

//���Փ˔���I�u�W�F�N�g
class SphereCollider :public BaseCollider, public Sphere
{
private://�G�C���A�X
	//DirectX::���ȗ�
	using XMVECTOR = DirectX::XMVECTOR;

public://�����o�֐�
	SphereCollider(XMVECTOR offset={0.0f,0.0f,0.0f,0.0f},float radius=1.0f):
		offset_(offset),radius_(radius)
	{
		//���`����Z�b�g
		shapeType = COLLISIONSHAPE_SPHERE;
	}
	//�X�V
	void Update() override;

	inline void SetRadius(float radius) { this->radius_ = radius; }

private://�����o�ϐ�
	//�I�u�W�F�N�g���S����̃I�t�Z�b�g
	XMVECTOR offset_;
	//���a
	float radius_;
};

