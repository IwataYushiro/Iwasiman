#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "ParticleManager.h"
#include "Input.h"

#include <chrono>
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	EnemyBoss.h

*	左右に動くボス敵

*/
class EnemyBoss :public BaseEnemy {
public:
	//デストラクタ
	~EnemyBoss();
	//生成(使用モデル、使用弾モデル、プレイヤー、ゲームプレイシーン,ボスステージかどうか)
	static std::unique_ptr<EnemyBoss> Create(const Model* model = nullptr, const Model* bullet = nullptr,
		const Player* player = nullptr, GamePlayScene* gamescene = nullptr, const bool isNotBossStage = false);
	
	//初期化
	bool Initialize()override;

	//リセット処理
	void Reset();
	
	//パラメータ
	void Parameter();
	//更新
	void Update(const bool isStart = false)override;
	//弾発射
	void Fire();
	//描画
	void Draw()override;
	//パーティクル描画
	void DrawParticle()override;

	//ベジェ曲線(最初点、中間点1、中間点2、最終点、時間の進み具合)
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, const unsigned short attribute,
		const unsigned short subAttribute)override;
	
private:
	//モデル	
	const Model* modelBullet_ = nullptr;

	//行動フェーズ
	enum class Phase {
		//ここからステージ1
		ApproachStage1, //接近
		AttackStage1,   //攻撃
		//全体
		Leave, //離脱
		
	};

	//フェーズ
	Phase phase_;
	//弾発射間隔
	int fireInterval_;
	//弾発射タイマー
	int32_t fireTimer_;
	
	//半径
	const float radius_ = 5.0f;
	//ボス死亡
	bool bossDead_ = false;

	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

//時間計測
	std::chrono::steady_clock::time_point startCount_;	//開始時間
	std::chrono::steady_clock::time_point nowCount_;	//現在時間
	std::chrono::microseconds elapsedCount_;			//経過時間 経過時間=現在時間-開始時間
	const float	maxTime_ = 5.0f;						//全体時間
	float	timeRate_;									//どれくらい時間が進んだか
	//制御点
	XMFLOAT3 start_;	//最初点
	XMFLOAT3 point1_;	//中間点1
	XMFLOAT3 point2_;	//中間点2
	XMFLOAT3 end_;		//最終点
	
	//死亡フラグ
	bool isDead_;
	//ライフ
	int life_;

	//反転フラグ
	bool isReverse_ = false;
	//入力
	IwasiEngine::Input* input_ = nullptr;

public:
	//死んだかどうか
	bool IsDead() const override { return isDead_; }
	//プレイヤーセット
	void SetPlayer(const Player* player) { player_ = player; }
	//ゲームシーンセット
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
	//ボスが死んだかどうか
	bool BossDead()const override{ return bossDead_; }
private:
	//ベジェ曲線の動きを共通化(デフォルト引数はボスステージの場合)
	void UpdateBezierMove(const bool notStageBoss = false);
	//X座標をプレイヤーの動きに連動させるための関数(通常ステージ限定)
	void UpdateSynchronizePlayerMove(const bool notStageBoss);

public://状態変化用の更新
	//状態変化用の更新関数
	//接近
	void UpdateApproach();
	//攻撃
	void UpdateAttack();
	//離脱
	void UpdateLeave();
	
private:
	//メンバ関数ポインタのテーブル
	static void (EnemyBoss::* updateTable_[])();
};