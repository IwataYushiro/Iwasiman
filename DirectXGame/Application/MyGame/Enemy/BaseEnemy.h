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

	virtual bool Initialize() { return Object3d::Initialize(); }
	virtual bool Initialize(unsigned short subAttribute) { return Object3d::Initialize(); }

	//更新
	virtual void Update() { Object3d::Update(); }
	//描画
	virtual void Draw() { Object3d::Draw(); }

protected:
	static CollisionManager* colManager_;

	//自機
	Player* player_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

	//死亡フラグとライフ
	bool isDead_;
	int life_;

	bool bossDead;
public://アクセッサ
	bool IsDead() const { return isDead_; }
	bool BossDead()const { return bossDead; }
};

