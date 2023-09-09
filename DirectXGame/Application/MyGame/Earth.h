#pragma once
#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class CollisionManager;

class Earth :public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	const int MUTEKI_COUNT = 60;
public:

	static std::unique_ptr<Earth> Create(Model* model = nullptr);
	//初期化
	bool Initialize()override;
	void Reset();
	//更新
	void Update()override;

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//ポジション
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isHit_ = false;
	bool isDead_ = false;
	int mutekiCount = 0;
	int life_ = 3;
	
	float radius_ = 50.0f;
public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
	//ライフ
	void SetLife(int life) { this->life_ = life; }
	const int& GetLife()const { return life_; }
};
#pragma once
