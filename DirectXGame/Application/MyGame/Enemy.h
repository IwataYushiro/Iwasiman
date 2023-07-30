#pragma once
#include "Camera.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "Object3d.h"
#include <chrono>
#include <DirectXMath.h>
#include <list>
#include <memory>

//自機クラスの前方宣言
class Player;
class CollisionManager;
class GamePlayScene;

//敵
class Enemy:public Object3d {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~Enemy();
	static std::unique_ptr<Enemy> Create(XMFLOAT3 pos,Model* model = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr);
	//弾発射間隔
	static const int kFireIntervalStage1 = 40;
	//初期化
	bool Initialize(XMFLOAT3 pos);

	//リセット処理
	void Reset(XMFLOAT3 pos);
	
	//パラメータ
	void Stage1Parameter(XMFLOAT3 pos);
	//更新
	void Update()override;
	//転送　
	void Trans();
	//弾発射
	void Fire();
	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();
	//描画
	void Draw();

	//状態変化用の更新関数
	//接近
	void UpdateApproachStage1();
	//攻撃
	void UpdateAttackStage1();
	//離脱
	void UpdateLeave();

	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute)override;
	void OnCollisionPlayer();
	
private:
	static CollisionManager* colManager_;
	
	//モデル	
	Model* modelBullet_ = nullptr;

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
	//弾発射タイマー
	int32_t fireTimer = 0;
	//スケール
	XMFLOAT3 scale;
	//ポジション
	XMFLOAT3 pos;
	//アングル
	XMFLOAT3 angle;
	//半径
	float radius_ = 5.0f;
	//自機
	Player* player_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	float	maxTime = 5.0f;					//全体時間
	float	timeRate;
	//制御点
	XMFLOAT3 start;
	XMFLOAT3 p1;
	XMFLOAT3 p2;
	XMFLOAT3 end;
	
	//死亡フラグとライフ
	bool isDead_;
	int life_;

	//反転フラグ
	bool isReverse_ = false;


public:
	bool IsDead() const { return isDead_; }
	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};