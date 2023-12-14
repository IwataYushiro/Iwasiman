#pragma once
#include "Object3d.h"
#include "Camera.h"
#include "Model.h"

//前方宣言
//自機クラス
class Player;
//コリジョンマネージャー
class CollisionManager;

/*

*	BaseGimmick.h (cpp X)

*	ギミック基盤クラス

*/
class BaseGimmick :public IwasiEngine::Object3d
{
protected:
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
	//デストラクタ
	virtual ~BaseGimmick() = default;

	//初期化
	virtual bool Initialize() { return Object3d::Initialize(); }
	//virtual bool Initialize([[maybe_unused]] Model* model) { return Object3d::Initialize(); }
	//更新
	virtual void Update() = 0; //{ Object3d::Update(); }
	//描画
	virtual void Draw() = 0; //{ Object3d::Draw(); }

};

