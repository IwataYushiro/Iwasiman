#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include "EnumList.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

/*

*	StageSelectScene.cpp

*	�X�e�[�W�Z���N�g�V�[��

*/

//�ÓI�����o�ϐ��̎���
DirectXCommon* StageSelectScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageSelectScene::input_ = Input::GetInstance();
Audio* StageSelectScene::audio_ = Audio::GetInstance();
SceneManager* StageSelectScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageSelectScene::imguiManager_ = ImGuiManager::GetInstance();

StageSelectScene::StageSelectScene(const int count) :menuCount_(count)
{
}

void StageSelectScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();

	//�J����������
	camera_ = std::make_unique<Camera>();
	//�I�[�f�B�I
	audio_->Initialize();

	//�J����
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//camera_->SetTarget({ 90.0f,0.0f,0.0f });
	//camera_->SetEye({ -10.0f,2.0f,0.0f });

	// ���_���W
	camera_->SetEye({ easeEyeStageSelect_[XYZ_X].start, easeEyeStageSelect_[XYZ_Y].start, easeEyeStageSelect_[XYZ_Z].start });
	// �����_���W
	camera_->SetTarget({ easeTargetStageSelect_[XYZ_X].start, easeTargetStageSelect_[XYZ_Y].start, easeTargetStageSelect_[XYZ_Z].start });

	//���x���f�[�^�ǂݍ���
	LoadLVData("scene/stageselect");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());

	spCommon_->LoadTexture(SSSTI_MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, SSSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[SSMEN_Menu].start,menuPosY_[SSMEN_Menu]});

	spCommon_->LoadTexture(SSSTI_MenuTutorialTex, "texture/titlemenut.png");
	spriteTutorial_->Initialize(spCommon_, SSSTI_MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX_[SSMEN_Tutorial].start,easeStartStagePosY_[SSSMI_StageTutorial_Tutorial].start });

	spCommon_->LoadTexture(SSSTI_Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, SSSTI_Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX_[SSMEN_Stage1_Sky].start,easeStartStagePosY_[SSSMI_Stage1_SkyStage].start });

	spCommon_->LoadTexture(SSSTI_Menustage2Tex, "texture/stagespace.png");
	spriteStage2_->Initialize(spCommon_, SSSTI_Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX_[SSMEN_Stage2_Space].start,easeStartStagePosY_[SSSMI_Stage2_SpaceStage].start });

	spCommon_->LoadTexture(SSSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, SSSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[SSMEN_SelectSpace].start,menuPosY_[SSMEN_SelectSpace] });

	spCommon_->LoadTexture(SSSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, SSSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].start,menuPosY_[SSMEN_Quit] });
	const XMFLOAT4 backColor = { 0.0f,0.0f,0.1f,1.0f };
	spriteBack_->SetColor(backColor);

	spCommon_->LoadTexture(SSSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SSSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ 
		black_.x,black_.y,black_.z,easeFadeInOut_.start });

	spCommon_->LoadTexture(SSSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, SSSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.end });//������

	spCommon_->LoadTexture(SSSTI_StageInfoNowTex, "texture/stage1.png");
	spriteStageInfoNow_->Initialize(spCommon_, SSSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
	
	spCommon_->LoadTexture(SSSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, SSSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[SSMEN_Tutorial] });

	modelStageTutorial_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");

	objStage_ = Object3d::Create();
	objStage_->SetModel(modelStageTutorial_.get());
	objStage_->SetCamera(camera_.get());
	const XMFLOAT3 stageScale = { 7.0f,7.0f,7.0f };
	objStage_->SetScale(stageScale);

	particle1_ = Particle::LoadFromParticleTexture("particle8.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_.get());
	pm1_->SetCamera(camera_.get());

	//�C�[�W���OStandby
	for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageSelect_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageSelect_[i].Standby(false);
	easeFadeInOut_.Standby(false);
	easeCursorPosX_.Standby(false);
}

void StageSelectScene::Update()
{
	if (menuCount_ <= SSSMI_StageTutorial_Tutorial)menuCount_ = SSSMI_StageTutorial_Tutorial;
	else if (menuCount_ >= SSSMI_Stage2_SpaceStage)menuCount_ = SSSMI_Stage2_SpaceStage;

	if (isStageSelect_)UpdateIsStageSelect();
	else if (isDone_)UpdateIsDone();
	else if (isStart_) UpdateIsGameStart();
	else UpdateIsQuitTitle();

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		const XMFLOAT2 dashOffset = { -1.0f,1.0f };//�I�t�Z�b�g
		//�p�[�e�B�N��
		const ParticleManager::Preset smoke =
		{
			particle1_.get(),
			{player->GetPosition().x + dashOffset.x,player->GetPosition().y + dashOffset.y,player->GetPosition().z},
			{ 0.0f ,3.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			2,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{0.0f,0.0f,0.0f,1.0f}
		};

		pm1_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		player->Update();
	}
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Update();
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Update();

	const float rotSpeed = 0.5f;
	rot_.y += rotSpeed;

	objStage_->SetRotation(rot_);

	spriteMenu_->Update();
	spriteTutorial_->Update();
	spriteStage1_->Update();
	spriteStage2_->Update();
	spriteDone_->Update();
	spriteBack_->Update();
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();
	spriteCursor_->Update();

	pm1_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage_->Update();

	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(false);
#endif // DEBUG

	imguiManager_->End();
}

void StageSelectScene::UpdateIsStageSelect()
{
	//�C�[�W���O
	for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageSelect_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageSelect_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuPosX_[SSMEN_Menu].num_X,menuPosY_[SSMEN_Menu] });
	spriteTutorial_->SetPosition({ easeMenuPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
	spriteStage1_->SetPosition({ easeMenuPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
	spriteStage2_->SetPosition({ easeMenuPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });
	spriteDone_->SetPosition({ easeMenuPosX_[SSMEN_SelectSpace].num_X,menuPosY_[SSMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });

	//�J���[�Z�b�g
	spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });	//�����x�����ς���

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeStageSelect_[XYZ_X].num_X, easeEyeStageSelect_[XYZ_Y].num_X, easeEyeStageSelect_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageSelect_[XYZ_X].num_X, easeTargetStageSelect_[XYZ_Y].num_X, easeTargetStageSelect_[XYZ_Z].num_X });

	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;
	
	//�I�𒆂̃��j���[�J���[
	const DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,0.7f };
	//�I�𒆂̃��j���[�J���[(�Â��w�i��)
	const DirectX::XMFLOAT4 selectMenuColorDark = { 1.0f,selectColor_.y + 0.1f,selectColor_.z + 0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColorDark = { 1.0f,1.0f,1.0f,0.7f };
	//����w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };
	//�^�C�g���֖߂�w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 quitColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	const DirectX::XMFLOAT4 quitColorDark = { selectColor_.x + 0.1f,selectColor_.y + 0.1f,1.0f,1.0f };
	//�F�ւ�
	UpdateChangeColor();

	if (menuCount_ == SSSMI_StageTutorial_Tutorial)
	{
		objStage_->SetModel(modelStageTutorial_.get());
		spriteMenu_->SetColor(selectMenuColor);
		spriteTutorial_->SetColor(selectMenuColor);
		spriteStage1_->SetColor(otherMenuColor);
		spriteStage2_->SetColor(otherMenuColor);
		spriteBack_->SetColor(quitColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Tutorial] });
	}
	else if (menuCount_ == SSSMI_Stage1_SkyStage)
	{
		objStage_->SetModel(modelStage1_.get());
		spriteMenu_->SetColor(selectMenuColor);
		spriteTutorial_->SetColor(otherMenuColor);
		spriteStage1_->SetColor(selectMenuColor);
		spriteStage2_->SetColor(otherMenuColor);
		spriteBack_->SetColor(quitColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Stage1_Sky] });
	}
	else if (menuCount_ == SSSMI_Stage2_SpaceStage)
	{
		objStage_->SetModel(modelStage2_.get());
		spriteMenu_->SetColor(selectMenuColorDark);
		spriteTutorial_->SetColor(otherMenuColorDark);
		spriteStage1_->SetColor(otherMenuColorDark);
		spriteStage2_->SetColor(selectMenuColorDark);
		spriteBack_->SetColor(quitColorDark);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Stage2_Space] });
	}
	//���ʃJ���[
	spriteDone_->SetColor(doneColor);
	if (spriteDone_->GetPosition().x == easeMenuPosX_[SSMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			for (int i = 0; i < SSMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeEyeDoneMenu_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetDoneMenu_[i].Standby(false);

			for (int i = 0; i < XYZ_Num; i++)easeStartStagePosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeStartStagePosY_[i].Standby(false);
			easeCursorPosX_.Standby(true);

			isDone_ = true;
			isStageSelect_ = false;
		}
		if (easeMenuPosX_[SSMEN_Quit].num_X == easeMenuPosX_[SSMEN_Quit].end)
		{
			if (input_->TriggerKey(DIK_Q))
			{
				for (int i = 0; i < XYZ_Num; i++)easePlayerQuitMove_[i].Standby(false);
				for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].Standby(true);
				easeCursorPosX_.Standby(true);

				outStageSelect_ = true;
				isStageSelect_ = false;
			}
		}
	}
}

void StageSelectScene::UpdateIsDone()
{
	//�C�[�W���O
	for (int i = 0; i < SSMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeDoneMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetDoneMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeStartStagePosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeStartStagePosY_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuEndPosX_[SSMEN_Menu].num_X,menuPosY_[SSMEN_Menu] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SSMEN_SelectSpace].num_X,menuPosY_[SSMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuEndPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//�X�e�[�W���Ƃɍ��W���Ⴄ
	if (menuCount_ == SSSMI_StageTutorial_Tutorial)
	{
		spriteTutorial_->SetPosition({ easeStartStagePosX_[SSSMI_StageTutorial_Tutorial].num_X,easeStartStagePosY_[SSSMI_StageTutorial_Tutorial].num_X });
		spriteStage1_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky]});
		spriteStage2_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });
	}
	else if (menuCount_ == SSSMI_Stage1_SkyStage)
	{
		spriteTutorial_->SetPosition({ easeMenuEndPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
		spriteStage1_->SetPosition({ easeStartStagePosX_[SSSMI_Stage1_SkyStage].num_X,easeStartStagePosY_[SSSMI_Stage1_SkyStage].num_X });
		spriteStage2_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });

	}
	else if (menuCount_ == SSSMI_Stage2_SpaceStage)
	{
		spriteTutorial_->SetPosition({ easeMenuEndPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
		spriteStage1_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
		spriteStage2_->SetPosition({ easeStartStagePosX_[SSSMI_Stage2_SpaceStage].num_X,easeStartStagePosY_[SSSMI_Stage2_SpaceStage].num_X });
	}

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeDoneMenu_[XYZ_X].num_X, easeEyeDoneMenu_[XYZ_Y].num_X, easeEyeDoneMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetDoneMenu_[XYZ_X].num_X, easeTargetDoneMenu_[XYZ_Y].num_X, easeTargetDoneMenu_[XYZ_Z].num_X });

	//�C�[�W���O���I�������
	if (camera_->GetEye().x == easeEyeDoneMenu_[XYZ_X].end)
	{
		for (int i = 0; i < XYZ_Num; i++)easePlayerStartMove_[i].Standby(false);
		for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
		for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
		isStart_ = true;
		isDone_ = false;
	}
}

void StageSelectScene::UpdateIsGameStart()
{
	//�v���C���[��X�l�������܂ŗ�����t�F�[�h�A�E�g
	const float gameStartPosX_ = 200.0f;
	//�C�[�W���O
	for (int i = 0; i < XYZ_Num; i++)easePlayerStartMove_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_quint();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_quint();

	//�J�����Z�b�g
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });

	//�v���C���[���W���Z�b�g
	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		player->SetPosition({ easePlayerStartMove_[XYZ_X].num_X,easePlayerStartMove_[XYZ_Y].num_X ,easePlayerStartMove_[XYZ_Z].num_X });
		if (player->GetPosition().x >= gameStartPosX_)FadeOut(white_);//�Q�[���v���C�J�ڎ��͔�������
	}
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//�`���[�g���A���X�e�[�W
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);//�X�e�[�W1
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area3);//�X�e�[�W2�͂܂�������

	}
}

void StageSelectScene::UpdateIsQuitTitle()
{
	//�C�[�W���O
	for (int i = 0; i < XYZ_Num; i++)easePlayerQuitMove_[i].ease_in_expo();
	for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuPosX_[SSMEN_Menu].num_X,menuPosY_[SSMEN_Menu] });
	spriteTutorial_->SetPosition({ easeMenuPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
	spriteStage1_->SetPosition({ easeMenuPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
	spriteStage2_->SetPosition({ easeMenuPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });
	spriteDone_->SetPosition({ easeMenuPosX_[SSMEN_SelectSpace].num_X,menuPosY_[SSMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//���j���[�W�L�̃C�[�W���O���I�������t�F�[�h�A�E�g
	if (spriteMenu_->GetPosition().x == easeMenuPosX_[SSMEN_Menu].start) FadeOut(black_);//��������

	//�v���C���[���W���Z�b�g
	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		player->SetPosition({ easePlayerQuitMove_[XYZ_X].num_X,easePlayerQuitMove_[XYZ_Y].num_X ,easePlayerQuitMove_[XYZ_Z].num_X });
	}
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("TITLE", SL_StageTutorial_StageID);
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("TITLE", SL_Stage1_StageID);
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("TITLE", SL_Stage2_StageID);
	}
}

void StageSelectScene::Draw()
{

	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Draw();
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();
	objStage_->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	pm1_->Draw();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();


	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	//�X�v���C�g�`��	
	spriteMenu_->Draw();
	spriteTutorial_->Draw();
	spriteStage1_->Draw();
	spriteStage2_->Draw();
	spriteDone_->Draw();
	spriteBack_->Draw();
	spriteFadeInOut_->Draw();
	spriteLoad_->Draw();
	spriteStageInfoNow_->Draw();
	spriteCursor_->Draw();
}

void StageSelectScene::Finalize()
{
	//����
	audio_->Finalize();
}

void StageSelectScene::FadeOut(const DirectX::XMFLOAT3& rgb)
{
	if (!isFadeOut_)
	{
		easeFadeInOut_.Standby(true);
		isFadeOut_ = true;
	}
	else
	{
		const DirectX::XMFLOAT3 negapozi = { 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//�����x�����ς���
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�l�K�|�W�̉��p
		if (isStart_)spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });

	}
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);

	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("playerdash", true);
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelGround_ = Model::LoadFromOBJ("ground");

	models_.insert(std::make_pair("playerdash", modelPlayer_.get()));
	models_.insert(std::make_pair("sphere", modelGoal_.get()));
	models_.insert(std::make_pair("ground", modelGround_.get()));

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objectData : levelData_->objects) {
		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}

		if (objectData.objectType.find("PLAYER") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			std::unique_ptr<Object3d> newObject = Object3d::Create();
			//�I�u�W�F�N�g�Ƀ��f���R�t����
			newObject->SetModel(model);

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			newObject->SetCamera(camera_.get());
			// �z��ɓo�^
			objPlayers_.push_back(std::move(newObject));
		}
		else if (objectData.objectType.find("PLANE") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			std::unique_ptr<Object3d> newObject = Object3d::Create();
			//�I�u�W�F�N�g�Ƀ��f���R�t����
			newObject->SetModel(model);

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			newObject->SetCamera(camera_.get());
			// �z��ɓo�^
			objGrounds_.push_back(std::move(newObject));
		}
		else if (objectData.objectType.find("GOAL") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			std::unique_ptr<Object3d> newObject = Object3d::Create();
			//�I�u�W�F�N�g�Ƀ��f���R�t����
			newObject->SetModel(model);

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			newObject->SetCamera(camera_.get());
			// �z��ɓo�^
			objGoals_.push_back(std::move(newObject));
		}

	}

}

void StageSelectScene::UpdateChangeColor()
{
	//�F��ς���X�s�[�h
	const float speedColor = 0.02f;

	if (isColorReverse_)
	{
		selectColor_.x -= speedColor;
		selectColor_.y -= speedColor;
		selectColor_.z -= speedColor;
	}

	else
	{
		selectColor_.x += speedColor;
		selectColor_.y += speedColor;
		selectColor_.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.7f,0.0f };//{max,min}

	if (selectColor_.x >= maxAndMinSpeedColor.x)
	{
		isColorReverse_ = true;
	}
	if (selectColor_.x <= maxAndMinSpeedColor.y)
	{
		isColorReverse_ = false;
	}
}
