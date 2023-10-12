#include "TouchableObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

//接触可能オブジェクト

TouchableObject* TouchableObject::Create(Model* model, bool Touth)
{
	TouchableObject* ins = new TouchableObject();
	if (ins == nullptr)return nullptr;
	//初期化
	if (!ins->Initialize(model,Touth))
	{
		delete ins;
		assert(0);
	}
	return ins;
}

bool TouchableObject::Initialize(Model* model, bool Touth)
{
	if (!Object3d::Initialize())return false;
	SetModel(model);

	//コライダーの追加
	if (Touth)
	{
		MeshCollider* collider = new MeshCollider;
		SetCollider(collider);
		collider->ConstructTriangles(model);
		collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	}
	return true;
}
