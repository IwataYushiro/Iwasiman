#pragma once
#include "BaseCollider.h"
#include <DirectXMath.h>

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	class Object3d;

	/*

	*	RayCastHit.h (cpp X)

	*	���C�L���X�g�ɂ����𓾂邽�߂̍\����

	*/
	struct RaycastHit
	{
		//�Փˑ���̃I�u�W�F�N�g
		const Object3d* object = nullptr;
		//�Փˑ���̃R���C�_�[
		BaseCollider* collider = nullptr;
		//�Փ˓_
		DirectX::XMVECTOR inter;
		//�Փ˓_�܂ł̋���
		float distance = 0.0f;
	};
}