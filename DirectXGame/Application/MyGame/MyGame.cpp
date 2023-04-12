#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	scene = new GamePlayScene();
	scene->Initialize();
	
}

void MyGame::Update()
{

	// DirectX毎フレーム処理　ここから
	// 更新処理ここから
	Framework::Update();

	scene->Update();
}

void MyGame::Draw()
{
	//描画前処理
	dxCommon_->PreDraw();

	scene->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	scene->Finalize();
	
	Framework::Finalize();
}
