#pragma once

#include "BaseGimmick.h"

#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Spike.h

*	当たると痛いトゲ

*/
class Spike :public BaseGimmick
{
public:
	//生成(使用モデル、プレイヤー)
	static std::unique_ptr<Spike> Create(const Model* model = nullptr, const Player* player = nullptr);
	//初期化
	bool Initialize()override;
	//更新
	void Update()override;
	//転送
	void Trans();

	//ワールド座標を取得
	const XMFLOAT3 GetWorldPosition()const;

	//描画
	void Draw()override;

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	
	//半径
	const float radius_ = 1.0f;

	//プレイヤー
	const Player* player_ = nullptr;

public: //アクセッサ、インライン関数
	//プレイヤーセット
	void SetPlayer(const Player* player) { player_ = player; }
};
#pragma once
