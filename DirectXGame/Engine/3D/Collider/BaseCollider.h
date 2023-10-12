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
	friend class CollisionManager;

public:
	BaseCollider() = default;
	//���z�f�X�g���N�^
	virtual ~BaseCollider() = default;

	inline void SetObject(Object3d* object) { this->object3d = object; }

	inline Object3d* GetObject3d() { return object3d; }

	//�X�V
	virtual void Update() = 0;

	//�`��^�C�v�擾
	CollisionShapeType GetShapeType() { return shapeType; }

	//�Փˎ��R�[���o�b�N�֐�
	inline void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute) {
		object3d->OnCollision(info, attribute, subAttribute);
	}

	inline void SetAttribute(unsigned short attribute) { this->attribute_ = attribute; }

	inline unsigned short GetAttribute() { return attribute_; }

	inline void AddAttribute(unsigned short attribute) { this->attribute_ |= attribute; }

	inline void RemoveAttribute(unsigned short attribute) { this->attribute_ &= !attribute; }

	inline void SetSubAttribute(unsigned short subAttribute) { this->subAttribute_ = subAttribute; }

	inline unsigned short GetSubAttribute() { return subAttribute_; }

	inline void AddSubAttribute(unsigned short subAttribute) { this->subAttribute_ |= subAttribute; }

	inline void RemoveSubAttribute(unsigned short subAttribute) { this->subAttribute_ &= !subAttribute; }
protected:
	Object3d* object3d = nullptr;
	//�`��^�C�v
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	//�����蔻�葮��
	unsigned short attribute_ = 0b1111111111111111;
	//�����蔻��T�u����
	unsigned short subAttribute_ = 0b1111111111111111;
};
