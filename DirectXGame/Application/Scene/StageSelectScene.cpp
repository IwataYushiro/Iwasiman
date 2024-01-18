#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "TouchableObject.h"
#include "EnumList.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;
using namespace IwasiEngine;
/*

*	StageSelectScene.cpp

*	�X�e�[�W�Z���N�g�V�[��

*/


StageSelectScene::StageSelectScene(const int count) :menuCount_(count)
{
}

void StageSelectScene::Initialize()
{
	//�C���X�^���X�擾
	dxCommon_ = DirectXCommon::GetInstance();		//DirectX���
	spCommon_ = SpriteCommon::GetInstance();		//�X�v���C�g���
	input_ = Input::GetInstance();					//���͏��
	audio_ = Audio::GetInstance();					//����
	sceneManager_ = SceneManager::GetInstance();	//�V�[���}�l�[�W���[
	imguiManager_ = ImGuiManager::GetInstance();	//ImGui�}�l�[�W���[

	//�J����������
	camera_ = std::make_unique<Camera>();
	//�I�[�f�B�I
	audio_->Initialize();

	// ���_���W
	camera_->SetEye({ easeEyeStageSelect_[XYZ_X].start, easeEyeStageSelect_[XYZ_Y].start, easeEyeStageSelect_[XYZ_Z].start });
	// �����_���W
	camera_->SetTarget({ easeTargetStageSelect_[XYZ_X].start, easeTargetStageSelect_[XYZ_Y].start, easeTargetStageSelect_[XYZ_Z].start });

	//���x���f�[�^�ǂݍ���
	LoadLVData("scene/stageselect");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);

	//���j���[��ʃX�v���C�g
	spCommon_->LoadTexture(SSSTI_MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, SSSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[SSMEN_Menu].start,menuPosY_[SSMEN_Menu] });

	//�`���[�g���A���X�e�[�W�X�v���C�g
	spCommon_->LoadTexture(SSSTI_MenuTutorialTex, "texture/stagetraining.png");
	spriteTutorial_->Initialize(spCommon_, SSSTI_MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX_[SSMEN_Tutorial].start,easeStartStagePosY_[SSSMI_StageTutorial_Tutorial].start });

	//�X�e�[�W1�X�v���C�g
	spCommon_->LoadTexture(SSSTI_Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, SSSTI_Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX_[SSMEN_Stage1_Sky].start,easeStartStagePosY_[SSSMI_Stage1_SkyStage].start });

	//�X�e�[�W2�X�v���C�g
	spCommon_->LoadTexture(SSSTI_Menustage2Tex, "texture/stageboss.png");
	spriteStage2_->Initialize(spCommon_, SSSTI_Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX_[SSMEN_Stage2_Space].start,easeStartStagePosY_[SSSMI_Stage2_SpaceStage].start });

	//����\���X�v���C�g
	spCommon_->LoadTexture(SSSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, SSSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[SSMEN_SelectSpace].start,menuPosY_[SSMEN_SelectSpace] });

	//�^�C�g���ɖ߂�X�v���C�g
	spCommon_->LoadTexture(SSSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, SSSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].start,menuPosY_[SSMEN_Quit] });
	const XMFLOAT4 backColor = { 0.0f,0.0f,0.1f,1.0f };
	spriteBack_->SetColor(backColor);

	//�t�F�[�h�C���A�E�g�X�v���C�g
	spCommon_->LoadTexture(SSSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SSSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({
		black_.x,black_.y,black_.z,easeFadeInOut_.start });

	//���[�h�X�v���C�g
	spCommon_->LoadTexture(SSSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, SSSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.end });//������

	//���݃X�e�[�W�X�v���C�g
	spCommon_->LoadTexture(SSSTI_StageInfoNowTex, "texture/stage1.png");
	spriteStageInfoNow_->Initialize(spCommon_, SSSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//�J�[�\���X�v���C�g	
	spCommon_->LoadTexture(SSSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, SSSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[SSMEN_Tutorial] });

	//�X�e�[�W���X�v���C�g
	spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/1-1.png");//�����͂����܂Œu������
	spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
	
	//���j���[����X�v���C�g
	spCommon_->LoadTexture(SSSTI_MenuUITex, "texture/menuui.png");
	spriteMenuUI_->Initialize(spCommon_, SSSTI_MenuUITex);
	spriteMenuUI_->SetPosition({ easeMenuPosX_[SSMEN_UI].start,menuPosY_[SSMEN_UI] });

	//�w�i���f��
	// ���f����ǂݍ���œo�^
	ModelMapping(modelStageTutorial_, "skydomet");			//�`���[�g���A���X�e�[�W���f��(�V��)
	ModelMapping(modelStage1_, "skydome");					//�X�e�[�W1���f��(�V��)
	ModelMapping(modelStage2_, "skydome2");					//�X�e�[�W2���f��(�V��)
	

	objStage_ = Object3d::Create();
	objStage_->SetModel(modelStageTutorial_.get());
	objStage_->SetCamera(camera_.get());
	const XMFLOAT3 stageScale = { 7.0f,7.0f,7.0f };//�X�e�[�W�V���̑傫��
	objStage_->SetScale(stageScale);

	//�p�[�e�B�N��
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	particleGoal_ = Particle::LoadFromParticleTexture("particle1.png");

	//�u�[�X�g�p
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());
	pmFire_->SetCamera(camera_.get());
	//�S�[���I�u�W�F�N�g�p
	pmGoal_ = ParticleManager::Create();
	pmGoal_->SetParticleModel(particleGoal_.get());
	pmGoal_->SetCamera(camera_.get());


	//�C�[�W���O�X�^���o�C
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

	if (isStageSelect_)UpdateIsStageSelect();		//�X�e�[�W�Z���N�g���j���[��
	else if (isDone_)UpdateIsDone();				//�X�e�[�W���莞
	else if (isStart_) UpdateIsGameStart();			//�Q�[���J�n��
	else UpdateIsQuitTitle();						//�^�C�g���J�ڎ�

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		const XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//�I�t�Z�b�g
		//���v���Z�b�g
		const ParticleManager::Preset fire =
		{
			particleFire_.get(),
			{player->GetPosition().x + dashOffsetXY.x,player->GetPosition().y + dashOffsetXY.y,player->GetPosition().z},
			{ 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			2,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{0.0f,0.0f,0.0f,1.0f}
		};
		//�p�[�e�B�N��
		pmFire_->ActiveX(fire);
		//�ۉe
		SetUpCircleShadow(player->GetPosition());
		//�X�V
		player->Update();
	}
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Update();	//�n�ʂ͍X�V����
	for (std::unique_ptr<Object3d>& goal : objGoals_)
	{
		//�p�[�e�B�N���v���Z�b�g
		const ParticleManager::Preset goalEffect =
		{
			particleGoal_.get(),
			goal->GetPosition(),
			{ 20.0f,20.0f,20.0f } ,
			{ 0.1f,4.0f,0.1f },
			{ 0.0f,0.001f,0.0f },
			1,
			{3.0f, 0.0f },
			{MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),1.0f},
			{MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),1.0f}
		};
		//�S�[���̈ʒu��m�点��p�[�e�B�N��
		pmGoal_->ActiveY(goalEffect);

		//�S�[���͏펞����Ă���
		DirectX::XMFLOAT3 rot = goal->GetRotation();
		const float rotSpeedY = 1.0f;
		rot.y += rotSpeedY;
		goal->SetRotation(rot);
		//�X�V
		goal->Update();
	}

	//�V�����펞��]
	const float rotSpeed = 0.5f;
	rot_.y += rotSpeed;
	objStage_->SetRotation(rot_);

	//�X�v���C�g�X�V
	spriteMenu_->Update();				//���j���[��ʃX�v���C�g
	spriteTutorial_->Update();			//�`���[�g���A���X�e�[�W�X�v���C�g
	spriteStage1_->Update();			//�X�e�[�W1�X�v���C�g
	spriteStage2_->Update();			//�X�e�[�W2�X�v���C�g
	spriteDone_->Update();				//����\���X�v���C�g
	spriteBack_->Update();				//�^�C�g���ɖ߂�X�v���C�g
	spriteFadeInOut_->Update();			//�t�F�[�h�C���A�E�g�X�v���C�g
	spriteLoad_->Update();				//���[�h�X�v���C�g
	spriteStageInfoNow_->Update();		//���݃X�e�[�W�X�v���C�g
	spriteCursor_->Update();			//�J�[�\���X�v���C�g
	spriteStageName_->Update();			//�X�e�[�W���X�v���C�g
	spriteMenuUI_->Update();			//���j���[������@�X�v���C�g

	//�X�V
	camera_->Update();					//�J����
	lightGroup_->Update();				//���C�g
	objStage_->Update();				//�w�i�I�u�W�F�N�g
	pmFire_->Update();					//�p�[�e�B�N���}�l�[�W���[(�W�F�b�g)
	pmGoal_->Update();					//�p�[�e�B�N���}�l�[�W���[(�S�[���I�u�W�F�N�g)

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
	spriteMenuUI_->SetPosition({ easeMenuPosX_[SSMEN_UI].num_X,menuPosY_[SSMEN_UI] });
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });

	//�J���[�Z�b�g
	spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });	//�����x�����ς���

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeStageSelect_[XYZ_X].num_X, easeEyeStageSelect_[XYZ_Y].num_X, easeEyeStageSelect_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageSelect_[XYZ_X].num_X, easeTargetStageSelect_[XYZ_Y].num_X, easeTargetStageSelect_[XYZ_Z].num_X });

	//���j���[�I��
	if (input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_S))menuCount_++;

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

	//�F��ς���(�I�����Ă��郁�j���[�͋���)
	if (menuCount_ == SSSMI_StageTutorial_Tutorial)//�`���[�g���A����I����
	{
		objStage_->SetModel(modelStageTutorial_.get());
		spriteMenu_->SetColor(selectMenuColor);
		spriteTutorial_->SetColor(selectMenuColor);
		spriteStage1_->SetColor(otherMenuColor);
		spriteStage2_->SetColor(otherMenuColor);
		spriteBack_->SetColor(quitColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Tutorial] });
	}
	else if (menuCount_ == SSSMI_Stage1_SkyStage)//��X�e�[�W��I����
	{
		objStage_->SetModel(modelStage1_.get());
		spriteMenu_->SetColor(selectMenuColor);
		spriteTutorial_->SetColor(otherMenuColor);
		spriteStage1_->SetColor(selectMenuColor);
		spriteStage2_->SetColor(otherMenuColor);
		spriteBack_->SetColor(quitColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Stage1_Sky] });
	}
	else if (menuCount_ == SSSMI_Stage2_SpaceStage)//�F���X�e�[�W��I����
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
			//�C�[�W���O�X�^���o�C
			for (int i = 0; i < SSMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeEyeDoneMenu_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetDoneMenu_[i].Standby(false);

			for (int i = 0; i < XYZ_Num; i++)easeStartStagePosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeStartStagePosY_[i].Standby(false);
			easeCursorPosX_.Standby(true);

			//�����ŃX�e�[�W���̃X�v���C�g�����[�h���A���W�A�A���J�[�|�C���g���Z�b�g����
			if (menuCount_ == SSSMI_StageTutorial_Tutorial)//�`���[�g���A����I����
			{
				spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/1-1.png");
				spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
				spriteStageName_->SetPosition(stageNamePos_);
				spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
				spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
			}
			else if (menuCount_ == SSSMI_Stage1_SkyStage)//��X�e�[�W��I����
			{
				spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/2-1.png");
				spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
				spriteStageName_->SetPosition(stageNamePos_);
				spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
				spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

			}
			else if (menuCount_ == SSSMI_Stage2_SpaceStage)//�F���X�e�[�W��I����
			{
				//���݃X�e�[�W�X�v���C�g
				spCommon_->LoadTexture(SSSTI_StageInfoNowTex, "texture/stagef.png");
				spriteStageInfoNow_->Initialize(spCommon_, SSSTI_StageInfoNowTex);
				spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
				spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
				
				spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/2-4.png");
				spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
				spriteStageName_->SetPosition(stageNamePos_);
				spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
				spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
			}

			isDone_ = true;
			isStageSelect_ = false;
		}
		//���j���[�̃C�[�W���O���I������瑀����󂯕t����
		if (easeMenuPosX_[SSMEN_Quit].num_X == easeMenuPosX_[SSMEN_Quit].end)
		{
			if (input_->TriggerKey(DIK_ESCAPE))//�^�C�g���֖߂�
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
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[SSMEN_UI].num_X,menuPosY_[SSMEN_UI] });
	spriteBack_->SetPosition({ easeMenuEndPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//�X�e�[�W���Ƃɍ��W���Ⴄ
	if (menuCount_ == SSSMI_StageTutorial_Tutorial)
	{
		spriteTutorial_->SetPosition({ easeStartStagePosX_[SSSMI_StageTutorial_Tutorial].num_X,easeStartStagePosY_[SSSMI_StageTutorial_Tutorial].num_X });
		spriteStage1_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
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
	//�X�y�[�X�������ƃX�L�b�v
	SkipDirectionOnSpace(white_);

	//���S�ɔ����Ȃ�����
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//�`���[�g���A���X�e�[�W
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);//�X�e�[�W1
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_AreaBoss);//�X�e�[�W2�͂܂�������

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
		if (player->GetPosition().x >= gameStartPosX_)FadeIn(white_);//�Q�[���v���C�J�ڎ��͔�������
		else SkipDirectionOnSpace(white_);//�X�y�[�X�������ƃX�L�b�v
	}

	//���S�ɔ����Ȃ�����
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//�`���[�g���A���X�e�[�W
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);//�X�e�[�W1
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_AreaBoss);//�X�e�[�W2�͂܂�������

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
	spriteMenuUI_->SetPosition({ easeMenuPosX_[SSMEN_UI].num_X,menuPosY_[SSMEN_UI] });
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//���j���[�W�L�̃C�[�W���O���I�������t�F�[�h�A�E�g
	if (spriteMenu_->GetPosition().x == easeMenuPosX_[SSMEN_Menu].start) FadeIn(black_);//��������
	else SkipDirectionOnSpace(black_);//�X�y�[�X�������ƃX�L�b�v

	//�v���C���[���W���Z�b�g
	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		player->SetPosition({ easePlayerQuitMove_[XYZ_X].num_X,easePlayerQuitMove_[XYZ_Y].num_X ,easePlayerQuitMove_[XYZ_Z].num_X });
	}
	//���S�ɍ����Ȃ�����
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//�^�C�g���֑J��(�w�i�ɂ���đJ�ڎ��̃^�C�g���V�[�����ς��)
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("TITLE", SL_StageTutorial_StageID);
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("TITLE", SL_Stage1_StageID);
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("TITLE", SL_Stage2_StageID);
	}
	
}

void StageSelectScene::Draw()
{

	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();	//�v���C���[
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Draw();	//�n��
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();			//�S�[��
	objStage_->Draw();														//�V��(�w�i)

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	pmFire_->Draw();
	pmGoal_->Draw();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();


	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	//�X�v���C�g�`��	
	spriteMenu_->Draw();					//���j���[��ʃX�v���C�g
	spriteTutorial_->Draw();				//�`���[�g���A���X�e�[�W�X�v���C�g
	spriteStage1_->Draw();					//�X�e�[�W1�X�v���C�g
	spriteStage2_->Draw();					//�X�e�[�W2�X�v���C�g
	spriteDone_->Draw();					//����\���X�v���C�g
	spriteBack_->Draw();					//�^�C�g���ɖ߂�X�v���C�g
	spriteFadeInOut_->Draw();				//�t�F�[�h�C���A�E�g�X�v���C�g
	spriteLoad_->Draw();					//���[�h�X�v���C�g
	spriteStageInfoNow_->Draw();			//���݃X�e�[�W�X�v���C�g
	spriteCursor_->Draw();					//�J�[�\���X�v���C�g
	spriteStageName_->Draw();				//�X�e�[�W���X�v���C�g
	spriteMenuUI_->Draw();					//���j���[������@�X�v���C�g

}

void StageSelectScene::Finalize()
{
	//����
	audio_->Finalize();
}

void StageSelectScene::FadeIn(const DirectX::XMFLOAT3& color)
{
	if (!isFadeIn_)
	{
		//�����ŃX�^���o�C
		easeFadeInOut_.Standby(true);
		isFadeIn_ = true;
	}
	else
	{
		//�����Ńt�F�[�h�C���A�E�g
		const DirectX::XMFLOAT3 negapozi = { 1.0f - color.x,1.0f - color.y,1.0f - color.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOut_.num_X });//�����x�����ς���
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�l�K�|�W�̉��p
		if (isStart_ || isDone_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
		}
	}
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);
	
	// ���f����ǂݍ���œo�^
	ModelMapping(modelPlayer_, "playerdash", true);		//���@���f��
	ModelMapping(modelGround_, "ground");				//�����f��
	ModelMapping(modelGoal_, "sphere");					//�S�[�����f��

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

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
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

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
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

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
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
		//���]������
		selectColor_.x -= speedColor;
		selectColor_.y -= speedColor;
		selectColor_.z -= speedColor;
	}

	else
	{
		//�F�͎����I�ɑ���
		selectColor_.x += speedColor;
		selectColor_.y += speedColor;
		selectColor_.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.7f,0.0f };//{max,min}

	if (selectColor_.x >= maxAndMinSpeedColor.x)//Max�l�ɓ��B�����甽�]
	{
		isColorReverse_ = true;
	}
	if (selectColor_.x <= maxAndMinSpeedColor.y)//Min�l�ɓ��B�����甽�]��߂�
	{
		isColorReverse_ = false;
	}
}

void StageSelectScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
{
	const DirectX::XMVECTOR dir = { 0.0f,1.0f,0.0f,0.0f };			//���e����
	const DirectX::XMFLOAT3 casterPosOffset = { -0.5f,0.0f,0.0f };	//�L���X�^�[���W�̃I�t�Z�b�g
	//�L���X�^�[���W
	const DirectX::XMFLOAT3 casterPos =
	{
		pos.x + casterPosOffset.x,
		pos.y + casterPosOffset.y,
		pos.z + casterPosOffset.z
	};
	const DirectX::XMFLOAT3 atten = { 0.5f,0.6f,0.0f };				//���������W��
	const DirectX::XMFLOAT2 factorAngle = { 0.2f,0.5f };			//�p�x�����W��

	//�V���h�E�̃Z�b�g
	lightGroup_->SetCircleShadowDir(LightGroup::LN_0, dir);
	lightGroup_->SetCircleShadowCasterPos(LightGroup::LN_0, casterPos);
	lightGroup_->SetCircleShadowAtten(LightGroup::LN_0, atten);
	lightGroup_->SetCircleShadowFactorAngleCos(LightGroup::LN_0, factorAngle);
}
