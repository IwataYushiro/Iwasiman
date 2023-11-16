#include "TouchableObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

/*

*	TouchableObject.cpp

*	�ڐG�\�I�u�W�F�N�g

*/

std::unique_ptr<TouchableObject> TouchableObject::Create(Model* model, bool Touth)
{
	std::unique_ptr<TouchableObject> ins = std::make_unique<TouchableObject>();
	if (ins == nullptr)return nullptr;
	//������
	if (!ins->Initialize(model,Touth))
	{
		assert(0);
	}
	return ins;
}

bool TouchableObject::Initialize(Model* model, bool Touth)
{
	if (!Object3d::Initialize())return false;
	SetModel(model);

	//�R���C�_�[�̒ǉ�
	if (Touth)
	{
		MeshCollider* collider = new MeshCollider;
		SetCollider(collider);
		collider->ConstructTriangles(model);
		collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	}
	return true;
}
