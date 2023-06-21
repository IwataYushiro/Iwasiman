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
	Variation = postCount % POST_NUM;
	pe[Variation].PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	pe[Variation].PostDraw(dxCommon_->GetCommandList());

	//描画前処理
	dxCommon_->PreDraw();
	
	//ポストエフェクトの描画
	pe[Variation].Draw(dxCommon_->GetCommandList());

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
	
	pe = new PostEffect[POST_NUM];

	pe[0].Initialize(sprCommon_);
	pe[1].Initialize(sprCommon_, "GaussianBlur");
	pe[2].Initialize(sprCommon_, "Sepia");
	pe[3].Initialize(sprCommon_, "Cold");
	pe[4].Initialize(sprCommon_, "NegaPozi");
	pe[5].Initialize(sprCommon_, "GrayScale");
	pe[6].Initialize(sprCommon_, "Mosaic");
	pe[7].Initialize(sprCommon_, "UVShift");
	pe[8].Initialize(sprCommon_, "UVShiftBlur");
	
	
}

void MyGame::PostDraw()
{
	//int Variation = postCount % POST_NUM;
	//ポストエフェクトの描画
	//
	

	
}


void MyGame::PostDelete()
{
	delete[] pe;

	
}
