#include "TouchableObject.h"
#include "MeshCollider.h"

using namespace IwasiEngine;
/*

*	TouchableObject.cpp

*	�ڐG�\�I�u�W�F�N�g

*/

std::unique_ptr<TouchableObject> TouchableObject::Create(Model* model,const bool Touth, const unsigned short subAttribute)
{
	std::unique_ptr<TouchableObject> ins = std::make_unique<TouchableObject>();
	if (ins == nullptr)return nullptr;
	//������
	if (!ins->Initialize(model, Touth, subAttribute))
	{
		assert(0);
	}
	return ins;
}

bool TouchableObject::Initialize(Model* model,const bool Touth, const unsigned short subAttribute)
{
	if (!Object3d::Initialize())return false;
	SetModel(model);

	//�R���C�_�[�̒ǉ�
	if (Touth)
	{
		collider_ = new MeshCollider;
		SetCollider(collider_);
		dynamic_cast<MeshCollider*>(collider_)->ConstructTriangles(model);
		collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);
		collider_->SetSubAttribute(subAttribute);
	}
	return true;
}
