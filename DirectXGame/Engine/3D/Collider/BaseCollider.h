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
	inline void SetObject(Object3d* object) { this->object3d_ = object; }
	//オブジェクトゲット
	inline Object3d* GetObject3d() { return object3d_; }

	//更新
	virtual void Update() = 0;

	//形状タイプ取得
	CollisionShapeType GetShapeType() { return shapeType_; }

	//衝突時コールバック関数(コリジョン情報、メイン属性、サブ属性)
	inline void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute) {
		object3d_->OnCollision(info, attribute, subAttribute);
	}

	//メイン属性セット
	inline void SetAttribute(unsigned short attribute) { this->attribute_ = attribute; }
	//メイン属性ゲット
	inline unsigned short GetAttribute() { return attribute_; }
	//メイン属性追加
	inline void AddAttribute(unsigned short attribute) { this->attribute_ |= attribute; }
	//メイン属性削除
	inline void RemoveAttribute(unsigned short attribute) { this->attribute_ &= !attribute; }
	//サブ属性セット
	inline void SetSubAttribute(unsigned short subAttribute) { this->subAttribute_ = subAttribute; }
	//サブ属性ゲット
	inline unsigned short GetSubAttribute() { return subAttribute_; }
	//サブ属性追加
	inline void AddSubAttribute(unsigned short subAttribute) { this->subAttribute_ |= subAttribute; }
	//サブ属性削除
	inline void RemoveSubAttribute(unsigned short subAttribute) { this->subAttribute_ &= !subAttribute; }
protected:
	//3Dオブジェクト(.obj)
	Object3d* object3d_ = nullptr;
	//形状タイプ
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	//当たり判定属性
	unsigned short attribute_ = 0b1111111111111111;
	//当たり判定サブ属性
	unsigned short subAttribute_ = 0b1111111111111111;
};
