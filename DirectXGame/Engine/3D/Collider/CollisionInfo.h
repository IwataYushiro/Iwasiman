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
		this->object_ = object;
		this->collider_ = collider;
		this->inter_ = inter;
	}

	//�Փˑ���̃I�u�W�F�N�g
	Object3d* object_ = nullptr;
	//�Փˑ���̃R���C�_�[
	BaseCollider* collider_ = nullptr;
	//�Փ˓_
	DirectX::XMVECTOR inter_;
};