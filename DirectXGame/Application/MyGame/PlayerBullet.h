#pragma once
#include "Object3d.h"
#include "Model.h"
#include <DirectXMath.h>

class CollisionManager;
//自キャラの弾
class PlayerBullet:public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	static std::unique_ptr<PlayerBullet> Create
	(const XMFLOAT3& position, const XMFLOAT3& velocity, Model* model = nullptr);
	//初期化
	bool Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity);
	//リセット処理
	void Reset();

	//更新
	void Update()override;

	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute)override;

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

private:
	static CollisionManager* colManager_;

	//速度
	XMFLOAT3 velocity_;
	//寿命<frm>
	static const int32_t kLifeTime = 60;
	//死亡時間
	int32_t deathTimer_ = kLifeTime;
	//死亡フラグ
	bool isDead_ = false;
	//半径
	float radius_ = 4.0f;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
};