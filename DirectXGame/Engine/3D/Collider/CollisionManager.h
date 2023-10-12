#pragma once
#include "CollisionPrimitive.h"
#include "RayCastHit.h"
#include "QueryCallback.h"

#include <forward_list>
#include <d3d12.h>
class BaseCollider;

/*

*	CollisionManager.h

*	当たり判定マネージャー

*/
class CollisionManager
{
public://静的メンバ関数
	static CollisionManager* GetInstance();

public://メンバ関数
	//コライダーの追加
	inline void AddCollider(BaseCollider* collider) { colliders.push_front(collider); }
	//コライダーの削除
	inline void RemoveCollider(BaseCollider* collider) { colliders.remove(collider); }
	//全ての衝突チェック
	void CheckAllCollisions();
/// <summary>
/// レイキャスト
/// </summary>
///	<param names="ray">レイ</param>
///	<param names="hitInfo">衝突情報</param>
/// <param names="maxDistance">最大距離</param>
/// <returns>レイが任意のコライダーと交わる場合true,それ以外はfalse</returns>
	bool RayCast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
/// レイキャスト(属性指定版)
/// </summary>
///	<param names="ray">レイ</param>
///	<param names="attribute">衝突属性</param>
///	<param names="hitInfo">衝突情報</param>
/// <param names="maxDistance">最大距離</param>
/// <returns>レイが任意のコライダーと交わる場合true,それ以外はfalse</returns>
	bool RayCast(const Ray& ray, unsigned short attribute, RaycastHit* hitInfo = nullptr,
		float maxDistance = D3D12_FLOAT32_MAX);

/// <summary>
/// 球による衝突全要素
/// </summary>
///	<param names="sphere">球</param>
///	<param names="callback">衝突時コールバック</param>
/// <param names="attribute">対象の衝突属性</param>
	void QuerySphere(const Sphere& sphere, QueryCallback* callback,
		unsigned short attribute = 0xffff);

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
	//コライダーのリスト
	std::forward_list<BaseCollider*>colliders;
};

