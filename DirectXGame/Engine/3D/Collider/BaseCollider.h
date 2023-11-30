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
	//コリジョンマネージャー
	friend class CollisionManager;

public:
	BaseCollider() = default;
	//仮想デストラクタ
	virtual ~BaseCollider() = default;

	//オブジェクトセット
	inline void SetObject(const Object3d* object) { this->object3d_ = const_cast<Object3d*>(object); }
	//オブジェクトゲット
	inline const Object3d* GetObject3d() const{ return object3d_; }

	//更新
	virtual void Update() = 0;

	//形状タイプ取得
	const CollisionShapeType GetShapeType() const { return shapeType_; }

	//衝突時コールバック関数(コリジョン情報、メイン属性、サブ属性)
	inline void OnCollision(const CollisionInfo& info, const unsigned short attribute,
		const unsigned short subAttribute) {
		object3d_->OnCollision(info, attribute, subAttribute);
	}

	//メイン属性セット
	inline void SetAttribute(const unsigned short attribute) { this->attribute_ = attribute; }
	//メイン属性ゲット
	inline unsigned short GetAttribute() { return attribute_; }
	//メイン属性追加
	inline void AddAttribute(const unsigned short attribute) { this->attribute_ |= attribute; }
	//メイン属性削除
	inline void RemoveAttribute(const unsigned short attribute) { this->attribute_ &= !attribute; }
	//サブ属性セット
	inline void SetSubAttribute(const unsigned short subAttribute) { this->subAttribute_ = subAttribute; }
	//サブ属性ゲット
	inline unsigned short GetSubAttribute() { return subAttribute_; }
	//サブ属性追加
	inline void AddSubAttribute(const unsigned short subAttribute) { this->subAttribute_ |= subAttribute; }
	//サブ属性削除
	inline void RemoveSubAttribute(const unsigned short subAttribute) { this->subAttribute_ &= !subAttribute; }
protected:
	//3Dオブジェクト(.obj)
	Object3d* object3d_ = nullptr;
	//形状タイプ
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	//当たり判定属性
	const unsigned short defaultAttribute_= 0b1111111111111111;
	unsigned short attribute_ = defaultAttribute_;
	//当たり判定サブ属性
	const unsigned short defaultSubAttribute_ = 0b1111111111111111;
	unsigned short subAttribute_ = defaultSubAttribute_;
};
