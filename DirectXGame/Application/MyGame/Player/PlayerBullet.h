#pragma once
#include "Object3d.h"
#include "Model.h"
#include "ParticleManager.h"
#include <DirectXMath.h>

/*

*	PlayerBullet.h

*	自機の弾

*/
class PlayerBullet:public IwasiEngine::Object3d
{
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
	static std::unique_ptr<PlayerBullet> Create
	(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model = nullptr);
	//初期化(初期座標、速度)
	bool Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity);
	//リセット処理
	void Reset();

	//更新
	void Update()override;

	//描画
	void Draw();

	//パーティクル描画
	void DrawParticle();
	
	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)override;

	//ワールド座標を取得
	const XMFLOAT3 GetWorldPosition()const;

private://メンバ変数

	//速度
	XMFLOAT3 velocity_;
	//寿命<frm>
	static const int32_t lifeTime_ = 60 * 2;
	//死亡時間
	int32_t deathTimer_ = lifeTime_;
	//死亡フラグ
	bool isDead_ = false;
	//半径
	float radius_ = 4.0f;

	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }

private://カプセル化メンバ関数
	//パーティクル更新
	void UpdateParticle();

};