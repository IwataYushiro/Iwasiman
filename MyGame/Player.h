#pragma once
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

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
	void Initialize(Model* model, Object3d* obj, Input* input);
	//リセット処理
	void Reset();
	
	//更新
	void Update();

	//プレイヤーの移動処理
	void Move();

	//プレイヤーの旋回処理
	void Rotate();

	//プレイヤーの攻撃処理
	void Attack();

	//移動処理制限
	void MoveLimit();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();
	void DrawDead();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

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

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//インプット
	Input* input_ = nullptr;
	
	//ポジション
	XMFLOAT3 pos;
	//アングル
	XMFLOAT3 angle;

	//死亡フラグとライフ
	bool isDead_ = false;
	int life_ = 5;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
};
