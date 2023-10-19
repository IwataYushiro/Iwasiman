#pragma once
#include "Object3d.h"
#include "EnemyBullet.h"
#include <DirectXMath.h>

//前方宣言
//自機クラス
class Player;
//コリジョンマネージャー
class CollisionManager;
//ゲームプレイシーン
class GamePlayScene;

/*

*	BaseEnemy.h (cpp X)

*	敵基盤クラス

*/
class BaseEnemy :public Object3d
{
private:
	using XMFLOAT3 = DirectX::XMFLOAT3;
public:
	//デストラクタ
	virtual ~BaseEnemy() = default;

	//初期化
	virtual bool Initialize() { return Object3d::Initialize(); }
	//初期化(サブ属性指定版)
	virtual bool Initialize([[maybe_unused]] unsigned short subAttribute ) { return Object3d::Initialize(); }

	//更新
	virtual void Update(bool isStart = false) = 0;
	//描画
	virtual void Draw() { Object3d::Draw(); }

protected:
	//コリジョンマネージャー
	static CollisionManager* colManager_;

	//自機
	Player* player_ = nullptr;

	//ゲームプレイシーン
	GamePlayScene* gameScene_ = nullptr;

	//死亡フラグ
	bool isDead_;
	//ライフ
	int life_;
	//速度
	XMFLOAT3 speed_;

	//ボスの死亡フラグ
	bool bossDead_;

public://アクセッサ
	//死んだかどうか
	virtual bool IsDead() const { return isDead_; }
	//ボスが死んだかどうか
	virtual bool BossDead()const { return bossDead_; }
};

