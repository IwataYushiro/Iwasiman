#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class Player;
class CollisionManager;

//当たると痛いトゲ
class Spike :public BaseGimmick
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	static std::unique_ptr<Spike> Create(Model* model = nullptr, Player* player = nullptr);
	//初期化
	bool Initialize()override;
	//更新
	void Update()override;
	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw()override;

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//ポジション
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isSpike_ = false;

	float radius_ = 5.0f;

	Player* player_ = nullptr;

public: //アクセッサ、インライン関数
	bool IsSpike() const { return isSpike_; }
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once
