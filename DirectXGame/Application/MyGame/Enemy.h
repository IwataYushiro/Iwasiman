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

//敵
class Enemy {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~Enemy();
	//弾発射間隔
	static const int kFireIntervalStage1 = 40;
	//初期化
	void Initialize(Model* model, Object3d* obj,Camera* camera);

	//リセット処理
	void Reset();
	
	//パラメータ
	void Stage1Parameter();
	//更新
	void Update();
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
	void OnCollisionPlayer();
	
	//弾リストを取得
	const std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets() { return enemyBullets_; }

private:
	
	//弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//モデル
	Model* model_ = nullptr;	
	Model* modelBullet_ = nullptr;

	Object3d* obj_ = nullptr;
	Object3d* objBullet_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

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

	//自機
	Player* player_ = nullptr;
//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	float	maxTime = 10.0f;					//全体時間
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
};