#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "Model.h"
#include "ParticleManager.h"

#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Enemy1.h

*	そのまま歩き続ける敵

*/
class Enemy1 :public BaseEnemy {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//デストラクタ
	~Enemy1();

	//生成(使用モデル、使用弾モデル、プレイヤー、ゲームプレイシーン、パラメータレベル)
	static std::unique_ptr<Enemy1> Create(const Model* model = nullptr,  Model* bullet = nullptr,
		const Player* player = nullptr, GamePlayScene* gamescene = nullptr, int level = 1);

	//初期化
	bool Initialize(int level);
	//サブ属性初期化
	void InitSubATTR(int level);
	//サブ属性別速度初期化
	void InitSpeed();
	//サブ属性別ライフ初期化
	void InitLife();

	//リセット処理
	void Reset();

	//パラメータ
	void Parameter();
	//更新
	void Update(bool isStart = false)override;
	//転送　
	void Trans();
	//弾発射
	void Fire();
	//着地処理
	void Landing();
	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();
	//描画
	void Draw()override;
	//パーティクル描画
	void DrawParticle()override;

	//状態変化用の更新関数
	//接近
	void UpdateApproach();
	//離脱
	void UpdateLeave();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute)override;

private:
	//コリジョンマネージャー
	static CollisionManager* colManager_;

	//モデル	
	Model* modelBullet_ = nullptr;

	//行動フェーズ
	enum class Phase {
		//ここからステージ1
		Approach,//登場
		Leave, //離脱

	};

	//フェーズ
	Phase phase_;

	//弾発射間隔
	int fireInterval_;
	//弾発射タイマー
	int32_t fireTimer_;
	//アングル
	XMFLOAT3 angle_;
	//半径
	const float radius_ = 3.0f;
	
	//地面に乗ってるか
	bool onGround_ = true;
	//落下時ベクトル
	XMFLOAT3 fallVec_;

	//反転フラグ
	bool isReverse_ = false;

	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;
public:
	
	//プレイヤーセット
	void SetPlayer(const Player* player) { player_ = player; }
	//ゲームプレイシーンセット
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};