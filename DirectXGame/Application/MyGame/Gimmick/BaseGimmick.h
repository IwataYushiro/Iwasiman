#pragma once
#include "Object3d.h"

//前方宣言
//自機クラス
class Player;
//コリジョンマネージャー
class CollisionManager;

/*

*	BaseGimmick.h (cpp X)

*	ギミック基盤クラス

*/
class BaseGimmick :public Object3d
{
public:
	//デストラクタ
	virtual ~BaseGimmick() = default;

	//初期化
	virtual bool Initialize() = 0;
	
	//更新
	virtual void Update() = 0; //{ Object3d::Update(); }
	//描画
	virtual void Draw() = 0; //{ Object3d::Draw(); }

};

