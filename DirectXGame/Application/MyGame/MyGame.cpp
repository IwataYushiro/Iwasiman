#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;

/*

*	MyGame.cpp

*	ゲーム全体

*/

void MyGame::Initialize()
{
	Framework::Initialize();
	
	
	sceneFactory_ = new SceneFactory();
	sceneManager_->SetSceneFactory(sceneFactory_);
	sceneManager_->ChangeScene("TITLE");

	//ポストエフェクト
	PostInitialize();
}

void MyGame::Update()
{
	// DirectX毎フレーム処理　ここから
	// 更新処理ここから
	Framework::Update();
}

void MyGame::Draw()
{
	PostDraw();
}

void MyGame::Finalize()
{
	PostDelete();
	Framework::Finalize();
}

void MyGame::PostInitialize()
{
	pe_ = new PostEffect();
	pe_->Initialize(sprCommon_,"Cold");
}

void MyGame::PostDraw()
{
	pe_->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	pe_->PostDraw(dxCommon_->GetCommandList());

	//描画前処理
	dxCommon_->PreDraw();

	//ポストエフェクトの描画
	pe_->Draw(dxCommon_->GetCommandList());

	//ImGuiの表示
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}


void MyGame::PostDelete()
{
	delete pe_;
}
