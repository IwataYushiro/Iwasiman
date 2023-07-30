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
	/*if (input_->TriggerMouse(0))
	{
		postCount++;
	}*/
	Variation = postCount % POST_NUM;
}

void MyGame::Draw()
{
	
	if (Variation == 11)
	{
		dxCommon_->PreDraw();

		//ポストエフェクトの描画
		sceneManager_->Draw();

		//ImGuiの表示
		imguiManager_->Draw();
		//描画後処理
		dxCommon_->PostDraw();
	}
	else
	{
		PostDraw();
	}
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
	pe[2].Initialize(sprCommon_, "Bloom");
	pe[3].Initialize(sprCommon_, "CrossFilter");
	pe[4].Initialize(sprCommon_, "Sepia");
	pe[5].Initialize(sprCommon_, "Cold");
	pe[6].Initialize(sprCommon_, "NegaPozi");
	pe[7].Initialize(sprCommon_, "GrayScale");
	pe[8].Initialize(sprCommon_, "Mosaic");
	pe[9].Initialize(sprCommon_, "UVShift");
	pe[10].Initialize(sprCommon_, "UVShiftBlur");
	
	
}

void MyGame::PostDraw()
{
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


void MyGame::PostDelete()
{
	delete[] pe;
}
