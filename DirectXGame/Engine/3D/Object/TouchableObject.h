#pragma once
#include "Object3d.h"
#include "CollisionAttribute.h"
#include <memory>
namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	TouchableObject.h

	*	接触可能オブジェクト

	*/
	class TouchableObject :public Object3d
	{
	public:
		//コンストラクタ
		TouchableObject() = default;
		//デストラクタ
		~TouchableObject() = default;

		//オブジェクトの生成(使用モデル、接触可能か)
		static std::unique_ptr<TouchableObject> Create(Model* model = nullptr,
			const bool Touth = true, const unsigned short subAttribute = SUBCOLLISION_ATTR_NONE);
		//初期化(使用モデル、接触可能か)
		bool Initialize(Model* model, const bool Touth, const unsigned short subAttribute);

	};
}
