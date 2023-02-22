#pragma once
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>

class Switch {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	// 初期化
	void Initialize(Model* model, Object3d* obj);

	// 更新
	void Update();
	//転送
	void Trans();
	// 描画
	void Draw();

	// スイッチオン
	void OnCollisionSwitch();

private:
	// モデル
	Model* model_ = nullptr;
	//オブジェクト
	Object3d* obj_ = nullptr;
	// ワールド変換データ
	XMFLOAT3 pos_;
	XMFLOAT3 scale_;

	// フラグ
	bool isFlag_ = false;

public: // アクセッサ
	bool GetFlag() { return isFlag_; }
	void SetPosition(XMFLOAT3 pos); 
};