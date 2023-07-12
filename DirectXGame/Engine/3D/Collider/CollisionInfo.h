#pragma once
#include <DirectXMath.h>

class Object3d;
class BaseCollider;

//�Փˏ��
struct CollisionInfo
{
	CollisionInfo(Object3d* object, BaseCollider* collider,
		const DirectX::XMVECTOR& inter) {
		this->object = object;
		this->collider = collider;
		this->inter = inter;
	}

	//�Փˑ���̃I�u�W�F�N�g
	Object3d* object = nullptr;
	//�Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	//�Փ˓_
	DirectX::XMVECTOR inter;
};