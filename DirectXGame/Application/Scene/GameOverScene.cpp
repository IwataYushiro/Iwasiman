#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "EnumList.h"

using namespace DirectX;

/*

*	GameOverScene.cpp

*	�Q�[���I�[�o�[�V�[��

*/

//�ÓI�����o�ϐ��̎���
DirectXCommon* GameOverScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();
Audio* GameOverScene::audio_ = Audio::GetInstance();
SceneManager* GameOverScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameOverScene::imguiManager_ = ImGuiManager::GetInstance();

GameOverScene::GameOverScene(int stagenum) :stageNum_(stagenum) {}

void GameOverScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();

	//�J����������
	camera_ = new Camera();
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

	spCommon_->LoadTexture(GOSTI_MenuTex, "texture/gameover2.png");
	spriteGameOver_->Initialize(spCommon_, GOSTI_MenuTex);
	spriteGameOver_->SetPosition({ easeMenuPosX_[0].start,0.0f });

	spCommon_->LoadTexture(GOSTI_ContinueTex, "texture/continue.png");
	spriteContinue_->Initialize(spCommon_, GOSTI_ContinueTex);
	spriteContinue_->SetPosition({ easeMenuPosX_[1].start,150.0f });

	spCommon_->LoadTexture(GOSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, GOSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].start,300.0f });

	spCommon_->LoadTexture(GOSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, GOSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[3].start,450.0f });

	spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, GOSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[4].start,580.0f });

	modelStageTutorial_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");

	objStage_ = Object3d::Create();
	objStage_->SetModel(modelStage2_);
	objStage_->SetCamera(camera_);
	objStage_->SetScale({ 7.0f,7.0f,7.0f });


	for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(false);
}

void GameOverScene::Update()
{
	if (menuCount_ <= GOSMI_Continue)menuCount_ = GOSMI_Continue;
	else if (menuCount_ >= GOSMI_Title)menuCount_ = GOSMI_Title;

	//�C�[�W���O
	for (int i = 0; i < 4; i++)easeMenuPosX_[i].ease_out_expo();

	//���W�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteContinue_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteTitle_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });

	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	if (isColorReverse_)speedColor_ -= 0.02f;
	else speedColor_ += 0.02f;

	if (speedColor_ >= 0.9f)
	{
		isColorReverse_ = true;
	}
	if (speedColor_ <= 0.0f)
	{
		isColorReverse_ = false;
	}

	if (menuCount_ == GOSMI_Continue)
	{
		spriteGameOver_->SetColor({ speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else if (menuCount_ == GOSMI_StageSelect)
	{
		spriteGameOver_->SetColor({ speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f });
		spriteTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else if (menuCount_ == GOSMI_Title)
	{
		spriteGameOver_->SetColor({ speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f,1.0f });
		spriteContinue_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStageSelect_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTitle_->SetColor({ 1.0f,speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f });
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (menuCount_ == GOSMI_Continue)
		{
			//�R���e�B�j���[
			camera_->Reset();
			sceneManager_->ChangeScene("GAMEPLAY", stageNum_);
		}
		else if (menuCount_ == GOSMI_StageSelect)
		{
			//�X�e�[�W�Z���N�g��
			if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
			else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
			else if (stageNum_ <= SL_StageTutorial_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);
		}
		else if (menuCount_ == GOSMI_Title)
		{
			//�^�C�g����
			camera_->Reset();
			sceneManager_->ChangeScene("TITLE", stageNum_);

		}
	}

	rot_.y += 0.1f;

	objStage_->SetRotation(rot_);

	imguiManager_->Begin();
	imguiManager_->End();

	spriteGameOver_->Update();
	spriteContinue_->Update();
	spriteStageSelect_->Update();
	spriteTitle_->Update();
	spriteDone_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage_->Update();
}

void GameOverScene::Draw()
{
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	objStage_->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	//�X�v���C�g�`��	
	spriteGameOver_->Draw();
	spriteContinue_->Draw();
	spriteStageSelect_->Draw();
	spriteTitle_->Draw();
	spriteDone_->Draw();
}

void GameOverScene::Finalize()
{
	//����
	audio_->Finalize();
	//sprite
	delete spriteGameOver_;
	delete spriteContinue_;
	delete spriteStageSelect_;
	delete spriteTitle_;
	delete spriteDone_;
	//�X�e�[�W
	delete objStage_;

	delete modelStageTutorial_;
	delete modelStage1_;
	delete modelStage2_;

	//�J����
	delete camera_;
	//���C�g
	delete lightGroup_;
}

void GameOverScene::LoadLVData([[maybe_unused]] const std::string& stagePath)
{
}
