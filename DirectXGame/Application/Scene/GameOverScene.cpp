#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "StageList.h"

using namespace DirectX;

DirectXCommon* GameOverScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();
Audio* GameOverScene::audio_ = Audio::GetInstance();
SceneManager* GameOverScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameOverScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* GameOverScene::camera_ = Camera::GetInstance();

GameOverScene::GameOverScene(int stagenum) :stageNum(stagenum) {}

void GameOverScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	//カメラ
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//camera_->SetTarget({ 90.0f,0.0f,0.0f });
	//camera_->SetEye({ -10.0f,2.0f,0.0f });

	// 視点座標
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// 注視点座標
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//レベルデータ読み込み
	//LoadLVData("enemytest");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(GOSTI_MenuTex, "texture/gameover2.png");
	spriteGameOver_->Initialize(spCommon_, GOSTI_MenuTex);
	spriteGameOver_->SetPosition({ easeMenuPosX[0].start,0.0f });

	spCommon_->LoadTexture(GOSTI_ContinueTex, "texture/continue.png");
	spriteContinue_->Initialize(spCommon_, GOSTI_ContinueTex);
	spriteContinue_->SetPosition({ easeMenuPosX[1].start,150.0f });

	spCommon_->LoadTexture(GOSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, GOSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX[2].start,300.0f });

	spCommon_->LoadTexture(GOSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, GOSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX[3].start,450.0f });

	spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, GOSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX[4].start,580.0f });

	modelStageTutorial = Model::LoadFromOBJ("skydomet");
	modelStage1 = Model::LoadFromOBJ("skydome");
	modelStage2 = Model::LoadFromOBJ("skydome2");

	objStage = Object3d::Create();
	objStage->SetModel(modelStage2);
	objStage->SetCamera(camera_);
	objStage->SetScale({ 7.0f,7.0f,7.0f });


	easeTitlePosX.Standby(false);
	for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(false);
}

void GameOverScene::Update()
{
	if (MenuCount <= GOSMI_Continue)MenuCount = GOSMI_Continue;
	else if (MenuCount >= GOSMI_Title)MenuCount = GOSMI_Title;

	//イージング
	easeTitlePosX.ease_out_expo();
	for (int i = 0; i < 4; i++)easeMenuPosX[i].ease_out_expo();

	//座標セット
	spriteGameOver_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
	spriteContinue_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
	spriteStageSelect_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
	spriteTitle_->SetPosition({ easeMenuPosX[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX[4].num_X,550.0f });

	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))MenuCount--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))MenuCount++;

	if (isColorReverse_)speedColor -= 0.02f;
	else speedColor += 0.02f;

	if (speedColor >= 0.9f)
	{
		isColorReverse_ = true;
	}
	if (speedColor <= 0.0f)
	{
		isColorReverse_ = false;
	}

	if (MenuCount == GOSMI_Continue)
	{
		spriteGameOver_->SetColor({ speedColor + 0.1f,speedColor + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else if (MenuCount == GOSMI_StageSelect)
	{
		spriteGameOver_->SetColor({ speedColor + 0.1f,speedColor + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
		spriteTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else if (MenuCount == GOSMI_Title)
	{
		spriteGameOver_->SetColor({ speedColor + 0.1f,speedColor + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTitle_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (MenuCount == GOSMI_Continue)
		{
			//コンティニュー
			camera_->Reset();
			sceneManager_->ChangeScene("GAMEPLAY", stageNum);
		}
		else if (MenuCount == GOSMI_StageSelect)
		{
			//ステージセレクトへ
			if (stageNum <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
			else if (stageNum <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
			else if (stageNum <= SL_StageTutorial_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);
		}
		else if (MenuCount == GOSMI_Title)
		{
			//タイトルへ
			camera_->Reset();
			sceneManager_->ChangeScene("TITLE", stageNum);

		}
	}

	rot.y += 0.1f;

	objStage->SetRotation(rot);

	imguiManager_->Begin();
	imguiManager_->End();

	spriteGameOver_->Update();
	spriteContinue_->Update();
	spriteStageSelect_->Update();
	spriteTitle_->Update();
	spriteDone_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage->Update();
}

void GameOverScene::Draw()
{
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	objStage->Draw();

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	spCommon_->PreDraw();
	//前景スプライト
	//スプライト描画	
	spriteGameOver_->Draw();
	spriteContinue_->Draw();
	spriteStageSelect_->Draw();
	spriteTitle_->Draw();
	spriteDone_->Draw();
}

void GameOverScene::Finalize()
{
	//音声
	audio_->Finalize();
	//sprite
	delete spriteGameOver_;
	delete spriteContinue_;
	delete spriteStageSelect_;
	delete spriteTitle_;
	delete spriteDone_;
	//ステージ
	delete objStage;

	delete modelStageTutorial;
	delete modelStage1;
	delete modelStage2;

	//ライト
	delete lightGroup_;
}

void GameOverScene::LoadLVData(const std::string& stagePath)
{
}
