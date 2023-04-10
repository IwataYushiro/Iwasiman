#pragma once
#include "WinApp.h"
#include "Audio.h"
#include "Camera.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
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
	WinApp* winApp_ = nullptr;
	//DXCommon
	DirectXCommon* dxCommon_ = nullptr;
	//SpriteCommon
	SpriteCommon* sprCommon_ = nullptr;
	//Audio
	Audio* audio_ = nullptr;
	//Input
	Input* input_ = nullptr;
	//imgui
	ImGuiManager* imguiManager_ = nullptr;

	
	//ゲーム終了フラグ
	bool EndGame_ = false;
};