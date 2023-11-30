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
	inline void SetObject(const Object3d* object) { this->object3d_ = const_cast<Object3d*>(object); }
	//�I�u�W�F�N�g�Q�b�g
	inline const Object3d* GetObject3d() const{ return object3d_; }

	//�X�V
	virtual void Update() = 0;

	//�`��^�C�v�擾
	const CollisionShapeType GetShapeType() const { return shapeType_; }

	//�Փˎ��R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	inline void OnCollision(const CollisionInfo& info, const unsigned short attribute,
		const unsigned short subAttribute) {
		object3d_->OnCollision(info, attribute, subAttribute);
	}

	//���C�������Z�b�g
	inline void SetAttribute(const unsigned short attribute) { this->attribute_ = attribute; }
	//���C�������Q�b�g
	inline unsigned short GetAttribute() { return attribute_; }
	//���C�������ǉ�
	inline void AddAttribute(const unsigned short attribute) { this->attribute_ |= attribute; }
	//���C�������폜
	inline void RemoveAttribute(const unsigned short attribute) { this->attribute_ &= !attribute; }
	//�T�u�����Z�b�g
	inline void SetSubAttribute(const unsigned short subAttribute) { this->subAttribute_ = subAttribute; }
	//�T�u�����Q�b�g
	inline unsigned short GetSubAttribute() { return subAttribute_; }
	//�T�u�����ǉ�
	inline void AddSubAttribute(const unsigned short subAttribute) { this->subAttribute_ |= subAttribute; }
	//�T�u�����폜
	inline void RemoveSubAttribute(const unsigned short subAttribute) { this->subAttribute_ &= !subAttribute; }
protected:
	//3D�I�u�W�F�N�g(.obj)
	Object3d* object3d_ = nullptr;
	//�`��^�C�v
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	//�����蔻�葮��
	const unsigned short defaultAttribute_= 0b1111111111111111;
	unsigned short attribute_ = defaultAttribute_;
	//�����蔻��T�u����
	const unsigned short defaultSubAttribute_ = 0b1111111111111111;
	unsigned short subAttribute_ = defaultSubAttribute_;
};
