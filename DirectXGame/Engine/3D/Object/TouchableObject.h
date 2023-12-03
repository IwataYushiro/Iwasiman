#pragma once
#include "Object3d.h"
#include "CollisionAttribute.h"
#include <memory>
/*

*	TouchableObject.h

*	�ڐG�\�I�u�W�F�N�g

*/
class TouchableObject:public Object3d
{
public:
	//�R���X�g���N�^
	TouchableObject() = default;
	//�f�X�g���N�^
	~TouchableObject() = default;

	//�I�u�W�F�N�g�̐���(�g�p���f���A�ڐG�\��)
	static std::unique_ptr<TouchableObject> Create(Model* model = nullptr,
		const bool Touth = true, const unsigned short subAttribute = SUBCOLLISION_ATTR_NONE);
	//������(�g�p���f���A�ڐG�\��)
	bool Initialize(Model* model, const bool Touth, const unsigned short subAttribute);

};
