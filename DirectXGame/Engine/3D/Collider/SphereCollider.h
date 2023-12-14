#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>
namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	SphereCollider.h

	*	球衝突判定オブジェクト

	*/
	class SphereCollider :public BaseCollider, public Sphere
	{
	private://エイリアス
		//DirectX::を省略
		using XMVECTOR = DirectX::XMVECTOR;

	public://メンバ関数(オフセット、半径)
		SphereCollider(const XMVECTOR& offset, const float radius) :
			offset_(offset), radius_(radius)
		{
			//球形状をセット
			shapeType_ = COLLISIONSHAPE_SPHERE;
		}
		//更新
		void Update() override;

		//オフセットゲット
		inline const XMVECTOR& GetOffset() { return offset_; }
		//オフセットセット
		inline void SetOffset(const XMVECTOR& offset) { this->offset_ = offset; }
		//半径ゲット
		inline float GetRadius() { return radius_; }
		//半径セット
		inline void SetRadius(const float rad) { this->radius_ = rad; }

	private://メンバ変数

		//オブジェクト中心からのオフセット
		const XMVECTOR defaultOffset_ = { 0.0f,0.0f,0.0f,0.0f };
		XMVECTOR offset_ = defaultOffset_;
		//半径
		const float defaultRadius_ = 1.0f;
		float radius_ = defaultRadius_;
	};
}
