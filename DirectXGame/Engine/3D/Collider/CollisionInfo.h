#pragma once
#include <DirectXMath.h>

class Object3d;
class BaseCollider;

/*

*	CollisionInfo.h (cpp X)

*	�Փˏ��

*/
struct CollisionInfo
{
	//�R���X�g���N�^(�I�u�W�F�N�g�A�R���C�_�[�A�Փ˓_)
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