#pragma once
#include "Object3d.h"
//接触可能オブジェクト
class TouchableObject:public Object3d
{
public:
	TouchableObject() = default;
	~TouchableObject() = default;

	//オブジェクトの生成
	static TouchableObject* Create(Model* model = nullptr, bool Touth = true);
	//初期化
	bool Initialize(Model* model, bool Touth);

};
