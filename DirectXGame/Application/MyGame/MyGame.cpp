#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	BaseScene* scene = new TitleScene();

	sceneManager_->SetNextScene(scene);
	
}

void MyGame::Update()
{
	// DirectX毎フレーム処理　ここから
	// 更新処理ここから
	Framework::Update();

}

void MyGame::Draw()
{
	//描画前処理
	dxCommon_->PreDraw();

	sceneManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	Framework::Finalize();
}
