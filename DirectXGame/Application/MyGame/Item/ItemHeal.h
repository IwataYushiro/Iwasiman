#pragma once
#include "Camera.h"
#include "ParticleManager.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include "Sprite.h"
#include <list>
#include <memory>

class Player;
class CollisionManager;

/*

*	ItemHeal.h

*	自機ライフ回復アイテム

*/
class ItemHeal :public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~ItemHeal();

	static std::unique_ptr<ItemHeal> Create(Model* model = nullptr, Player* player = nullptr);
	//初期化
	bool Initialize()override;
	//更新
	void Update()override;

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();
	//パーティクル描画
	void DrawParticle();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//ポジション
	XMFLOAT3 pos_;
	XMFLOAT3 scale_;

	bool isGet_ = false;

	float radius_ = 3.0f;

	Player* player_ = nullptr;

	Particle* p_ = nullptr;
	ParticleManager* pm_ = nullptr;

public: //アクセッサ、インライン関数
	bool IsGet() const { return isGet_; }
	void SetPlayer(Player* player) { player_ = player; }
};


