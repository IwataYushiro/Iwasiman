#pragma once
#include "CollisionInfo.h" 
#include "CollisionTypes.h"
#include "Object3d.h"

//コライダー基盤クラス
class BaseCollider
{
public:
	BaseCollider() = default;
	//仮想デストラクタ
	virtual ~BaseCollider() = default;

	inline void SetObject(Object3d* object) { this->object3d = object; }

	inline Object3d* GetObject3d() { return object3d; }

	//更新
	virtual void Update() = 0;

	//形状タイプ取得
	CollisionShapeType GetShapeType() { return shapeType; }

	//衝突時コールバック関数
	inline void OnCollision(const CollisionInfo& info) { object3d->OnCollision(info); }

protected:
	Object3d* object3d = nullptr;
	//形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
};
