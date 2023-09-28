#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include <cassert>
#include <sstream>
#include <iomanip>

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

	UINT MenuTex = 00;
	spCommon_->LoadTexture(MenuTex, "texture/gameover2.png");
	spriteGameOver_->Initialize(spCommon_, MenuTex);
	spriteGameOver_->SetPosition({ easeMenuPosX[0].start,0.0f });

	UINT MenuTutorialTex = 01;
	spCommon_->LoadTexture(MenuTutorialTex, "texture/continue.png");
	spriteContinue_->Initialize(spCommon_, MenuTutorialTex);
	spriteContinue_->SetPosition({ easeMenuPosX[1].start,150.0f });

	UINT Menustage1Tex = 02;
	spCommon_->LoadTexture(Menustage1Tex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, Menustage1Tex);
	spriteStageSelect_->SetPosition({ easeMenuPosX[2].start,300.0f });

	UINT Menustage2Tex = 03;
	spCommon_->LoadTexture(Menustage2Tex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, Menustage2Tex);
	spriteTitle_->SetPosition({ easeMenuPosX[3].start,450.0f });

	UINT MenuDoneTex = 04;
	spCommon_->LoadTexture(MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, MenuDoneTex);
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
	if (MenuCount <= 0)MenuCount = 0;
	else if (MenuCount >= 2)MenuCount = 2;

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

	if (isColorReverse_)speedColor -= 0.01f;
	else speedColor += 0.01f;

	if (speedColor >= 0.9f)
	{
		isColorReverse_ = true;
	}
	if (speedColor <= 0.0f)
	{
		isColorReverse_ = false;
	}

	if (MenuCount == 0)
	{
		spriteGameOver_->SetColor({ speedColor + 0.1f,speedColor + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else if (MenuCount == 1)
	{
		spriteGameOver_->SetColor({ speedColor + 0.1f,speedColor + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
		spriteTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else if (MenuCount == 2)
	{
		spriteGameOver_->SetColor({ speedColor + 0.1f,speedColor + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTitle_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (MenuCount == 0)
		{
			//コンティニュー
			camera_->Reset();
			sceneManager_->ChangeScene("GAMEPLAY", stageNum);
		}
		else if (MenuCount == 1)
		{
			//ステージセレクトへ
			camera_->Reset();
			sceneManager_->ChangeScene("STAGESELECT");

		}
		else if (MenuCount == 2)
		{
			//タイトルへ
			camera_->Reset();
			sceneManager_->ChangeScene("TITLE");

		}
	}

	rot.y += 0.1f;

	objStage->SetRotation(rot);

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
