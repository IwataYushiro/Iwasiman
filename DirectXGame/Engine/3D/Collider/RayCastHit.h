#pragma once
#include "BaseCollider.h"
#include <DirectXMath.h>

namespace IwasiEngine//IwasiEngineのネームスペース
{
	class Object3d;

	/*

	*	RayCastHit.h (cpp X)

	*	レイキャストによる情報を得るための構造体

	*/
	struct RaycastHit
	{
		//衝突相手のオブジェクト
		const Object3d* object = nullptr;
		//衝突相手のコライダー
		BaseCollider* collider = nullptr;
		//衝突点
		DirectX::XMVECTOR inter;
		//衝突点までの距離
		float distance = 0.0f;
	};
}