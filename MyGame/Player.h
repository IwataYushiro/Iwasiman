#pragma once
#include "Audio.h"
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <memory>

class Player {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//初期化
	void Initialize(Model* model, Object3d* obj, Input* input, XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f });

	//更新
	void Update();
	//転送
	void Trans();
	//描画
	void Draw();

	//リセット処理
	void Reset();

	// 死亡
	void IsDead() { isDead_ = true; }

	// 当たり判定
	void OnCollisionStage(bool collisionFlag);
	static void OnCollisionPlayer(bool collisionFlag);

private:
	//プレイヤーの移動処理
	void Move();

private:
	//インプット
	Input* input_ = nullptr;
	
	//モデル
	Model* model_ = nullptr;
	Object3d* obj_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//サウンドデータ
	uint32_t jumpSound_ = 0;

	// ワールド変換データ
	XMFLOAT3 pos_;
	// 前フレーム座標
	XMFLOAT3 prePos_{};
	// 半径
	float radius_ = 1.5f;
	//死亡フラグ
	bool isDead_;
	static bool isDeads_;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
	XMFLOAT3 GetPosition() const { return pos_; }
	float GetRadius() const { return radius_; }

};
