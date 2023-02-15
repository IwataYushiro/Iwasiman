#pragma once
#include "Object3d.h"
#include "Model.h"
#include <DirectXMath.h>

//自キャラの弾
class PlayerBullet {
private:
	// DirectX::を省略
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	//初期化
	void Initialize(Model* model, Object3d* obj, const XMFLOAT3& position, const XMFLOAT3& velocity);
	//リセット処理
	void Reset();

	//更新
	void Update();

	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

private:
	
	//モデル
	Model* model_ = nullptr;
	Object3d* obj_ = nullptr;
	//速度
	XMFLOAT3 velocity_;
	//寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	//死亡時間
	int32_t deathTimer_ = kLifeTime;
	//死亡フラグ
	bool isDead_ = false;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
};