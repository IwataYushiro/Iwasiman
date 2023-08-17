#pragma once
#include "Camera.h"
#include "Easing.h"
#include "ParticleManager.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include "Sprite.h"
#include <list>
#include <memory>

class Player;
class CollisionManager;

//アイテム管理
class Item :public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://定数
	const float MAX_TIME = 200.0f;
public:
	~Item();
	
	static std::unique_ptr<Item> Create(Model* model = nullptr, Player* player = nullptr
		, unsigned short subAttribute = 0b1000000000000000);
	//初期化
	bool Initialize(unsigned short subAttribute);
	//更新
	void Update()override;
	//ジャンプ強化
	void UpdateJumpPowerup();
	//ライフ回復(は別にupdate無くてもよさそう)

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();
	//パーティクル描画
	void DrawParticle();
	//スプライト描画
	void DrawSprite();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute,unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//ポジション
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isGet_ = false;
	//ジャンプ
	bool isGetJump_ = false;

	float radius_ = 3.0f;

	Player* player_ = nullptr;

	//イージング
	float count = 0.0f;
	const float timer = MAX_TIME / 60.0f;
	Easing ease = Easing(1.0f, 0.0f, timer);

	//パーティクル
	Particle* p = nullptr;
	ParticleManager* pm_ = nullptr;

public: //アクセッサ、インライン関数
	bool IsGetJump() const { return isGetJump_; }

	void SetPlayer(Player* player) { player_ = player; }
	Easing GetEasing()const { return ease; }

private:
	SpriteCommon* spCommon_ = SpriteCommon::GetInstance();
	Sprite* spriteItemJumpBar_ = new Sprite();

private:
	void LoadSprite();
};

