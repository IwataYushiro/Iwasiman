#pragma once
#pragma once
#include "BaseEnemy.h"

#include "Camera.h"

#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

//敵
class Enemy2 :public BaseEnemy {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	int MAX_GROUND = 60;

public:
	~Enemy2();
	static std::unique_ptr<Enemy2> Create(Model* model = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr);

	//弾発射間隔
	int kFireInterval;
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
	void Draw()override;

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
	//地面に当たった時
	XMFLOAT3 upPos;
	//アングル
	XMFLOAT3 angle;
	//半径
	float radius_ = 3.0f;


	bool onGround = true;
	XMFLOAT3 fallVec;

	//反転フラグ
	bool isReverse_ = false;

	int count = 0;

public:

	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};