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
	PostEffectDraw();//ポストエフェクト描画
}

void MyGame::Finalize()
{
	Framework::Finalize();//終了処理
}

void MyGame::PostInitialize()
{
	//ポストエフェクト初期化
	pe_ = std::make_unique<PostEffect>();
	pe_->Initialize(sprCommon_);
}

void MyGame::PostEffectDraw()
{
	//ポストエフェクトの前にシーンを描画
	pe_->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	pe_->PostDraw(dxCommon_->GetCommandList());

	//描画前処理
	dxCommon_->PreDraw();

	//その後にポストエフェクトの描画
	pe_->Draw(dxCommon_->GetCommandList());

	//ImGuiの表示
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

