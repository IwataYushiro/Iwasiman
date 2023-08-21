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
class Enemy1 :public Object3d {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~Enemy1();
	static std::unique_ptr<Enemy1> Create(Model* model = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr);
	//弾発射間隔
	static const int kFireInterval = 150;
	//初期化
	bool Initialize()override;

	//リセット処理
	void Reset();

	//パラメータ
	void Parameter();
	//更新
	void Update()override;
	//転送　
	void Trans();
	//弾発射
	void Fire();
	//着地処理
	void Landing();
	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();
	//描画
	void Draw();

	//状態変化用の更新関数
	//接近
	void UpdateApproach();
	//離脱
	void UpdateLeave();

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
		Approach,
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
	float radius_ = 1.0f;
	//自機
	Player* player_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

	//死亡フラグとライフ
	bool isDead_;
	int life_;

	bool onGround = true;
	XMFLOAT3 fallVec;

	//反転フラグ
	bool isReverse_ = false;


public:
	bool IsDead() const { return isDead_; }
	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};