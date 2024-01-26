#include "GameOverScene.h"
#include "FbxLoader.h"
#include "TouchableObject.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;
using namespace IwasiEngine;

/*

*	GameOverScene.cpp

*	�Q�[���I�[�o�[�V�[��

*/
GameOverScene::GameOverScene(const int stagenum) :stageNum_(stagenum) {}

void GameOverScene::Initialize()
{
	//�C���X�^���X�擾
	dxCommon_ = DirectXCommon::GetInstance();		//DirectX���
	spCommon_ = SpriteCommon::GetInstance();		//�X�v���C�g���
	input_ = Input::GetInstance();					//���͏��
	audio_ = Audio::GetInstance();					//����
	sceneManager_ = SceneManager::GetInstance();	//�V�[���}�l�[�W���[
	imguiManager_ = ImGuiManager::GetInstance();	//ImGui�}�l�[�W���[

	//�C�[�W���O�̃��[�h
	for (int i = 0; i < GOMEN_Num; i++)LoadEasingData("gameover/menuposx.csv", easeMenuPosX_[i], i);
	for (int i = 0; i < GOMEN_Num; i++)LoadEasingData("gameover/menuendposx.csv", easeMenuEndPosX_[i], i);
	LoadEasingData("gameover/cursorposx.csv", easeCursorPosX_);
	LoadEasingData("gameover/continueposx.csv", easeContinuePosX_);
	LoadEasingData("gameover/continueposy.csv", easeContinuePosY_);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/eyecontinue.csv", easeEyeContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/targetcontinue.csv", easeTargetContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playerrotatecontinue.csv", easePlayerRotateContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playermovecontinue.csv", easePlayerMoveContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/eyequitstageselect.csv", easeEyeQuitStageSelect_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/targetquitstageselect.csv", easeTargetQuitStageSelect_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playerrotatequitstageselect.csv", easePlayerRotateQuitStageSelect_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playermovequitstageselect.csv", easePlayerMoveQuitStageSelect_[i], i);
	LoadEasingData("gameover/fadeinout.csv", easeFadeInOut_);


	//�J����������
	camera_ = std::make_unique<Camera>();
	//�I�[�f�B�I
	audio_->Initialize();

	//�J����
	// ���_���W
	camera_->SetEye({ easeEyeContinue_[XYZ_X].start, easeEyeContinue_[XYZ_Y].start, easeEyeContinue_[XYZ_Z].start });
	// �����_���W
	camera_->SetTarget({ easeTargetContinue_[XYZ_X].start, easeTargetContinue_[XYZ_Y].start, easeTargetContinue_[XYZ_Z].start });


	//���x���f�[�^�ǂݍ���
	if (stageNum_ == SL_Default)LoadLVData("scene/gameovert");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/gameover1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/gameover2");
	else LoadLVData("scene/gameovert");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());

	//�Q�[���I�[�o�[�X�v���C�g
	spCommon_->LoadTexture(GOSTI_MenuTex, "texture/gameover2.png");
	spriteGameOver_->Initialize(spCommon_, GOSTI_MenuTex);
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].start,menuPosY_[GOMEN_Menu] });

	//�R���e�B�j���[�\���X�v���C�g
	spCommon_->LoadTexture(GOSTI_ContinueTex, "texture/continue.png");
	spriteContinue_->Initialize(spCommon_, GOSTI_ContinueTex);
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].start,menuPosY_[GOMEN_Continue] });

	//�X�e�[�W�Z���N�g�\���X�v���C�g
	spCommon_->LoadTexture(GOSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, GOSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].start,menuPosY_[GOMEN_StageSelect] });

	//�^�C�g���\���X�v���C�g
	spCommon_->LoadTexture(GOSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, GOSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].start,menuPosY_[GOMEN_Title] });

	//����X�v���C�g
	spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, GOSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].start,menuPosY_[GOMEN_SelectSpace] });

	//�t�F�[�h�C���A�E�g�X�v���C�g
	spCommon_->LoadTexture(GOSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, GOSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });

	//���[�h�X�v���C�g
	spCommon_->LoadTexture(GOSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, GOSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//���݃X�e�[�W�X�v���C�g
	const int32_t remainderNum = stageNum_ % 10;//�]��ɂ���ăX�v���C�g��ς���
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage1.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage4) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stagef.png");
	spriteStageInfoNow_->Initialize(spCommon_, GOSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//�J�[�\���X�v���C�g
	spCommon_->LoadTexture(GOSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, GOSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[GOMEN_Continue] });

	//���j���[����X�v���C�g
	spCommon_->LoadTexture(GOSTI_MenuUITex, "texture/menuui.png");
	spriteMenuUI_->Initialize(spCommon_, GOSTI_MenuUITex);
	spriteMenuUI_->SetPosition({ easeMenuPosX_[GOMEN_UI].start,menuPosY_[GOMEN_UI] });

	//�X�e�[�W���X�v���C�g
	LoadStageNameSprite();

	//�p�[�e�B�N��
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");		//��
	particleFall_ = Particle::LoadFromParticleTexture("wind.png");			//�������o�̃p�[�e�B�N��
	particleSmoke_ = Particle::LoadFromParticleTexture("particle9.png");	//��
	particleGoal_ = Particle::LoadFromParticleTexture("particle1.png");		//�S�[���p
	//�W�F�b�g�p
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFall_.get());
	pmFire_->SetCamera(camera_.get());
	//���p
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetBlendMode(ParticleManager::BP_ALPHA);
	pmSmoke_->SetParticleModel(particleSmoke_.get());
	pmSmoke_->SetCamera(camera_.get());
	//�S�[���I�u�W�F�N�g�p
	pmGoal_ = ParticleManager::Create();
	pmGoal_->SetParticleModel(particleGoal_.get());
	pmGoal_->SetCamera(camera_.get());

	//�C�[�W���O�X�^���o�C
	easeFadeInOut_.Standby(false);
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].Standby(false);
	easeCursorPosX_.Standby(false);

}

void GameOverScene::Update()
{
	//���j���[���
	if (menuCount_ <= GOSMI_Continue)menuCount_ = GOSMI_Continue;
	else if (menuCount_ >= GOSMI_Title)menuCount_ = GOSMI_Title;


	if (isContinue_)UpdateIsContinue();						//�R���e�B�j���[
	else if (isQuitStageSelect_)UpdateIsQuitStageSelect();	//�X�e�[�W�Z���N�g�J��
	else if (isQuitTitle_)UpdateIsQuitTitle();				//�^�C�g���J��
	else UpdateIsGameOver();								//���j���[���

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
			{ 0.0f,0.0f,0.0f,1.0f }
		};

		if (completeRotate_)//�C�[�W���O�ɂ���]���I�������
		{
			//�_�b�V���G�t�F�N�g�ɐ؂�ւ�
			pmFire_->ActiveX(fire);
		}
		//�X�V
		player->Update();
	}
	//�V��(�w�i)
	for (std::unique_ptr<Object3d>& stage : objStages_)
	{
		//�V����]�p
		DirectX::XMFLOAT3 rotStage = stage->GetRotation();
		//�V��(�w�i)�͏펞��]����
		const float rotSpeed = -0.2f;
		rotStage.y += rotSpeed;
		stage->SetRotation(rotStage);
		//�X�V
		stage->Update();
	}
	//�S�[��
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Update();//�X�V

	//�X�v���C�g�X�V
	spriteGameOver_->Update();					 //�Q�[���I�[�o�[�X�v���C�g
	spriteContinue_->Update();					 //�R���e�B�j���[�\���X�v���C�g
	spriteStageSelect_->Update();				 //�X�e�[�W�Z���N�g�\���X�v���C�g
	spriteTitle_->Update();						 //�^�C�g���\���X�v���C�g
	spriteDone_->Update();						 //����X�v���C�g
	spriteFadeInOut_->Update();					 //�t�F�[�h�C���A�E�g�X�v���C�g
	spriteLoad_->Update();						 //���[�h�X�v���C�g
	spriteStageInfoNow_->Update();				 //���݃X�e�[�W�X�v���C�g
	spriteCursor_->Update();					 //�J�[�\���X�v���C�g
	spriteStageName_->Update();					 //�X�e�[�W���X�v���C�g
	spriteMenuUI_->Update();					 //���j���[������@�X�v���C�g

	//�X�V
	camera_->Update();			//�J����
	lightGroup_->Update();		//���C�g
	//�p�[�e�B�N���}�l�[�W���[�X�V
	pmFire_->Update();			//��			
	pmSmoke_->Update();			//��
	pmGoal_->Update();
	//ImGui
	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(false);//�f�o�b�O�J����
#endif // DEBUG
	imguiManager_->End();
}

void GameOverScene::UpdateIsGameOver()
{
	//�Q�[���I�[�o�[�J���[
	DirectX::XMFLOAT4 gameOverColor;
	const DirectX::XMFLOAT4 isLightBackGroundGameOverColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };//���邢�w�i
	const DirectX::XMFLOAT4 isDarkBackGroundGameOverColor = { selectColor_.x + 0.1f,selectColor_.y + 0.1f,1.0f,1.0f };//�Â��w�i

	//�X�e�[�W�ʒu(�w�i)�ɂ���ĐF��ς���
	if (stageNum_ == SL_Default)gameOverColor = isLightBackGroundGameOverColor;
	else if (stageNum_ <= SL_Stage1_StageID)gameOverColor = isLightBackGroundGameOverColor;
	else if (stageNum_ <= SL_Stage2_StageID)gameOverColor = isDarkBackGroundGameOverColor;
	else gameOverColor = isLightBackGroundGameOverColor;

	//�I�����Ă�F
	DirectX::XMFLOAT4 selectMenuColor;
	const DirectX::XMFLOAT4 isLightBackGroundSelectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 isDarkBackGroundSelectMenuColor = { 1.0f,selectColor_.y + 0.1f,selectColor_.z + 0.1f,1.0f };

	if (stageNum_ == SL_Default)selectMenuColor = isLightBackGroundSelectMenuColor;
	else if (stageNum_ <= SL_Stage1_StageID)selectMenuColor = isLightBackGroundSelectMenuColor;
	else if (stageNum_ <= SL_Stage2_StageID)selectMenuColor = isDarkBackGroundSelectMenuColor;
	else selectMenuColor = isLightBackGroundSelectMenuColor;

	//�I������Ă��Ȃ��F
	DirectX::XMFLOAT4 otherMenuColor;
	const DirectX::XMFLOAT4 isLightBackGroundOtherMenuColor = { 0.0f,0.0f,0.0f,0.7f };//���邢�w�i
	const DirectX::XMFLOAT4 isDarkBackGroundOtherMenuColor = { 1.0f,1.0f,1.0f,0.7f };//�Â��w�i

	//�X�e�[�W�ʒu(�w�i)�ɂ���ĐF��ς���
	if (stageNum_ == SL_Default)otherMenuColor = isLightBackGroundOtherMenuColor;
	else if (stageNum_ <= SL_Stage1_StageID)otherMenuColor = isLightBackGroundOtherMenuColor;
	else if (stageNum_ <= SL_Stage2_StageID)otherMenuColor = isDarkBackGroundOtherMenuColor;
	else otherMenuColor = isLightBackGroundOtherMenuColor;

	//����w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };
	//�J���[�X�V
	UpdateChangeColor();

	//�C�[�W���O
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();

	//���W,�J���[�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].num_X,menuPosY_[GOMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteFadeInOut_->SetColor({ deepRed_.x,deepRed_.y, deepRed_.z, easeFadeInOut_.num_X });//�����x�����ς���

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//�����Ă�Ƃ��̃p�[�e�B�N��
		FallParticle(player);

		//�펞�v���C���[�̉�]���C�[�W���O�ɃZ�b�g����
		for (int32_t i = 0; i < XYZ_Num; i++)
		{
			EaseRotateSetUp(player->GetRotation(), easePlayerRotateContinue_[i], i);
			EaseRotateSetUp(player->GetRotation(), easePlayerRotateQuitStageSelect_[i], i);
		}
	}
	//���j���[����
	if (input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_S))menuCount_++;

	//�F��ς���(�I�����Ă��郁�j���[�͋���)
	if (menuCount_ == GOSMI_Continue)//�R���e�B�j���[�I����
	{
		spriteContinue_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[GOMEN_Continue] });
	}
	else if (menuCount_ == GOSMI_StageSelect)//�X�e�[�W�Z���N�g�J�ڂ�I����
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[GOMEN_StageSelect] });
	}
	else if (menuCount_ == GOSMI_Title)//�^�C�g���J�ڂ�I����
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[GOMEN_Title] });
	}
	//���莞�̃X�v���C�g�̃C�[�W���O���I������瑀����󂯕t����
	if (spriteDone_->GetPosition().x == easeMenuPosX_[GOMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			//����X�v���C�g����X�L�b�v�L�[�\���֐؂�ւ�
			spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/skip.png");
			//�C�[�W���O���X�^���o�C�����̏�����
			if (menuCount_ == GOSMI_Continue)//�R���e�B�j���[
			{
				for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeContinue_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetContinue_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerRotateContinue_[i].Standby(false);
				easeContinuePosX_.Standby(false);
				easeContinuePosY_.Standby(false);
				easeCursorPosX_.Standby(true);
				//�R���e�B�j���[
				isContinue_ = true;
				isGameover_ = false;

			}
			else if (menuCount_ == GOSMI_StageSelect)//�X�e�[�W�Z���N�g�֖߂�
			{
				for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeQuitStageSelect_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetQuitStageSelect_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerRotateQuitStageSelect_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				//�X�e�[�W�Z���N�g��
				isQuitStageSelect_ = true;
				isGameover_ = false;
			}
			else if (menuCount_ == GOSMI_Title)//�^�C�g���֖߂�
			{
				for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				//�^�C�g����
				isQuitTitle_ = true;
				isGameover_ = false;

			}
		}
	}
	//�J���[�Z�b�g
	spriteGameOver_->SetColor(gameOverColor);
	spriteDone_->SetColor(doneColor);
}

void GameOverScene::UpdateIsContinue()
{
	//�C�[�W���O
	for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeContinue_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetContinue_[i].ease_in_out_expo();
	easeContinuePosX_.ease_in_out_expo();
	easeContinuePosY_.ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuEndPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeContinuePosX_.num_X,easeContinuePosY_.num_X });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//�J�������Z�b�g
	camera_->SetEye({ easeEyeContinue_[XYZ_X].num_X, easeEyeContinue_[XYZ_Y].num_X, easeEyeContinue_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetContinue_[XYZ_X].num_X, easeTargetContinue_[XYZ_Y].num_X, easeTargetContinue_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{

		//���@�̉�]
		if (!completeRotate_)
		{
			//��]�J�n
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateContinue_[i].ease_in_expo();
			player->SetRotation({ easePlayerRotateContinue_[XYZ_X].num_X,easePlayerRotateContinue_[XYZ_Y].num_X,
				easePlayerRotateContinue_[XYZ_Z].num_X });

			//���@�̉�]���I�������
			if (player->GetRotation().x == easePlayerRotateContinue_[XYZ_X].end)
			{
				//���@�̍��W�C�[�W���O�Ǝ��@�̃_�b�V�����f�����X�^���o�C
				player->SetModel(modelPlayerContinue_.get());
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveContinue_[i].Standby(false);
				completeRotate_ = true;
			}
		}
		else//���@�̉�]�̃C�[�W���O���I������玩�@�̍��W�̃C�[�W���O���n�߂�
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerMoveContinue_[i].ease_in_back();
			player->SetPosition({ easePlayerMoveContinue_[XYZ_X].num_X,easePlayerMoveContinue_[XYZ_Y].num_X,
				easePlayerMoveContinue_[XYZ_Z].num_X });
		}

		if (spriteTitle_->GetPosition().x == easeMenuEndPosX_[GOMEN_Title].end)FadeIn(white_);//��������
		//�X�y�[�X�������ƃX�L�b�v
		else SkipDirectionOnSpace(white_);

		//���S�ɔ����Ȃ�����
		if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
		{
			sceneManager_->ChangeScene("GAMEPLAY", stageNum_);
		}
	}
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
	}
}

void GameOverScene::UpdateIsQuitStageSelect()
{
	//�C�[�W���O
	for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeQuitStageSelect_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetQuitStageSelect_[i].ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuEndPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuEndPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//�J�������Z�b�g
	camera_->SetEye({ easeEyeQuitStageSelect_[XYZ_X].num_X, easeEyeQuitStageSelect_[XYZ_Y].num_X, easeEyeQuitStageSelect_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetQuitStageSelect_[XYZ_X].num_X, easeTargetQuitStageSelect_[XYZ_Y].num_X, easeTargetQuitStageSelect_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{

		//���@�̉�]
		if (!completeRotate_)
		{
			//��]�J�n
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateQuitStageSelect_[i].ease_in_expo();
			player->SetRotation({ easePlayerRotateQuitStageSelect_[XYZ_X].num_X,easePlayerRotateQuitStageSelect_[XYZ_Y].num_X ,easePlayerRotateQuitStageSelect_[XYZ_Z].num_X });
			//���@�̉�]���I�������
			if (player->GetRotation().x == easePlayerRotateQuitStageSelect_[XYZ_X].end)
			{
				//���@�̍��W�C�[�W���O�Ǝ��@�̃_�b�V�����f�����X�^���o�C
				player->SetModel(modelPlayerContinue_.get());
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveQuitStageSelect_[i].Standby(false);
				completeRotate_ = true;
			}
		}
		else//���@�̉�]�̃C�[�W���O���I������玩�@�̍��W�̃C�[�W���O���n�߂�
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerMoveQuitStageSelect_[i].ease_in_quint();
			player->SetPosition({ easePlayerMoveQuitStageSelect_[XYZ_X].num_X,easePlayerMoveQuitStageSelect_[XYZ_Y].num_X,easePlayerMoveQuitStageSelect_[XYZ_Z].num_X });
		}

	}
	//���j���[�̃C�[�W���O���I�������J�ډ��o
	if (spriteTitle_->GetPosition().x == easeMenuEndPosX_[GOMEN_Title].end)FadeIn(black_);//��������
	//�X�y�[�X�������ƃX�L�b�v
	else SkipDirectionOnSpace(black_);
	
	//���S�ɍ����Ȃ�����X�e�[�W�Z���N�g��
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//�X�e�[�W���ƂɃX�e�[�W�Z���N�g�̔w�i���Ⴄ
		if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
		else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
		else if (stageNum_ <= SL_StageTutorial_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);

	}
}

void GameOverScene::UpdateIsQuitTitle()
{
	//�C�[�W���O
	for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuEndPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuEndPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//���@�͗���������
		DirectX::XMFLOAT3 move = player->GetPosition();
		const DirectX::XMFLOAT3 speed = { 0.0f,-1.0f,0.0f };

		move.y += speed.y;
		player->SetPosition(move);
		//�����Ă�Ƃ��̃p�[�e�B�N��
		FallParticle(player);
		//���j���[�̃C�[�W���O���I�������J�ډ��o
		if (spriteGameOver_->GetPosition().x == easeMenuEndPosX_[GOMEN_Menu].end)FadeIn(black_);//��������
		//�X�y�[�X�������ƃX�L�b�v
		else SkipDirectionOnSpace(black_);

		//���S�ɍ����Ȃ�����
		if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
		{
			sceneManager_->ChangeScene("TITLE", stageNum_);//
		}
	}
}

void GameOverScene::Draw()
{
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();					//���@
	for (std::unique_ptr<Object3d>& stage : objStages_)stage->Draw();						//�w�i(�V��)
	if (!isQuitStageSelect_)for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();	//�S�[��

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	pmSmoke_->Draw();	//�p�[�e�B�N��(��)
	pmFire_->Draw();	//�p�[�e�B�N��(��)
	pmGoal_->Draw();	//�p�[�e�B�N��(�S�[��)
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	//�O�i�X�v���C�g
	spCommon_->PreDraw();
	//�X�v���C�g�`��	
	spriteGameOver_->Draw();			 //�Q�[���I�[�o�[�X�v���C�g
	spriteContinue_->Draw();			 //�R���e�B�j���[�\���X�v���C�g
	spriteStageSelect_->Draw();			 //�X�e�[�W�Z���N�g�\���X�v���C�g
	spriteTitle_->Draw();				 //�^�C�g���\���X�v���C�g
	spriteDone_->Draw();				 //����X�v���C�g
	spriteFadeInOut_->Draw();			 //�t�F�[�h�C���A�E�g�X�v���C�g
	spriteLoad_->Draw();				 //���[�h�X�v���C�g
	spriteStageInfoNow_->Draw();		 //���݃X�e�[�W�X�v���C�g
	spriteCursor_->Draw();				 //�J�[�\���X�v���C�g
	spriteStageName_->Draw();			 //�X�e�[�W���X�v���C�g
	spriteMenuUI_->Draw();				//���j���[������@�X�v���C�g
}

void GameOverScene::Finalize()
{
	//����
	audio_->Finalize();

}

void GameOverScene::LoadLVData([[maybe_unused]] const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);

	// ���f����ǂݍ���œo�^
	ModelMapping(modelPlayer_, "playerhit");			  //���@���f��
	ModelMapping(modelPlayerContinue_, "playerdash");	  //���@���f���i�R���e�B�j���[�j
	ModelMapping(modelGoal_, "sphere");					  //�S�[�����f��
	ModelMapping(modelStageTutorial_, "skydomet");		  //�`���[�g���A���X�e�[�W���f��(�V��)
	ModelMapping(modelStage1_, "skydome");				  //�X�e�[�W1���f��(�V��)
	ModelMapping(modelStage2_, "skydome2");				  //�X�e�[�W2���f��(�V��)

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

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
			newObject->SetCamera(camera_.get());
			// �z��ɓo�^
			objStages_.push_back(std::move(newObject));
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

void GameOverScene::FadeIn(const DirectX::XMFLOAT3& color)
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
		const XMFLOAT3 negapozi = { 1.0f - color.x,1.0f - color.y,1.0f - color.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOut_.num_X });//�����x�����ς���
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�l�K�|�W�̉��p
		if (isContinue_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
		}
	}
}

void GameOverScene::UpdateChangeColor()
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

void GameOverScene::EaseRotateSetUp(const DirectX::XMFLOAT3& rotation, Easing& easing, const int32_t num)
{
	//�C�[�W���O�̉�]��ݒ�
	XMFLOAT3 rot = rotation;
	if (num == XYZ_X)easing.SetEasing(rot.x, easing.end, easing.maxtime);
	if (num == XYZ_Y)easing.SetEasing(rot.y, easing.end, easing.maxtime);
	if (num == XYZ_Z)easing.SetEasing(rot.z, easing.end, easing.maxtime);
}

void GameOverScene::FallParticle(const std::unique_ptr<Object3d>& player)
{
	const DirectX::XMFLOAT2 dashOffsetXY = { -2.0f,-1.5f };//�|�W�V�����I�t�Z�b�g
	//���v���Z�b�g
	const ParticleManager::Preset smoke =
	{
		particleSmoke_.get(),
		{player->GetPosition().x + dashOffsetXY.x,player->GetPosition().y + dashOffsetXY.y,player->GetPosition().z},
		{ 3.0f ,4.0f,0.0f },
		{ 0.15f,2.3f,0.2f },
		{ 0.0f,0.001f,0.0f },
		2,
		{ 1.0f, 5.0f },
		{0.5f,0.5f,0.5f,1.0f },
		{ 0.0f,0.0f,0.0f,0.0f }
	};
	//���v���Z�b�g
	const ParticleManager::Preset wind =
	{
		particleFall_.get(),
		player->GetPosition(),
		{ 200.0f ,-100.0f,5.0f },
		{ 0.0f,3.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		8,
		{ 1.0f, 0.0f },
		{1.0f,1.0f,1.0,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};
	//���p�[�e�B�N��
	pmSmoke_->ActiveY(smoke);
	//�����ۂ��p�[�e�B�N��
	pmFire_->ActiveY(wind);

}

void GameOverScene::LoadStageNameSprite()
{
	//�X�e�[�W���ƂɃX�e�[�W����ύX
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-1.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-1.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-2.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-3.png");
	else if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-4.png");
	//�A���J�[�|�C���g�Ȃǂ��Z�b�e�B���O
	spriteStageName_->Initialize(spCommon_, GOSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
	spriteStageName_->Update();
}