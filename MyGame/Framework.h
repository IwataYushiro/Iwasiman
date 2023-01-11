#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "SpriteCommon.h"

//ゲーム全体
class Framework
{
public://メンバ関数
	virtual ~Framework() = default;
	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//描画
	virtual void Draw() = 0;
	//終了処理
	virtual void Finalize();
	//終了フラグの受付
	virtual bool IsEndRequest() { return EndGame_; }

	//実行
	void Run();

protected:
	//WinApp
	WinApp* winApp_ = new WinApp();
	//DXCommon
	DirectXCommon* dxCommon_ = new DirectXCommon();
	//SpriteCommon
	SpriteCommon* sprCommon_ = new SpriteCommon();
	//Input
	Input* input_ = new Input();

	//ゲーム終了フラグ
	bool EndGame_ = false;
};