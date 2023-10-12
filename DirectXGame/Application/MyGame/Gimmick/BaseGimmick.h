#pragma once
#include "Object3d.h"

//自機クラスの前方宣言
class Player;
class CollisionManager;

/*

*	BaseGimmick.h (cpp X)

*	ギミック基盤クラス

*/
class BaseGimmick :public Object3d
{
public:
	virtual ~BaseGimmick() = default;

	virtual bool Initialize() = 0; //{ return Object3d::Initialize(); }

	//更新
	virtual void Update() = 0; //{ Object3d::Update(); }
	//描画
	virtual void Draw() = 0; //{ Object3d::Draw(); }

};

