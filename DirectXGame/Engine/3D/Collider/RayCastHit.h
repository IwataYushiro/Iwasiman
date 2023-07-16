#pragma once
#include "BaseCollider.h"
#include <DirectXMath.h>

class Object3d;

//���C�L���X�g�ɂ����𓾂邽�߂̍\����
struct RaycastHit
{
	//�Փˑ���̃I�u�W�F�N�g
	Object3d* object = nullptr;
	//�Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	//�Փ˓_
	DirectX::XMVECTOR inter;
	//�Փ˓_�܂ł̋���
	float distance = 0.0f;
};