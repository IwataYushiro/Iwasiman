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
	CollisionInfo(const Object3d* object, const BaseCollider* collider,
		const DirectX::XMVECTOR& inter) {
		this->object_ = const_cast<Object3d*>(object);
		this->collider_ = const_cast<BaseCollider*>(collider);
		this->inter_ = inter;
	}

	//�Փˑ���̃I�u�W�F�N�g
	Object3d* object_ = nullptr;
	//�Փˑ���̃R���C�_�[
	BaseCollider* collider_ = nullptr;
	//�Փ˓_
	DirectX::XMVECTOR inter_;
};