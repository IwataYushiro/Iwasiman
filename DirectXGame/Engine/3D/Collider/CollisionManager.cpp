#include "CollisionManager.h"
#include "BaseCollider.h"
#include "Collision.h"

using namespace DirectX;

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;
	//全ての組み合わせについて総当たりチェック
	itA = colliders.begin();
	for (; itA != colliders.end(); ++itA)
	{
		itB = itA;
		++itB;
		for (; itB != colliders.end(); ++itB)
		{
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;
			//ともに球
			if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE && colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
			{
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Sphere* SphereB = dynamic_cast<Sphere*>(colB);
				DirectX::XMVECTOR inter;
				if (Collision::ChackSphere2Sphere(*SphereA, *SphereB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
		}
	}

}

bool CollisionManager::RayCast(const Ray& ray, RaycastHit* hitInfo, float maxDistance)
{
	bool result = false;
	//走査用イテレータ
	std::forward_list<BaseCollider*>::iterator it;
	//今までで最も近いコライダーを記録するためのイテレータ
	std::forward_list<BaseCollider*>::iterator it_hit;
	//今までで最も近いコライダーの距離を記録する変数
	float distance = maxDistance;
	//今までで最も近いコライダーとの交点を記録する変数
	XMVECTOR inter;
	//全コライダーと総当たりチェック
	it = colliders.begin();
	for (; it != colliders.end(); ++it)
	{
		BaseCollider* colA = *it;
		//球の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphere = dynamic_cast<Sphere*>(colA);
			float tempDistance;
			XMVECTOR tempInter;
			//当たらなければ除外
			if (!Collision::ChackRay2Sphere(ray, *sphere, &tempDistance, &tempInter))continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance)continue;
			//今までで最も近いので記録を取る
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
	}
	//最終的に何かに当たっていれば結果を書き込む
	if (result && hitInfo)
	{
		hitInfo->distance = distance;
		hitInfo->inter = inter;
		hitInfo->collider = *it_hit;
		hitInfo->object = hitInfo->collider->GetObject3d();
	}

	return result;
}
