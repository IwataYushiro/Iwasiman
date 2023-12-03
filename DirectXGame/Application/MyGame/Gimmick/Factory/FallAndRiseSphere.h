#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	FallAndRiseSphere.h

*	落ちる球と昇る球

*/
class FallAndRiseSphere :public BaseGimmick
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	//生成(使用モデル、プレイヤー、サブ属性)
	static std::unique_ptr<FallAndRiseSphere> Create(Model* model = nullptr, const Player* player = nullptr,
		const unsigned short subAttribute = 0b1000000000000001);
	//初期化
	bool Initialize(Model* model);
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
	const XMFLOAT3 GetWorldPosition()const ;

	//描画
	void Draw()override;

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//ポジション
	XMFLOAT3 pos_;
	//スケール
	XMFLOAT3 scale_;

	//半径
	const float radius_ = 8.0f;

	//プレイヤー
	const Player* player_ = nullptr;

	//乗るとtrue
	bool isRide_ = false;
	//乗って離れるとtrue
	bool isReturn_ = false;

	//開始地点
	XMFLOAT3 startPos_ = {};

	const float resetPosY_ = 50.0f;
public: //アクセッサ、インライン関数
	
	//プレイヤーセット
	void SetPlayer(const Player* player) { player_ = player; }
};