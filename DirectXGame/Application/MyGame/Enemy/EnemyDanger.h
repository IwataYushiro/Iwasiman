#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "Model.h"
#include "ParticleManager.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	EnemyDanger.h

*	危険な敵

*/
class EnemyDanger :public BaseEnemy {
public:
	//デストラクタ
	~EnemyDanger();
	//生成(使用モデル、使用弾モデル、プレイヤー、ゲームプレイシーン、どの方向に進むか)
	static std::unique_ptr<EnemyDanger> Create(const Model* model = nullptr,const Player* player = nullptr,
		GamePlayScene* gamescene = nullptr, unsigned short xyz = XYZ_X);

	//初期化
	bool Initialize()override;

	//リセット処理
	void Reset();

	//パラメータ
	void Parameter();
	//更新
	void Update(const bool isStart = false)override;
	//描画
	void Draw()override;
	//パーティクル描画
	void DrawParticle()override;

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, const unsigned short attribute,
		const unsigned short subAttribute)override;

private:

	//半径
	const float radius_ = 10.0f;
	//ボス死亡
	bool bossDead_ = false;

	//死亡フラグ
	bool isDead_;
	//ライフ
	int life_;

	//反転フラグ
	bool isReverse_ = false;
	
	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

public:
	//死んだかどうか
	bool IsDead() const override { return isDead_; }
	//プレイヤーセット
	void SetPlayer(const Player* player) { player_ = player; }
	//ゲームシーンセット
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
	//見た目をパーティクルで覆う処理
	void UpdateParticleSkin();
};