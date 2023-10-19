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
//コリジョンマネージャー
class CollisionManager;
//ゲームプレイシーン
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
	//生成(使用モデル、使用弾モデル、ゲームプレイシーン)
	static std::unique_ptr<Player> Create(Model* model = nullptr, Model* bullet = nullptr,
		GamePlayScene* gamescene = nullptr);
	//初期化
	bool Initialize() override;
	//リセット処理
	void Reset();
	
	//更新(手前と背面ジャンプの有効化、攻撃処理の有効化、開始時演出だと全操作無効)
	void Update(bool isBack = true, bool isAttack = true, bool isStart = false);
	//プレイヤーの移動処理
	void Move();
	
	//落ちる処理とジャンプ処理
	void FallAndJump();
	//奥へ移動
	void JumpBack();
	//着地(属性指定)
	void Landing(unsigned short attribute);
	//プレイヤーの攻撃処理
	void Attack();

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();
	//パーティクル描画
	void DrawParticle();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

	//ベジェ曲線(最初点、中間点1、中間点2、最終点、時間の進み具合)
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	//コリジョンマネージャー
	static CollisionManager* colManager_;
	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	//右を向いてるか
	bool isRight_ = true;
	//モデル
	Model* modelBullet_ = nullptr;
	
	//インプット
	Input* input_ = nullptr;
	
	//アングル
	XMFLOAT3 angle_;

	//ジャンプしてるか
	bool onGround_ = true;
	//ジャンプ力
	float jumpVYFist_ = 2.0f;
	//落下時ベクトル
	XMFLOAT3 fallVec_;

	//手前、奥側に移動
	bool isJumpBack_;
	//奥側にいる場合
	bool isBack_;
	//手前、奥側ジャンプ時のポジション
	XMFLOAT3 jumpBackPos_;

	//時間計測
	std::chrono::steady_clock::time_point startCount_;	//開始時間
	std::chrono::steady_clock::time_point nowCount_;		//現在時間
	std::chrono::microseconds elapsedCount_;				//経過時間 経過時間=現在時間-開始時間
	float	maxTime_ = 1.0f;								//全体時間
	float	timeRate_;									//どれくらい時間が進んだか
	//制御点
	XMFLOAT3 start_;										//最初点
	XMFLOAT3 point1_;									//中間点1
	XMFLOAT3 point2_;									//中間点2
	XMFLOAT3 end_;										//最終点

	//半径
	float radius_ = 1.0f;
	
	//死亡フラグ
	bool isDead_ = false;
	//無敵時間
	int mutekiCount_ = 0;
	//ライフ
	int life_ = 5;
	//敵の攻撃やトゲ等に当たったか
	bool isHit_ = false;
	//シェイク時用のカメラムーブ
	bool cameramove_ = true;
	//パーティクル
	Particle* particleDash_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pmDash_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_ = nullptr;
	//シェイク機能
	bool isShake_ = false;
	//現在視点
	XMFLOAT3 nowEye_;
	//現在注視点
	XMFLOAT3 nowTarget_;
	//シェイク時にスクロールしても元のカメラ視点、注視点に戻るための座標
	XMFLOAT3 hitMove_;

public: //アクセッサ、インライン関数
	//死んだかどうか
	bool IsDead() const { return isDead_; }
	//立ってるかどうか
	bool OnGround()const { return onGround_; }
	//立ち判定のセット
	void SetOnGround(bool og) { this->onGround_ = og; }

	//ジャンプ力セット
	void SetJumpVYFist(float jumpFist) { this->jumpVYFist_ = jumpFist; }
	//ライフセット
	void SetLife(int life) { this->life_ = life; }
	//ライフゲット
	const int& GetLife()const { return life_; }
	//ゲームシーン
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }

};
