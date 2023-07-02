#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>

class Goal
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	
	//初期化
	void Initialize(Model* model, Object3d* obj, Camera* camera);
	//更新
	void Update();

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

private:
	
	//モデル
	Model* model_ = nullptr;

	Object3d* obj_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//ポジション
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isGoal_ = false;
public: //アクセッサ、インライン関数
	bool IsDead() const { return isGoal_; }
};
#pragma once
