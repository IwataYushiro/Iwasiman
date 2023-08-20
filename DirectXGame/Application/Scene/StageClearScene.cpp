#include "StageClearScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>


DirectXCommon* StageClearScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageClearScene::input_ = Input::GetInstance();
Audio* StageClearScene::audio_ = Audio::GetInstance();
SceneManager* StageClearScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageClearScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* StageClearScene::camera_ = Camera::GetInstance();


StageClearScene::StageClearScene(int stagenum) :stageNum(stagenum)
{	
}

void StageClearScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// 注視点座標
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//レベルデータ読み込み
	//LoadLVData();

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT StageTex = 00;
	if (stageNum == 2)spCommon_->LoadTexture(StageTex, "texture/gameclear.png");
	else spCommon_->LoadTexture(StageTex, "texture/stageclear.png");
	spriteStageClear_->Initialize(spCommon_, StageTex);

}

void StageClearScene::Update()
{

	
	if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		if (stageNum == 1) sceneManager_->ChangeScene("GAMEPLAY", 2);
		else if (stageNum == 2)sceneManager_->ChangeScene("TITLE");
	}
spriteStageClear_->Update();

	camera_->Update();
	lightGroup_->Update();
	
}

void StageClearScene::Draw()
{
	//背景スプライト描画前処理
	spCommon_->PreDraw();
	//スプライト描画
	spriteStageClear_->Draw();


	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	
	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//前景スプライト


}

void StageClearScene::Finalize()
{
	//スプライト
	delete spriteStageClear_;
	
}


