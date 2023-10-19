#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Spike.h

*	当たると痛いトゲ

*/
class Spike :public BaseGimmick
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//生成(使用モデル、プレイヤー)
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

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	//コリジョンマネージャー
	static CollisionManager* colManager_;
	//半径
	float radius_ = 5.0f;

	//プレイヤー
	Player* player_ = nullptr;

public: //アクセッサ、インライン関数
	//プレイヤーセット
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once
