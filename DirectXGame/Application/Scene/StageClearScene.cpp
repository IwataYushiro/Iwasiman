#include "StageClearScene.h"
#include "FbxLoader.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace IwasiEngine;
using namespace MyMath;
/*

*	StageClearScene.cpp

*	�X�e�[�W�N���A�V�[��

*/

//�ÓI�����o�ϐ��̎���
DirectXCommon* StageClearScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageClearScene::input_ = Input::GetInstance();
Audio* StageClearScene::audio_ = Audio::GetInstance();
SceneManager* StageClearScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageClearScene::imguiManager_ = ImGuiManager::GetInstance();

StageClearScene::StageClearScene(const int stagenum) :stageNum_(stagenum) {}

void StageClearScene::Initialize()
{
	//�C���X�^���X�擾
	spCommon_ = SpriteCommon::GetInstance();

	//�J����
	camera_ = std::make_unique<Camera>();
	//�I�[�f�B�I
	audio_->Initialize();

	// ���_���W
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].start, easeEyeStageClear_[XYZ_Y].start, easeEyeStageClear_[XYZ_Z].start });
	// �����_���W
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].start, easeTargetStageClear_[XYZ_Y].start, easeTargetStageClear_[XYZ_Z].start });

	//���x���f�[�^�ǂݍ���
	if (stageNum_ == SL_Default)LoadLVData("scene/stagecleart");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/stageclear1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/stageclear2");
	else LoadLVData("scene/stagecleart");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);

	//�X�v���C�g
	//�X�e�[�W�N���A���̃X�v���C�g
	if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_Stage2_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else spCommon_->LoadTexture(SCSTI_MenuTex, "texture/stageclear2.png");
	spriteStageClear_->Initialize(spCommon_, SCSTI_MenuTex);
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].start,menuPosY_[SCMEN_Menu] });

	//���̃X�e�[�W�\���̃X�v���C�g
	spCommon_->LoadTexture(SCSTI_NextStageTex, "texture/nextstage.png");
	spriteNextStage_->Initialize(spCommon_, SCSTI_NextStageTex);
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].start,menuPosY_[SCMEN_NextStage] });

	//�X�e�[�W�Z���N�g�\���̃X�v���C�g
	spCommon_->LoadTexture(SCSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, SCSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].start,menuPosY_[SCMEN_StageSelect] });

	//�^�C�g���\���̃X�v���C�g
	spCommon_->LoadTexture(SCSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, SCSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].start,menuPosY_[SCMEN_Title] });

	//����\���̃X�v���C�g
	spCommon_->LoadTexture(SCSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, SCSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].start,menuPosY_[SCMEN_SelectSpace] });

	//�t�F�[�h�C���A�E�g�̃X�v���C�g
	spCommon_->LoadTexture(SCSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SCSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });

	//���[�h�X�v���C�g
	spCommon_->LoadTexture(SCSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, SCSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//���݃X�e�[�W�X�v���C�g
	const int remainderNum = stageNum_ % 10;//�]��ɂ���ăX�v���C�g��ς���
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stagef.png");
	spriteStageInfoNow_->Initialize(spCommon_, SCSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//�J�[�\���X�v���C�g
	spCommon_->LoadTexture(SCSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, SCSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[SCMEN_NextStage] });

	//�X�e�[�W���X�v���C�g
	LoadStageNameSprite();

	//�p�[�e�B�N��
	//�N���A���o�p
	particleClear_ = Particle::LoadFromParticleTexture("particle1.png");
	pmClear_ = ParticleManager::Create();
	pmClear_->SetParticleModel(particleClear_.get());
	pmClear_->SetCamera(camera_.get());
	//���@�̈ړ����o�p
	particleSmoke_ = Particle::LoadFromParticleTexture("particle8.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());
	pmSmoke_->SetCamera(camera_.get());

	//�e�X�e�[�W�̍ŏI�ʂ͎��̃X�e�[�W�֍s���\�L������
	if (stageNum_ == SL_Stage1_AreaBoss)
	{
		isFinalStage_ = true;
		menuCount_ = SCSMI_StageSelect;
	}
	else if (stageNum_ == SL_Stage2_AreaBoss)
	{
		isFinalStage_ = true;
		menuCount_ = SCSMI_StageSelect;
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		isFinalStage_ = true;
		menuCount_ = SCSMI_StageSelect;
	}

	//�C�[�W���O�X�^���o�C
	easeFadeInOut_.Standby(false);
	for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].Standby(false);
	easeCursorPosX_.Standby(false);
}

void StageClearScene::Update()
{
	if (isNextStage_)UpdateIsNextStage();				//���̃X�e�[�W�֑J�ڂ��鎞
	else if (isStageSelect_)UpdateIsStageSelect();		//�X�e�[�W�Z���N�g�J�ڎ�
	else if (isQuitTitle_)UpdateIsQuitTitle();			//�^�C�g���֑J�ڂ��鎞
	else UpdateIsMenu();								//���j���[��ʎ�

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		const XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//�I�t�Z�b�g
		//���v���Z�b�g
		const ParticleManager::Preset smoke =
		{
			particleSmoke_.get(),
			{player->GetPosition().x + dashOffsetXY.x,player->GetPosition().y + dashOffsetXY.y,player->GetPosition().z},
			{ 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			2,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		//�p�[�e�B�N��
		pmSmoke_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);
		
		//�ۉe
		SetUpCircleShadow(player->GetPosition());
		//�X�V
		player->Update();
	}

	for (std::unique_ptr<Object3d>& ground : objGrounds_)
	{
		DirectX::XMFLOAT3 move = ground->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };
		//X�l�������܂ŗ����烋�[�v
		const float returnPos_ = -120.0f;
		//�펞�i��
		move.x += speed.x;
		ground->SetPosition(move);
		if (ground->GetPosition().x <= returnPos_)ground->SetPosition(startPos_);
		//�X�V
		ground->Update();
	}
	//�V��(�w�i)
	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)
	{
		//�V����]�p
		DirectX::XMFLOAT3 rotSkydome = skydome->GetRotation();
		//�V��(�w�i)�͏펞��]����
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);
		//�X�V
		skydome->Update();
	}
	//�S�[��
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Update();//�X�V


	//�X�v���C�g�X�V
	spriteStageClear_->Update();		   //�X�e�[�W�N���A���̃X�v���C�g
	spriteNextStage_->Update();			   //���̃X�e�[�W�\���̃X�v���C�g
	spriteStageSelect_->Update();		   //�X�e�[�W�Z���N�g�\���̃X�v���C�g
	spriteTitle_->Update();				   //�^�C�g���\���̃X�v���C�g
	spriteDone_->Update();				   //����\���̃X�v���C�g
	spriteFadeInOut_->Update();			   //�t�F�[�h�C���A�E�g�̃X�v���C�g
	spriteLoad_->Update();				   //���[�h�X�v���C�g
	spriteStageInfoNow_->Update();		   //���݃X�e�[�W�X�v���C�g
	spriteCursor_->Update();			   //�J�[�\���X�v���C�g
	spriteStageName_->Update();			   //�X�e�[�W���X�v���C�g

	//�X�V
	camera_->Update();		   //�J����
	lightGroup_->Update();	   //���C�g
	pmClear_->Update();		   //�N���A���̃p�[�e�B�N��
	pmSmoke_->Update();		   //���̃p�[�e�B�N��
	//ImGui
	imguiManager_->Begin();
	imguiManager_->End();

}

void StageClearScene::UpdateIsNextStage()
{
	//�C�[�W���O
	for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//���W�Z�b�g
	spriteStageClear_->SetPosition({ easeMenuEndPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuEndPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//�J�������Z�b�g
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].num_X, easeEyeStageClear_[XYZ_Y].num_X, easeEyeStageClear_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].num_X, easeTargetStageClear_[XYZ_Y].num_X, easeTargetStageClear_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//���v���Z�b�g
		const ParticleManager::Preset clear =
		{
			particleClear_.get(),
			{player->GetPosition().x + 70.0f,player->GetPosition().y + 30.0f ,player->GetPosition().z},
			{ 0.0f ,60.0f,75.0f },
			{  RandomMTFloat(-15.0f,-8.0f),0.0f,0.0f},
			{ 0.0f,0.001f,0.0f },
			3,
			{ RandomMTFloat(4.0f,6.0f), 0.0f },
			{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f},
			{ RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f }
		};
		//�p�[�e�B�N��
		pmClear_->ActiveX(clear.particle, clear.startPos, clear.pos, clear.vel,
			clear.acc, clear.num, clear.scale, clear.startColor, clear.endColor);

	}

	for (std::unique_ptr<Object3d>& goal : objGoals_)
	{
		//�����ŃS�[���I�u�W�F�N�g������
		DirectX::XMFLOAT3 move = goal->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.5f,0.0f,0.0f };
		//X�l�������܂ŗ�����C�[�W���O
		const float gameStartPos_ = 60.0f;
		//�i��
		move.x += speed.x;
		goal->SetPosition(move);
		//�S�[���I�u�W�F�N�g�̈ʒu�����̈ʒu�ɓ���������J�ډ��o
		if (goal->GetPosition().x <= gameStartPos_)
		{
			FadeOut(white_);//�Q�[���v���C�J�ڎ��͔�������
			//���S�ɔ����Ȃ�����
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				if (stageNum_ == SL_Stage1_AreaBoss) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
				else if (stageNum_ == SL_StageTutorial_Final) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else sceneManager_->ChangeScene("GAMEPLAY", ++stageNum_);
			}
		};


	}
}

void StageClearScene::UpdateIsStageSelect()
{
	//�C�[�W���O
	for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easePlayerMoveStageSelect_[i].ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteStageClear_->SetPosition({ easeMenuEndPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuEndPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//�J�������Z�b�g
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].num_X, easeEyeStageClear_[XYZ_Y].num_X, easeEyeStageClear_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].num_X, easeTargetStageClear_[XYZ_Y].num_X, easeTargetStageClear_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//���@�̍��W���Z�b�g
		player->SetPosition({ easePlayerMoveStageSelect_[XYZ_X].num_X,easePlayerMoveStageSelect_[XYZ_Y].num_X,easePlayerMoveStageSelect_[XYZ_Z].num_X });
		//���j���[�̃C�[�W���O���I�������J�ډ��o
		if (spriteStageClear_->GetPosition().x == easeMenuEndPosX_[SCMEN_Menu].end)
		{
			FadeOut(black_);//��������
			//���S�ɍ����Ȃ�����
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//�X�e�[�W�I��
				if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
				else sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);//�����̓`���[�g���A���ɔ�΂��Ă�����
			}
		}
	}
}

void StageClearScene::UpdateIsQuitTitle()
{
	//�C�[�W���O
	for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteStageClear_->SetPosition({ easeMenuEndPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuEndPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);

	//���j���[�̃C�[�W���O���I�������J�ډ��o
	if (spriteStageClear_->GetPosition().x == easeMenuEndPosX_[SCMEN_Menu].end)FadeOut(black_);//��������
	//���S�ɍ����Ȃ�����
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//�^�C�g����
		sceneManager_->ChangeScene("TITLE", stageNum_);
	}
}

void StageClearScene::UpdateIsMenu()
{
	//���j���[�̏���������X�e�[�W�ɂ���ĕύX
	if (!isFinalStage_)
	{
		if (menuCount_ <= SCSMI_NextStage)menuCount_ = SCSMI_NextStage;
		else if (menuCount_ >= SCSMI_Title)menuCount_ = SCSMI_Title;
	}
	else//�ŏI�ʎ��͎��̃X�e�[�W�̕\�L������
	{
		if (menuCount_ <= SCSMI_StageSelect)menuCount_ = SCSMI_StageSelect;
		else if (menuCount_ >= SCSMI_Title)menuCount_ = SCSMI_Title;
	}

	//�I�𒆂̃��j���[�J���[
	const DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,0.7f };
	//�X�e�[�W�N���A�̃J���[
	const DirectX::XMFLOAT4 clearColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	//����w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };
	//�J���[�X�V
	UpdateChangeColor();

	//�C�[�W���O
	easeMenuPosX_[SCMEN_Menu].ease_out_expo();
	for (int i = SCMEN_NextStage; i < SCMEN_Num; i++)easeMenuPosX_[i].ease_in_expo();
	easeFadeInOut_.ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();
	//���W�Z�b�g
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });

	spriteFadeInOut_->SetColor({ white_.x,white_.y, white_.z, easeFadeInOut_.num_X });//�����x�����ς���

	//���j���[����
	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	//�F��ς���(�I�����Ă��郁�j���[�͋���)
	if (menuCount_ == SCSMI_NextStage)//���̃X�e�[�W��I����
	{
		spriteNextStage_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SCMEN_NextStage] });
	}
	else if (menuCount_ == SCSMI_StageSelect)//�X�e�[�W�Z���N�g�J�ڂ�I����
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SCMEN_StageSelect] });
	}
	else if (menuCount_ == SCSMI_Title)//�^�C�g���J�ڎ�
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SCMEN_Title] });
	}
	//���莞�̃X�v���C�g�̃C�[�W���O���I������瑀����󂯕t����
	if (spriteDone_->GetPosition().x == easeMenuPosX_[SCMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			//�C�[�W���O���X�^���o�C�����̏�����
			if (menuCount_ == SCSMI_NextStage)//���̃X�e�[�W��
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isNextStage_ = true;
			}
			else if (menuCount_ == SCSMI_StageSelect)//�X�e�[�W�Z���N�g�֖߂�
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveStageSelect_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isStageSelect_ = true;
			}
			else if (menuCount_ == SCSMI_Title)//�^�C�g���֖߂�
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isQuitTitle_ = true;
			}


		}
	}
	
	//�N���A���o�v���Z�b�g
	const ParticleManager::Preset clear =
	{
		particleClear_.get(),
		{0.0f,0.0f,0.0f},//�g��Ȃ�
		{ 5.0f ,2.0f,0.0f },
		{ 3.0f,6.0f,0.3f },
		{ 0.0f,0.001f,0.0f },
		3,
		{ RandomMTFloat(4.0f,6.0f), 0.0f },
		{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f},
		{ RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f }
	};
	
	const DirectX::XMFLOAT3 clearStartPosLeft = { -30.0f,-30.0f,-5.0f };//����
	const DirectX::XMFLOAT3 clearStartPosRight = { 30.0f,-30.0f,-5.0f };//�E��

	//������
	pmClear_->ActiveY(clear.particle, clearStartPosLeft, clear.pos, clear.vel,
		clear.acc, clear.num, clear.scale, clear.startColor, clear.endColor);

	//�E����
	pmClear_->ActiveY(clear.particle, clearStartPosRight, clear.pos, clear.vel,
		clear.acc, clear.num, clear.scale, clear.startColor, clear.endColor);
	//�J���[�Z�b�g
	spriteStageClear_->SetColor(clearColor);
	spriteDone_->SetColor(doneColor);
}

void StageClearScene::Draw()
{
	//�w�i�X�v���C�g�`��O����
	spCommon_->PreDraw();

	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();				  //���@
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Draw();				  //�V��
	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)skydome->Draw();				  //��
	if (!isStageSelect_)for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();	  //�S�[��

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());


	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());
	pmClear_->Draw();	//�N���A�G�t�F�N�g
	pmSmoke_->Draw();	//���G�t�F�N�g
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	//�O�i�X�v���C�g
	spCommon_->PreDraw();
	//�X�v���C�g�`��	
	spriteStageClear_->Draw();					   //�X�e�[�W�N���A���̃X�v���C�g
	if (!isFinalStage_)spriteNextStage_->Draw();   //���̃X�e�[�W�\���̃X�v���C�g(�ŏI�ʂ̏ꍇ�X�L�b�v)
	spriteStageSelect_->Draw();					   //�X�e�[�W�Z���N�g�\���̃X�v���C�g
	spriteTitle_->Draw();						   //�^�C�g���\���̃X�v���C�g
	spriteDone_->Draw();						   //����\���̃X�v���C�g
	spriteFadeInOut_->Draw();					   //�t�F�[�h�C���A�E�g�̃X�v���C�g
	spriteLoad_->Draw();						   //���[�h�X�v���C�g
	spriteStageInfoNow_->Draw();				   //���݃X�e�[�W�X�v���C�g
	spriteCursor_->Draw();						   //�J�[�\���X�v���C�g
	spriteStageName_->Draw();					   //�X�e�[�W���X�v���C�g
}

void StageClearScene::FadeOut(const DirectX::XMFLOAT3& rgb)
{
	if (!isFadeOut_)
	{
		//�����ŃX�^���o�C
		easeFadeInOut_.Standby(true);
		isFadeOut_ = true;
	}
	else
	{
		//�����Ńt�F�[�h�C���A�E�g
		const DirectX::XMFLOAT3 negapozi = { 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//�����x�����ς���
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�l�K�|�W�̉��p
		if (isNextStage_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
		}
	}
}

void StageClearScene::Finalize()
{
	//����
	audio_->Finalize();
}

void StageClearScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);

	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("playerdash", true);		 //���@���f��
	modelSkydome_ = Model::LoadFromOBJ("skydomet");				 //�`���[�g���A���X�e�[�W���f��(�V��)
	modelSkydomeStage1_ = Model::LoadFromOBJ("skydome");		 //�X�e�[�W1���f��(�V��)
	modelSkydomeStage2_ = Model::LoadFromOBJ("skydome2");		 //�X�e�[�W2���f��(�V��)
	modelGround_ = Model::LoadFromOBJ("ground");				 //���̃��f��
	modelGoal_ = Model::LoadFromOBJ("sphere");					 //�S�[�����f��
	//�}�b�v�ɓo�^����
	models_.insert(std::make_pair("playerdash", modelPlayer_.get()));
	models_.insert(std::make_pair("skydomet", modelSkydome_.get()));
	models_.insert(std::make_pair("skydome", modelSkydomeStage1_.get()));
	models_.insert(std::make_pair("skydome2", modelSkydomeStage2_.get()));
	models_.insert(std::make_pair("ground", modelGround_.get()));
	models_.insert(std::make_pair("sphere", modelGoal_.get()));

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
			startPos_ = newObject->GetPosition();

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
			newObject->SetCamera(camera_.get());
			// �z��ɓo�^
			objGrounds_.push_back(std::move(newObject));
		}
		else if (objectData.objectType.find("SKYDOME") == LDTOF_TRUE)
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

			//�J�����Z�b�g
			newObject->SetCamera(camera_.get());

			// �z��ɓo�^
			objSkydomes_.push_back(std::move(newObject));
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

			//�J�����Z�b�g
			newObject->SetCamera(camera_.get());

			// �z��ɓo�^
			objGoals_.push_back(std::move(newObject));
		}

	}

}

void StageClearScene::UpdateChangeColor()
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

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.9f,0.0f };//{max,min}

	if (selectColor_.x >= maxAndMinSpeedColor.x)//Max�l�ɓ��B�����甽�]
	{
		isColorReverse_ = true;
	}
	if (selectColor_.x <= maxAndMinSpeedColor.y)//Min�l�ɓ��B�����甽�]��߂�
	{
		isColorReverse_ = false;
	}
}

void StageClearScene::LoadStageNameSprite()
{
	//�X�e�[�W���ƂɃX�e�[�W����ύX
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/2-2.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/2-3.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/2-4.png");
	//�A���J�[�|�C���g�Ȃǂ��Z�b�e�B���O
	spriteStageName_->Initialize(spCommon_, SCSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
	spriteStageName_->Update();
}

void StageClearScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
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
