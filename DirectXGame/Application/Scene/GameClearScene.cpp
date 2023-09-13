#include "GameClearScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>


DirectXCommon* GameClearScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameClearScene::input_ = Input::GetInstance();
Audio* GameClearScene::audio_ = Audio::GetInstance();
SceneManager* GameClearScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameClearScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* GameClearScene::camera_ = Camera::GetInstance();


void GameClearScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	BGM = audio_->SoundLoadWave("Resources/sound/bgm/ending.wav");
	doneSE = audio_->SoundLoadWave("Resources/sound/se/done.wav");

	audio_->SoundPlayWave(audio_->GetXAudio2(), BGM, false);

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
	spCommon_->LoadTexture(StageTex, "texture/gameclear.png");
	spriteGameClear_->Initialize(spCommon_, StageTex);

}

void GameClearScene::Update()
{


	if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		audio_->SoundPlayWave(audio_->GetXAudio2(), doneSE, false);
		sceneManager_->ChangeScene("TITLE");
		
	}
	spriteGameClear_->Update();
	camera_->Update();
	lightGroup_->Update();

}

void GameClearScene::Draw()
{
	
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
//背景スプライト描画前処理
	spCommon_->PreDraw();
	//スプライト描画
	spriteGameClear_->Draw();



}

void GameClearScene::Finalize()
{
	//音声
	audio_->Finalize();
	audio_->SoundUnLoad(&BGM);
	audio_->SoundUnLoad(&doneSE);

	delete lightGroup_;
	//スプライト
	delete spriteGameClear_;

}


