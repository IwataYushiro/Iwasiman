#include "TouchableObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

TouchableObject* TouchableObject::Create(Model* model)
{
	TouchableObject* ins = new TouchableObject();
	if (ins == nullptr)return nullptr;
	//初期化
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

	//コライダーの追加
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	collider->ConstructTriangles(model);
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	return true;
}
