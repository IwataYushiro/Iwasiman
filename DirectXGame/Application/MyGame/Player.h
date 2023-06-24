#pragma once
#include "Camera.h"
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include <DirectXMath.h>
#include <list>
#include <memory>
#include <chrono>

class Player
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~Player();

	//初期化
	void Initialize(Model* model, Object3d* obj, Input* input, Camera* camera);
	//リセット処理
	void Reset();
	
	//更新
	void Update();

	//プレイヤーの移動処理
	void Move();
	void CameraMove();
	//ジャンプ
	void Jump();
	//奥へ移動
	void JumpBack();
	// 奥移動
	
	//プレイヤーの攻撃処理
	void Attack();

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	//ベジェ曲線
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	
	//モデル
	Model* model_ = nullptr;
	Model* modelBullet_ = nullptr;

	Object3d* obj_ = nullptr;
	Object3d* objBullet_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//インプット
	Input* input_ = nullptr;
	
	//ポジション
	XMFLOAT3 pos;
	//アングル
	XMFLOAT3 angle;

	//ジャンプしてるか
	bool isJump;
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

	//ジャンプ力
	const float power = 2.0f;
	//重力
	float gravity = 0.0f;
	
	//死亡フラグとライフ
	bool isDead_ = false;
	int life_ = 5;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
};
