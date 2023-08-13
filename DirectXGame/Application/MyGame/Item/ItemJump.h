#pragma once
#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class Player;
class CollisionManager;

class ItemJump :public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	static std::unique_ptr<ItemJump> Create(Model* model = nullptr, Player* player = nullptr);
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

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute)override;

private:
	static CollisionManager* colManager_;
	//ポジション
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	float count = 200.0f;

	bool isGet_ = false;

	float radius_ =3.0f;

	Player* player_ = nullptr;
public: //アクセッサ、インライン関数
	bool IsGet() const { return isGet_; }
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once
