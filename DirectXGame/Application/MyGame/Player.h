#pragma once
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include "ParticleManager.h"

#include <DirectXMath.h>
#include <list>
#include <memory>
#include <chrono>

class CollisionManager;
class GamePlayScene;

class Player:public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	const int MUTEKI_COUNT = 60;
public:
	~Player();

	static std::unique_ptr<Player> Create(Model* model = nullptr,
		GamePlayScene* gamescene = nullptr);
	//初期化
	bool Initialize() override;
	//リセット処理
	void Reset();
	
	//更新
	void Update() override;
	//プレイヤーの移動処理
	void Move();
	void CameraMove();
	//プレイヤーの攻撃処理
	void Attack();

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();
	void DrawParticle();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

	//ベジェ曲線
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	static CollisionManager* colManager_;
	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	bool isRight_ = true;
	//モデル
	Model* modelBullet_ = nullptr;
	
	//インプット
	Input* input_ = nullptr;
	
	//ポジション
	XMFLOAT3 pos;
	//アングル
	XMFLOAT3 angle;

	//ジャンプしてるか
	bool onGround = true;
	float jumpVYFist = 2.0f;
	XMFLOAT3 fallVec;

	//奥側に移動
	bool isJumpBack;
	bool isBack;

	//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	float	maxTime = 1.0f;					//全体時間
	float	timeRate;
	//制御点
	XMFLOAT3 start;
	XMFLOAT3 p1;
	XMFLOAT3 p2;
	XMFLOAT3 end;

	//半径
	float radius_ = 1.0f;
	
	//死亡フラグとライフ
	bool isDead_ = false;
	int mutekiCount = 0;
	int life_ = 5;
	bool ishit = false;
	bool cameramove = true;
	//パーティクル
	Particle* particleDash_ = nullptr;
	ParticleManager* pmDash_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
	bool OnGround()const { return onGround; }
	void SetOnGround(bool og) { this->onGround = og; }

	//ジャンプ力
	void SetJumpVYFist(float jumpFist) { this->jumpVYFist = jumpFist; }
	//ライフ
	void SetLife(int life) { this->life_ = life; }
	const int& GetLife()const { return life_; }
	//ゲームシーン
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};
