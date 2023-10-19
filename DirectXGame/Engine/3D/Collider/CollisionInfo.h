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
		this->object_ = object;
		this->collider_ = collider;
		this->inter_ = inter;
	}

	//衝突相手のオブジェクト
	Object3d* object_ = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider_ = nullptr;
	//衝突点
	DirectX::XMVECTOR inter_;
};