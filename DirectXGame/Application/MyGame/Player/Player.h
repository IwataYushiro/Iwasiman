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

//前方宣言
class CollisionManager;
class GamePlayScene;

/*

*	Player.h

*	自機

*/
class Player:public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//最大無敵時間
	const int MUTEKI_COUNT = 60;
public:
	//デストラクタ
	~Player();

	static std::unique_ptr<Player> Create(Model* model = nullptr, Model* bullet = nullptr,
		GamePlayScene* gamescene = nullptr);
	//初期化
	bool Initialize() override;
	//リセット処理
	void Reset();
	
	//更新
	void Update(bool isBack = true, bool isAttack = true);
	//プレイヤーの移動処理
	void Move();
	void CameraMove();
	//落ちる＆ジャンプ
	void FallAndJump();
	//奥へ移動
	void JumpBack();
	//着地
	void Landing(unsigned short attribute);
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
	XMFLOAT3 pos_;
	//アングル
	XMFLOAT3 angle_;

	//ジャンプしてるか
	bool onGround_ = true;
	float jumpVYFist_ = 2.0f;
	XMFLOAT3 fallVec_;

	//奥側に移動
	bool isJumpBack_;
	bool isBack_;
	XMFLOAT3 jumpBackPos_;

	//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	float	maxTime = 1.0f;					//全体時間
	float	timeRate;
	//制御点
	XMFLOAT3 start;
	XMFLOAT3 point1;
	XMFLOAT3 point2;
	XMFLOAT3 end;

	//半径
	float radius_ = 1.0f;
	
	//死亡フラグとライフ
	bool isDead_ = false;
	int mutekiCount_ = 0;
	int life_ = 5;
	bool isHit_ = false;
	bool cameramove_ = true;
	//パーティクル
	Particle* particleDash_ = nullptr;
	ParticleManager* pmDash_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;
	//シェイク機能
	bool isShake_ = false;
	//スタート時
	XMFLOAT3 nowEye_;
	XMFLOAT3 nowTarget_;
	XMFLOAT3 hitMove_;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
	bool OnGround()const { return onGround_; }
	void SetOnGround(bool og) { this->onGround_ = og; }

	//ジャンプ力
	void SetJumpVYFist(float jumpFist) { this->jumpVYFist_ = jumpFist; }
	//ライフ
	void SetLife(int life) { this->life_ = life; }
	const int& GetLife()const { return life_; }
	//ゲームシーン
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }

};
