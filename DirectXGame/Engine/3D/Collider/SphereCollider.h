#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>
namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	SphereCollider.h

	*	���Փ˔���I�u�W�F�N�g

	*/
	class SphereCollider :public BaseCollider, public Sphere
	{
	private://�G�C���A�X
		//DirectX::���ȗ�
		using XMVECTOR = DirectX::XMVECTOR;

	public://�����o�֐�(�I�t�Z�b�g�A���a)
		SphereCollider(const XMVECTOR& offset, const float radius) :
			offset_(offset), radius_(radius)
		{
			//���`����Z�b�g
			shapeType_ = COLLISIONSHAPE_SPHERE;
		}
		//�X�V
		void Update() override;

		//�I�t�Z�b�g�Q�b�g
		inline const XMVECTOR& GetOffset() { return offset_; }
		//�I�t�Z�b�g�Z�b�g
		inline void SetOffset(const XMVECTOR& offset) { this->offset_ = offset; }
		//���a�Q�b�g
		inline float GetRadius() { return radius_; }
		//���a�Z�b�g
		inline void SetRadius(const float rad) { this->radius_ = rad; }

	private://�����o�ϐ�

		//�I�u�W�F�N�g���S����̃I�t�Z�b�g
		const XMVECTOR defaultOffset_ = { 0.0f,0.0f,0.0f,0.0f };
		XMVECTOR offset_ = defaultOffset_;
		//���a
		const float defaultRadius_ = 1.0f;
		float radius_ = defaultRadius_;
	};
}
