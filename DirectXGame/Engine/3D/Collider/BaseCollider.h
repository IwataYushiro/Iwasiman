#pragma once
#include "CollisionInfo.h" 
#include "CollisionTypes.h"
#include "Object3d.h"

/*

*	BaseCollider.h (cpp X)

*	コライダー基盤

*/
class BaseCollider
{
public://フレンドクラス
	friend class CollisionManager;

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
	inline void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute) {
		object3d->OnCollision(info, attribute, subAttribute);
	}

	inline void SetAttribute(unsigned short attribute) { this->attribute_ = attribute; }

	inline unsigned short GetAttribute() { return attribute_; }

	inline void AddAttribute(unsigned short attribute) { this->attribute_ |= attribute; }

	inline void RemoveAttribute(unsigned short attribute) { this->attribute_ &= !attribute; }

	inline void SetSubAttribute(unsigned short subAttribute) { this->subAttribute_ = subAttribute; }

	inline unsigned short GetSubAttribute() { return subAttribute_; }

	inline void AddSubAttribute(unsigned short subAttribute) { this->subAttribute_ |= subAttribute; }

	inline void RemoveSubAttribute(unsigned short subAttribute) { this->subAttribute_ &= !subAttribute; }
protected:
	Object3d* object3d = nullptr;
	//形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	//当たり判定属性
	unsigned short attribute_ = 0b1111111111111111;
	//当たり判定サブ属性
	unsigned short subAttribute_ = 0b1111111111111111;
};
