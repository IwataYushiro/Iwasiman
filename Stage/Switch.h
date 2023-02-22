#pragma once
#include "Model.h"
#include "MyMathUtility.h"

class Switch {
public:
	// 初期化
	void Initialize(Model* model);

	// 更新
	void Update();

	// 描画
	void Draw(ViewProjection viewProjection);

	// スイッチオン
	void OnCollisionSwitch();

private:
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	int32_t textureHandle_ = 0u;

	// ワールド変換データ
	WorldTransform worldTransform_;

	// フラグ
	bool isFlag_ = false;

public: // アクセッサ
	bool GetFlag() { return isFlag_; }
	void SetPosition(Vector3 pos) {
		// 初期化
		worldTransform_.Initialize();
		// スケール設定
		worldTransform_.scale_ = { 2.0f, 2.0f, 2.0f };
		// 座標設定
		worldTransform_.translation_ = pos;
		// 行列更新
		worldTransform_.matWorld_ = MyMathUtility::MySetMatrix4Identity();
		worldTransform_.matWorld_ *= MyMathUtility::MySynMatrix4WorldTransform(worldTransform_);
		worldTransform_.TransferMatrix();
	}
};