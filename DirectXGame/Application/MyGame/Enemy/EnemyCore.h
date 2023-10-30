#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "ParticleManager.h"
#include <chrono>
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	EnemyCore.h

*	ボス敵のコア

*/
class EnemyCore :public BaseEnemy {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//デストラクタ
	~EnemyCore();
	////生成(使用モデル、使用弾モデル、プレイヤー、ゲームプレイシーン、パラメータレベル)
	static std::unique_ptr<EnemyCore> Create(Model* model = nullptr, Model* bullet = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr, unsigned short stage = 1);

	
	//初期化
	bool Initialize()override;

	//リセット処理
	void Reset();

	//パラメータ
	void Parameter();
	//更新
	void Update(bool isStart = false)override;
	//転送　
	void Trans();
	//弾発射
	void Fire();
	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();
	//描画
	void Draw()override;
	//パーティクル描画
	void DrawParticle()override;

	//状態変化用の更新関数
	//コア更新
	void UpdateCore();
	//コア撃破
	void UpdateBreakCore();

	//離脱
	void UpdateLeave();

	//ベジェ曲線(最初点、中間点1、中間点2、最終点、時間の進み具合)
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;

	//モデル	
	Model* modelBullet_ = nullptr;

	//行動フェーズ
	enum class Phase {
		//ここからステージ1
		CoreStage1,
		CoreBreak,
		//全体
		Leave, //離脱

	};

	//フェーズ
	Phase phase_;
	
	//弾発射間隔
	int fireInterval_;
	//弾発射タイマー
	int32_t fireTimer_ = 0;

	//半径
	float radius_ = 10.0f;
	//ボス死亡
	bool bossDead_ = false;

	//時間計測
	std::chrono::steady_clock::time_point startCount_;	//開始時間
	std::chrono::steady_clock::time_point nowCount_;	//現在時間
	std::chrono::microseconds elapsedCount_;			//経過時間 経過時間=現在時間-開始時間
	float	maxTime_ = 2.0f;							//全体時間
	float	timeRate_;									//どれくらい時間が進んだか
	//制御点
	XMFLOAT3 start_;		//最初点
	XMFLOAT3 point1_;	//中間点1
	XMFLOAT3 point2_;	//中間点2
	XMFLOAT3 end_;		//最終点

	//死亡フラグ
	bool isDead_;
	//ライフ
	int life_;

	//反転フラグ
	bool isReverse_ = false;
	//現在位置取得
	XMFLOAT3 nowPos_ = {};

	//パーティクル
	Particle* particleFire_ = nullptr;
	Particle* particleSmoke_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pmSmoke_ = nullptr;
	ParticleManager* pmFire_ = nullptr;

public:
	//死んだかどうか
	bool IsDead() const override{ return isDead_; }
	//プレイヤーセット
	void SetPlayer(Player* player) { player_ = player; }
	//ゲームシーンセット
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};