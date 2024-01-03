#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include "ParticleManager.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Goal.h

*	ゴール

*/
class Goal :public IwasiEngine::Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::を省略
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using CollisionInfo = IwasiEngine::CollisionInfo;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;
public:
	
	//生成(使用モデル、地形化フラグ)
	static std::unique_ptr<Goal> Create(const Model* model = nullptr, const bool isLandShape = false);
	//初期化(地形化フラグ)
	bool Initialize(const bool isLandShape = false);
	//リセット
	void Reset();
	//更新
	void Update()override;

	//転送
	void Trans();

	//描画
	void Draw();
	//パーティクル描画
	void DrawParticle();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	//ゴールしたか
	bool isGoal_ = false;
	//半径
	const float radius_ = 10.0f;
	//パーティクル
	std::unique_ptr<Particle> particle_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pm_ = nullptr;

public: //アクセッサ、インライン関数
	//ゴールしたか
	bool IsGoal() const { return isGoal_; }
};