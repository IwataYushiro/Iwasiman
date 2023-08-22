#pragma once
#include "Object3d.h"
#include "EnemyBullet.h"

//自機クラスの前方宣言
class Player;
class CollisionManager;
class GamePlayScene;

//敵基盤クラス
class BaseEnemy :public Object3d
{
public:
	virtual ~BaseEnemy() = default;

	virtual bool Initialize() = 0;

	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;

protected:
	static CollisionManager* colManager_;

	//自機
	Player* player_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

	//死亡フラグとライフ
	bool isDead_;
	int life_;
public://アクセッサ
	bool IsDead() const { return isDead_; }
};

