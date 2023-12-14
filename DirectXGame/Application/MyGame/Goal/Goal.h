#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Goal.h

*	ゴール

*/
class Goal :public IwasiEngine::Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::を省略
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using CollisionInfo = IwasiEngine::CollisionInfo;
public:
	
	//生成(使用モデル)
	static std::unique_ptr<Goal> Create(const Model* model = nullptr);
	//初期化
	bool Initialize()override;
	//リセット
	void Reset();
	//更新
	void Update()override;

	//転送
	void Trans();

	//ワールド座標を取得
	const XMFLOAT3 GetWorldPosition()const;

	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	//ゴールしたか
	bool isGoal_ = false;
	//半径
	const float radius_ = 10.0f;

public: //アクセッサ、インライン関数
	//ゴールしたか
	bool IsGoal() const { return isGoal_; }
};
#pragma once
