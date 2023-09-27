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
	//�I�[�f�B�I
	audio_->Initialize();

	//�J����
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//camera_->SetTarget({ 90.0f,0.0f,0.0f });
	//camera_->SetEye({ -10.0f,2.0f,0.0f });

	// ���_���W
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// �����_���W
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//���x���f�[�^�ǂݍ���
	//LoadLVData("enemytest");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT MenuTex = 00;
	spCommon_->LoadTexture(MenuTex, "texture/gameover.png");
	spriteMenu_->Initialize(spCommon_, MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });

	UINT MenuTutorialTex = 01;
	spCommon_->LoadTexture(MenuTutorialTex, "texture/titlemenut.png");
	spriteTutorial_->Initialize(spCommon_, MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX[1].start,150.0f });

	UINT Menustage1Tex = 02;
	spCommon_->LoadTexture(Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX[2].start,300.0f });

	UINT Menustage2Tex = 03;
	spCommon_->LoadTexture(Menustage2Tex, "texture/stagetower.png");
	spriteStage2_->Initialize(spCommon_, Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX[3].start,450.0f });

	UINT MenuDoneTex = 04;
	spCommon_->LoadTexture(MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX[4].start,580.0f });

	modelStageTutorial = Model::LoadFromOBJ("skydomet");
	modelStage1 = Model::LoadFromOBJ("skydome");
	modelStage2 = Model::LoadFromOBJ("skydome2");

	objStage = Object3d::Create();
	objStage->SetModel(modelStageTutorial);
	objStage->SetCamera(camera_);
	objStage->SetScale({ 7.0f,7.0f,7.0f });


	easeTitlePosX.Standby(false);
	for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(false);
}

void GameOverScene::Update()
{
	if (MenuCount <= 0)MenuCount = 0;
	else if (MenuCount >= 2)MenuCount = 2;

	//�C�[�W���O
	easeTitlePosX.ease_out_expo();
	for (int i = 0; i < 4; i++)easeMenuPosX[i].ease_out_expo();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
	spriteTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
	spriteStage1_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
	spriteStage2_->SetPosition({ easeMenuPosX[3].num_X,450.0f });
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
		objStage->SetModel(modelStageTutorial);
		spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteTutorial_->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
		spriteStage1_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
	}
	else if (MenuCount == 1)
	{
		objStage->SetModel(modelStage1);
		spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteTutorial_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteStage1_->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
		spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
	}
	else if (MenuCount == 2)
	{
		objStage->SetModel(modelStage2);
		spriteMenu_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTutorial_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStage1_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStage2_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (MenuCount == 0)
		{
			//�`���[�g���A���X�e�[�W
			camera_->Reset();
			sceneManager_->ChangeScene("GAMEPLAY", 1);
		}
		else if (MenuCount == 1)
		{
			//�X�e�[�W1
			camera_->Reset();
			sceneManager_->ChangeScene("GAMEPLAY", 3);

		}
		else if (MenuCount == 2)
		{
			//�X�e�[�W2
			camera_->Reset();
			sceneManager_->ChangeScene("GAMEPLAY", 4);

		}
	}

	rot.y += 0.5f;

	objStage->SetRotation(rot);

	spriteMenu_->Update();
	spriteTutorial_->Update();
	spriteStage1_->Update();
	spriteStage2_->Update();
	spriteDone_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage->Update();
}

void GameOverScene::Draw()
{
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	objStage->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	//�X�v���C�g�`��	
	spriteMenu_->Draw();
	spriteTutorial_->Draw();
	spriteStage1_->Draw();
	spriteStage2_->Draw();
	spriteDone_->Draw();
}

void GameOverScene::Finalize()
{
	//����
	audio_->Finalize();
	//sprite
	delete spriteMenu_;
	delete spriteTutorial_;
	delete spriteStage1_;
	delete spriteStage2_;
	delete spriteDone_;
	//�X�e�[�W
	delete objStage;

	delete modelStageTutorial;
	delete modelStage1;
	delete modelStage2;

	//���C�g
	delete lightGroup_;
}

void GameOverScene::LoadLVData(const std::string& stagePath)
{
}
