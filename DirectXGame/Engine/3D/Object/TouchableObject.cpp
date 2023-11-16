#include "TouchableObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

/*

*	TouchableObject.cpp

*	接触可能オブジェクト

*/

std::unique_ptr<TouchableObject> TouchableObject::Create(Model* model, bool Touth)
{
	std::unique_ptr<TouchableObject> ins = std::make_unique<TouchableObject>();
	if (ins == nullptr)return nullptr;
	//初期化
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
