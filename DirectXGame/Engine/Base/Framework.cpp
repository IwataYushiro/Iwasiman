#include "Framework.h"
#include "LightGroup.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "ParticleManager.h"

using namespace IwasiEngine;

/*

*	Framework.cpp

*	ゲーム全体フレームワーク

*/

void Framework::Initialize()
{
	//インスタンス取得
	//WinApp
	winApp_ = WinApp::GetInstance();
	//DXCommon
	dxCommon_ = DirectXCommon::GetInstance();
	//SpriteCommon
	sprCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_ = Audio::GetInstance();
	//Input
	input_ = Input::GetInstance();
	//SceneManager
	sceneManager_ = SceneManager::GetInstance();
	//imgui
	imguiManager_ = ImGuiManager::GetInstance();

	//WinApp初期化
	winApp_->Initialize();
	//DirectX初期化
	dxCommon_->Initialize();
	//スプライト基盤
	sprCommon_->Initialize();
	//入力
	input_->Initialize();
	//imgui
	imguiManager_->Initialize();
	//FBX
	FbxLoader::GetInstance()->Initialize();
	//静的初期化
	//3Dオブジェクト(.obj)
	Object3d::StaticInitialize();
	//3Dオブジェクト(.fbx)
	ObjectFbx::StaticInitialize();
	//パーティクルマネージャー
	ParticleManager::StaticInitialize();
	//ライト
	LightGroup::StaticInitialize();

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

	//シーン更新
	sceneManager_->Update();


}

void Framework::Finalize()
{
	//シーン
	sceneManager_->Finalize();
	//FBX
	FbxLoader::GetInstance()->Finalize();
	//imgui
	imguiManager_->Finalize();
	//WinApp
	winApp_->Finalize();
}


void Framework::Run()
{
	//初期化
	Initialize();

	//ゲームループ
	while (true)
	{
		//更新
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