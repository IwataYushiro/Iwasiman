#include "Framework.h"
#include "Object3d.h"
#include "ParticleManager.h"

void Framework::Initialize()
{
	//WinApp
	winApp_ = new WinApp();
	//DXCommon
	dxCommon_ = new DirectXCommon();
	//SpriteCommon
	sprCommon_ = new SpriteCommon();
	//オーディオ
	audio_ = new Audio();
	//Input
	input_ = new Input();
	//imgui
	imguiManager_ = new ImGuiManager();

	//WinApp初期化
	winApp_->Initialize();
	//DirectX初期化
	dxCommon_->Initialize(winApp_);
	//スプライト基盤
	sprCommon_->Initialize(dxCommon_);
	//オーディオ
	audio_->Initialize();
	//入力
	input_->Initialize(winApp_);
	//imgui
	imguiManager_->Initialize(winApp_, dxCommon_);

	Object3d::StaticInitialize(dxCommon_->GetDevice());
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());
}

void Framework::Update()
{
	//Windowsのメッセージ処理
	if (winApp_->ProcessMessage())
	{
		//ゲーム終了
		EndGame_ = true;
	}
	//入力の更新
	input_->Update();
	
}

void Framework::Finalize()
{
	//imgui
	imguiManager_->Finalize();
	//WinApp
	winApp_->Finalize();

	//基盤類
	delete imguiManager_;
	delete audio_;
	delete sprCommon_;
	delete input_;
	delete dxCommon_;
	delete winApp_;
}


void Framework::Run()
{
	//初期化
	Initialize();

	//ゲームループ
	while (true)
	{
		Update();

		//終了リクエストが来たらループを抜ける
		if (IsEndRequest())
		{
			//ゲームループを抜ける
			break;
		}
		//描画処理ここから
		Draw();
		// 描画処理ここまで

		// DirectX毎フレーム処理　ここまで

	}
	//終了処理
	Finalize();
}