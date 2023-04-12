#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	scene_ = new TitleScene();
	scene_->Initialize();
	
}

void MyGame::Update()
{

	// DirectX毎フレーム処理　ここから
	// 更新処理ここから
	Framework::Update();

	scene_->Update();
}

void MyGame::Draw()
{
	//描画前処理
	dxCommon_->PreDraw();

	scene_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	scene_->Finalize();
	delete scene_;
	
	Framework::Finalize();
}
