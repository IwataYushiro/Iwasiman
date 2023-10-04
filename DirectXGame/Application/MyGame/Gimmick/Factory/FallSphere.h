#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class Player;
class CollisionManager;

//落ちる球と昇る球
class FallSphere :public BaseGimmick
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	static std::unique_ptr<FallSphere> Create(Model* model = nullptr, Player* player = nullptr, unsigned short subAttribute = 0b1000000000000001);
	//初期化
	bool Initialize()override;
	//更新
	void Update()override;
	//更新(おちっぱ)
	void UpdateFallSphere();
	//戻る
	void UpdateFallSphereReturn();
	//更新(のぼりっぱ)
	void UpdateRiseSphere();
	//戻る
	void UpdateRiseSphereReturn();

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
	XMFLOAT3 pos_;
	XMFLOAT3 scale_;

	bool isFallSphere_ = false;

	float radius_ = 9.0f;

	Player* player_ = nullptr;

	//乗るとtrue
	bool isRide_ = false;
	//乗って離れるとtrue
	bool isReturn_ = false;

	XMFLOAT3 startPos_ = {};

public: //アクセッサ、インライン関数
	bool IsFallSphere() const { return isFallSphere_; }
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once
