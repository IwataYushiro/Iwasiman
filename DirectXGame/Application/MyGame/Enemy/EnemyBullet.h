#pragma once
#include "Model.h"
#include "Object3d.h"
#include "ParticleManager.h"
#include <DirectXMath.h>

/*

*	EnemyBullet.h

*	敵の弾

*/
class EnemyBullet: public IwasiEngine::Object3d {
private:
	// DirectX::を省略
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::を省略
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using CollisionInfo = IwasiEngine::CollisionInfo;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;

public:
	//生成(初期座標、速度、使用モデル)
	static std::unique_ptr<EnemyBullet> Create(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model = nullptr);
	//初期化(初期座標、速度)
	bool Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity);
	//リセット処理
	void Reset();

	//更新
	void Update()override;
	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	//速度
	XMFLOAT3 velocity_;

	//寿命
	static const int32_t lifeTime_ = 60 * 2;
	//死亡時間
	int32_t deathTimer_ = lifeTime_;
	//死亡フラグ
	bool isDead_ = false;
	//半径
	float radius_ = 4.0f;

public: //アクセッサ、インライン関数
	//死んだかどうか
	bool IsDead() const { return isDead_; }

};