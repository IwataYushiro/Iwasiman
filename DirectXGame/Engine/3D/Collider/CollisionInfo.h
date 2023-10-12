#pragma once
#include <DirectXMath.h>

class Object3d;
class BaseCollider;

/*

*	CollisionInfo.h (cpp X)

*	衝突情報

*/
struct CollisionInfo
{
	//コンストラクタ(オブジェクト、コライダー、衝突点)
	CollisionInfo(Object3d* object, BaseCollider* collider,
		const DirectX::XMVECTOR& inter) {
		this->object = object;
		this->collider = collider;
		this->inter = inter;
	}

	//衝突相手のオブジェクト
	Object3d* object = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	DirectX::XMVECTOR inter;
};