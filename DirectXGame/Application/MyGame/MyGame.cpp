#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;
using namespace IwasiEngine;

/*

*	MyGame.cpp

*	ゲーム全体

*/

void MyGame::Initialize()
{
	//フレームワーク初期化
	Framework::Initialize();
	
	//シーンファクトリー
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("TITLE");
}

void MyGame::Update()
{
	// DirectX毎フレーム処理　ここから
	// 更新処理ここから
	Framework::Update();
}

void MyGame::Draw()
{
	//ポストエフェクトの前にシーンを描画
	sceneManager_->Draw();
	//描画前処理
	dxCommon_->PreDraw();
	//その後にポストエフェクトの描画
	sceneManager_->DrawPostEffect();
	//ImGuiの表示
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	Framework::Finalize();//終了処理
}

