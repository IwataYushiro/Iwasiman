#include "TouchableObject.h"
#include "MeshCollider.h"

TouchableObject* TouchableObject::Create(Model* model)
{
	TouchableObject* ins = new TouchableObject();
	if (ins == nullptr)return nullptr;
	//������
	if (!ins->Initialize(model))
	{
		delete ins;
		assert(0);
	}
	return ins;
}

bool TouchableObject::Initialize(Model* model)
{
	if (!Object3d::Initialize())return false;
	SetModel(model);

	//�R���C�_�[�̒ǉ�
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	collider->ConstructTriangles(model);

	return true;
}
