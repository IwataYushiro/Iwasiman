#pragma once
#include "CollisionInfo.h" 
#include "CollisionTypes.h"
#include "Object3d.h"

/*

*	BaseCollider.h (cpp X)

*	�R���C�_�[���

*/
class BaseCollider
{
public://�t�����h�N���X
	//�R���W�����}�l�[�W���[
	friend class CollisionManager;

public:
	BaseCollider() = default;
	//���z�f�X�g���N�^
	virtual ~BaseCollider() = default;

	//�I�u�W�F�N�g�Z�b�g
	inline void SetObject(Object3d* object) { this->object3d_ = object; }
	//�I�u�W�F�N�g�Q�b�g
	inline Object3d* GetObject3d() { return object3d_; }

	//�X�V
	virtual void Update() = 0;

	//�`��^�C�v�擾
	CollisionShapeType GetShapeType() { return shapeType_; }

	//�Փˎ��R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	inline void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute) {
		object3d_->OnCollision(info, attribute, subAttribute);
	}

	//���C�������Z�b�g
	inline void SetAttribute(unsigned short attribute) { this->attribute_ = attribute; }
	//���C�������Q�b�g
	inline unsigned short GetAttribute() { return attribute_; }
	//���C�������ǉ�
	inline void AddAttribute(unsigned short attribute) { this->attribute_ |= attribute; }
	//���C�������폜
	inline void RemoveAttribute(unsigned short attribute) { this->attribute_ &= !attribute; }
	//�T�u�����Z�b�g
	inline void SetSubAttribute(unsigned short subAttribute) { this->subAttribute_ = subAttribute; }
	//�T�u�����Q�b�g
	inline unsigned short GetSubAttribute() { return subAttribute_; }
	//�T�u�����ǉ�
	inline void AddSubAttribute(unsigned short subAttribute) { this->subAttribute_ |= subAttribute; }
	//�T�u�����폜
	inline void RemoveSubAttribute(unsigned short subAttribute) { this->subAttribute_ &= !subAttribute; }
protected:
	//3D�I�u�W�F�N�g(.obj)
	Object3d* object3d_ = nullptr;
	//�`��^�C�v
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	//�����蔻�葮��
	unsigned short attribute_ = 0b1111111111111111;
	//�����蔻��T�u����
	unsigned short subAttribute_ = 0b1111111111111111;
};
