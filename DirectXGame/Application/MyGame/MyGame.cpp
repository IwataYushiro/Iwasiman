#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;

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
	if (input_->TriggerKey(DIK_0))
	{
		postCount++;
	}
	
}

void MyGame::Draw()
{
	pe1->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	pe1->PostDraw(dxCommon_->GetCommandList());

	//描画前処理
	dxCommon_->PreDraw();
	
	//ポストエフェクトの描画
	//PostDraw();
	pe1->Draw(dxCommon_->GetCommandList());

	//ImGuiの表示
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	PostDelete();
	Framework::Finalize();
}

void MyGame::PostInitialize()
{
	pe1 = new PostEffect();

	pe1->Initialize(sprCommon_, "Test");

}

void MyGame::PostDraw()
{
	//int Variation = postCount % POST_NUM;
	//ポストエフェクトの描画
	//pe[Variation].Draw(dxCommon_->GetCommandList());
	

	
}


void MyGame::PostDelete()
{
	//delete[] pe;

	delete pe1;
	delete pe2;
}
