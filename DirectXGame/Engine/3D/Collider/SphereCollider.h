#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/*

*	SphereCollider.h

*	球衝突判定オブジェクト

*/
class SphereCollider :public BaseCollider, public Sphere
{
private://エイリアス
	//DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;

public://メンバ関数
	SphereCollider(XMVECTOR offset = { 0.0f,0.0f,0.0f,0.0f }, float radius = 1.0f) :
		offset_(offset), radius_(radius)
	{
		//球形状をセット
		shapeType = COLLISIONSHAPE_SPHERE;
	}
	//更新
	void Update() override;

	inline const XMVECTOR& GetOffset() { return offset_; }

	inline void SetOffset(const XMVECTOR& offset) { this->offset_ = offset; }

	inline float GetRadius(){return radius_;}

	inline void SetRadius(float rad) { this->radius_ = rad; }

private://メンバ変数
	//オブジェクト中心からのオフセット
	XMVECTOR offset_;
	//半径
	float radius_;
};

