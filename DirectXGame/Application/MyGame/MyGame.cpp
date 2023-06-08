#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	
	sceneFactory_ = new SceneFactory();
	sceneManager_->SetSceneFactory(sceneFactory_);
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
	postEffect_->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	postEffect_->PostDraw(dxCommon_->GetCommandList());

	//描画前処理
	dxCommon_->PreDraw();
	
	//ポストエフェクトの描画
	postEffect_->Draw(dxCommon_->GetCommandList());
	
	//ImGuiの表示
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	Framework::Finalize();
}
