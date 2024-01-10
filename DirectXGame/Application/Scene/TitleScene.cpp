#include "TitleScene.h"
#include "FbxLoader.h"

#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace IwasiEngine;
/*

*	TitleScene.cpp

*	�^�C�g���V�[��

*/

//�ÓI�����o�ϐ��̎���
DirectXCommon* TitleScene::dxCommon_ = DirectXCommon::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
Audio* TitleScene::audio_ = Audio::GetInstance();
SceneManager* TitleScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* TitleScene::imguiManager_ = ImGuiManager::GetInstance();

TitleScene::TitleScene(const int stagenum) : stageNum_(stagenum) {}

void TitleScene::Initialize()
{
	//�C���X�^���X�擾
	spCommon_ = SpriteCommon::GetInstance();//�X�v���C�g���

	//�J����������
	camera_ = std::make_unique<Camera>();
	//�I�[�f�B�I
	audio_->Initialize();

	// ���_���W
	camera_->SetEye({ easeEyeMenu_[XYZ_X].start, easeEyeMenu_[XYZ_Y].start, easeEyeMenu_[XYZ_Z].start });
	// �����_���W
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].start, easeTargetMenu_[XYZ_Y].start, easeTargetMenu_[XYZ_Z].start });

	//���x���f�[�^�ǂݍ���
	if (stageNum_ == SL_Default)LoadLVData("scene/titlet");				//�`���[�g���A������߂��Ă����ꍇ
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/title1");	//��X�e�[�W����߂��Ă����ꍇ
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/title2");	//�F���X�e�[�W����߂��Ă����ꍇ
	else LoadLVData("scene/titlet");									//�����̃��x��

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);//0�ԃV���h�E�����𓮂���

	//�X�v���C�g
	//�^�C�g����ʃX�v���C�g
	spCommon_->LoadTexture(TSTI_TitleTex, "texture/title3.png");
	spriteTitle_->Initialize(spCommon_, TSTI_TitleTex);
	spriteTitle_->SetColor(backTitleColor_);
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].start,startTitlePosY_[TS_Title] });

	//�^�C�g����ʃX�v���C�g(�^�C�g���̌��)
	spCommon_->LoadTexture(TSTI_TitleBackTex, "texture/titleback.png");
	spriteTitleBack_->Initialize(spCommon_, TSTI_TitleBackTex);
	spriteTitleBack_->SetPosition({ easeTitlePosX_[TS_Title].start,startTitlePosY_[TS_Title] });

	//�^�C�g������\���X�v���C�g
	spCommon_->LoadTexture(TSTI_TitleDoneTex, "texture/space3.png");
	spriteTitleDone_->Initialize(spCommon_, TSTI_TitleDoneTex);
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].start,startTitlePosY_[TS_Done] });

	//�^�C�g�����j���[��ʃX�v���C�g
	spCommon_->LoadTexture(TSTI_MenuTex, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, TSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].start,menuPosY_[TMEN_Menu] });

	//�`���[�g���A���\���X�v���C�g
	spCommon_->LoadTexture(TSTI_MenuTutorialTex, "texture/titlemenug.png");
	spriteMenuTutorial_->Initialize(spCommon_, TSTI_MenuTutorialTex);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].start,easeStartStagePosY_.start });

	//�X�e�[�W�Z���N�g�\���X�v���C�g
	spCommon_->LoadTexture(TSTI_MenuStageSerectTex, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, TSTI_MenuStageSerectTex);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].start,menuPosY_[TMEN_StageSelect] });

	//�^�C�g�����j���[����\���X�v���C�g
	spCommon_->LoadTexture(TSTI_MenuDoneTex, "texture/space.png");
	spriteMenuDone_->Initialize(spCommon_, TSTI_MenuDoneTex);
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].start,menuPosY_[TMEN_SelectSpace] });

	//�^�C�g�����j���[���^�C�g���ڍs�̃X�v���C�g
	spCommon_->LoadTexture(TSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, TSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].start,menuPosY_[TMEN_Quit] });
	spriteBack_->SetColor(backTitleColor_);

	//�t�F�[�h�C���A�E�g�X�v���C�g
	spCommon_->LoadTexture(TSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, TSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.start });

	//���[�h�X�v���C�g
	spCommon_->LoadTexture(TSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, TSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.end });//������

	//���݃X�e�[�W�X�v���C�g
	spCommon_->LoadTexture(TSTI_StageInfoNowTex, "texture/stage1.png");
	spriteStageInfoNow_->Initialize(spCommon_, TSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//�J�[�\���X�v���C�g
	spCommon_->LoadTexture(TSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, TSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[TMEN_Tutorial] });

	//�X�e�[�W���X�v���C�g
	spCommon_->LoadTexture(TSTI_StageNameTex, "texture/stagename/1-1.png");
	spriteStageName_->Initialize(spCommon_, TSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//�p�[�e�B�N��
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	particleGoal_ = Particle::LoadFromParticleTexture("particle1.png");
	//�u�[�X�g�p
	pmFire_ = ParticleManager::Create();
	pmFire_->SetBlendMode(ParticleManager::BP_ADD);
	pmFire_->SetParticleModel(particleFire_.get());
	pmFire_->SetCamera(camera_.get());
	//�S�[���I�u�W�F�N�g�p
	pmGoal_ = ParticleManager::Create();
	pmGoal_->SetParticleModel(particleGoal_.get());
	pmGoal_->SetCamera(camera_.get());

	//�C�[�W���O�X�^���o�C
	easeFadeInOut_.Standby(false);
}

void TitleScene::Update()
{

	//���j���[�㉺��
	if (menuCount_ <= TSMI_Tutorial)menuCount_ = TSMI_Tutorial;
	else if (menuCount_ >= TSMI_StageSelect)menuCount_ = TSMI_StageSelect;

	//�^�C�g���J���[
	DirectX::XMFLOAT4 titleColor;
	const DirectX::XMFLOAT4 isLightBackGroundTitleColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };//���邢�w�i
	const DirectX::XMFLOAT4 isDarkBackGroundTitleColor = { selectColor_.x + 0.1f,selectColor_.y + 0.1f,1.0f,1.0f };//�Â��w�i

	//�߂��Ă����X�e�[�W�ɂ���ĕ����̐F��ς���
	if (stageNum_ == SL_Default)titleColor = isLightBackGroundTitleColor;
	else if (stageNum_ <= SL_Stage1_StageID)titleColor = isLightBackGroundTitleColor;
	else if (stageNum_ <= SL_Stage2_StageID)titleColor = isDarkBackGroundTitleColor;
	else titleColor = isLightBackGroundTitleColor;

	//����w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };

	//SetColor���O�ɌĂяo��
	UpdateChangeColor();

	if (isStartGame_)UpdateIsStartGame();			//�Q�[���X�^�[�g��
	else if (isStageSelect_)UpdateIsStageSelect();	//�X�e�[�W�Z���N�g�J�ڎ�
	else if (isMenu_)UpdateIsMenu();				//���j���[��
	else if (isBack_)UpdateIsBack();				//�^�C�g���֖߂�����
	else											//�^�C�g����ʎ�
	{
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });//�����x�����ς���

		if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.end)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				for (int i = 0; i < 2; i++)easeTitlePosX_[i].Standby(false);
				for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(false);
				for (int i = 0; i < 3; i++)easeEyeMenu_[i].Standby(false);
				for (int i = 0; i < 3; i++)easeTargetMenu_[i].Standby(false);
				easeCursorPosX_.Standby(false);
				isMenu_ = true;

			}
		}
	}
	//�J���[�Z�b�g
	spriteTitle_->SetColor(titleColor);
	spriteMenu_->SetColor(titleColor);
	spriteTitleDone_->SetColor(doneColor);
	spriteMenuDone_->SetColor(doneColor);

	//�X�v���C�g�X�V
	spriteTitle_->Update();				   //�^�C�g����ʃX�v���C�g
	spriteTitleDone_->Update();			   //�^�C�g������\���X�v���C�g
	spriteMenu_->Update();				   //�^�C�g�����j���[��ʃX�v���C�g
	spriteMenuTutorial_->Update();		   //�`���[�g���A���\���X�v���C�g
	spriteMenuStageSelect_->Update();	   //�X�e�[�W�Z���N�g�\���X�v���C�g
	spriteMenuDone_->Update();			   //�^�C�g�����j���[����\���X�v���C�g
	spriteBack_->Update();				   //�^�C�g�����j���[���^�C�g���ڍs�̃X�v���C�g
	spriteFadeInOut_->Update();			   //�t�F�[�h�C���A�E�g�X�v���C�g
	spriteLoad_->Update();				   //���[�h�X�v���C�g
	spriteStageInfoNow_->Update();		   //���݃X�e�[�W�X�v���C�g
	spriteCursor_->Update();			   //�J�[�\���X�v���C�g
	spriteTitleBack_->Update();			   //�^�C�g����ʃX�v���C�g(�^�C�g���̌��)
	spriteStageName_->Update();			   //�X�e�[�W���X�v���C�g

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		const DirectX::XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//�|�W�V�����I�t�Z�b�g
		
		//���v���Z�b�g
		const ParticleManager::Preset smoke =
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
		//�p�[�e�B�N��
		pmFire_->ActiveX(smoke);
		
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

	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)
	{
		//�V����]�p
		DirectX::XMFLOAT3 rotSkydome = skydome->GetRotation();
		//�펞���
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);
		//�X�V
		skydome->Update();
	}
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Update();
	//�X�V
	camera_->Update();		//�J����
	lightGroup_->Update();	//���C�g
	pmFire_->Update();			//�p�[�e�B�N���}�l�[�W���[(�W�F�b�g)
	pmGoal_->Update();			//�p�[�e�B�N���}�l�[�W���[(�S�[���I�u�W�F�N�g)
	
	//objF->Update();
	
	//ImGui
	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(true);//�f�o�b�O�J����
#endif // _DEBUG

	imguiManager_->End();
}

void TitleScene::UpdateIsStartGame()
{
	//�C�[�W���O
	for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_out_expo();
	easeStartStagePosX_.ease_out_expo();
	easeStartStagePosY_.ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuEndPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeStartStagePosX_.num_X,easeStartStagePosY_.num_X });
	spriteMenuStageSelect_->SetPosition({ easeMenuEndPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuEndPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuEndPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });
	
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
		//�S�[���I�u�W�F�N�g�̈ʒu�����̈ʒu�ɓ���������J�ډ��o
		if (goal->GetPosition().x <= gameStartPos_)
		{
			FadeOut(white_);//�Q�[���v���C�J�ڎ��͔�������
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)//���S�ɔ����Ȃ�����
			{
				//�`���[�g���A���X�e�[�W��
				sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);
			}
		};


	}

}

void TitleScene::UpdateIsStageSelect()
{
	//�C�[�W���O
	for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easePlayerMove_[i].ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuEndPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuEndPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial]});
	spriteMenuStageSelect_->SetPosition({ easeMenuEndPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuEndPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuEndPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//���W�Z�b�g
		player->SetPosition({ easePlayerMove_[XYZ_X].num_X,easePlayerMove_[XYZ_Y].num_X,easePlayerMove_[XYZ_Z].num_X });
		
		//���j���[�̃C�[�W���O���I�������J�ډ��o
		if (spriteMenu_->GetPosition().x == easeMenuEndPosX_[TMEN_Menu].end)
		{
			FadeOut(black_);//��������
			//���S�ɍ����Ȃ�����
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//�X�e�[�W�I��
				if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
				else sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);//�`���[�g���A���ɔ�΂��Ɩ{���]�|
			}
		}
	}

}

void TitleScene::UpdateIsBack()
{
	//�C�[�W���O
	for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetMenu_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title]});
	spriteTitleBack_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].num_X,startTitlePosY_[TS_Done] });
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial] });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//�J�������Z�b�g
	camera_->SetEye({ easeEyeMenu_[XYZ_X].num_X, easeEyeMenu_[XYZ_Y].num_X, easeEyeMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].num_X, easeTargetMenu_[XYZ_Y].num_X, easeTargetMenu_[XYZ_Z].num_X });
	
	//�߂�p�̃X�v���C�g�̃C�[�W���O���I������瑀����󂯕t����
	if (spriteBack_->GetPosition().x == easeMenuPosX_[TMEN_Quit].start)
	{
		if (input_->TriggerKey(DIK_SPACE))//���j���[��
		{
			for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].Standby(false);
			for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].Standby(false);
			easeCursorPosX_.Standby(false);
			isMenu_ = true;
			isBack_ = false;
		}
	}
}

void TitleScene::UpdateIsMenu()
{
	//�I�����Ă�F
	DirectX::XMFLOAT4 selectMenuColor;
	const DirectX::XMFLOAT4 isLightBackGroundSelectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 isDarkBackGroundSelectMenuColor = { 1.0f,selectColor_.y + 0.1f,selectColor_.z + 0.1f,1.0f };
	//�X�e�[�W�ʒu(�w�i)�ɂ���ĐF��ς���
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

	//�C�[�W���O
	for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();
	//���W�Z�b�g
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleBack_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].num_X,startTitlePosY_[TS_Done] });
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial] });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeMenu_[XYZ_X].num_X, easeEyeMenu_[XYZ_Y].num_X, easeEyeMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].num_X, easeTargetMenu_[XYZ_Y].num_X, easeTargetMenu_[XYZ_Z].num_X });

	//���j���[�I��
	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;	
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;	

	//�F��ς���(�I�����Ă��郁�j���[�͋���)
	if (menuCount_ == TSMI_Tutorial)//�`���[�g���A���J�ڂ�I����
	{	
		spriteMenuTutorial_->SetColor(selectMenuColor);
		spriteMenuStageSelect_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[TMEN_Tutorial] });
	}
	else if (menuCount_ == TSMI_StageSelect)//�X�e�[�W�Z���N�g�J�ڂ�I����
	{
		spriteMenuTutorial_->SetColor(otherMenuColor);
		spriteMenuStageSelect_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[TMEN_StageSelect] });
	}

	//���j���[�̃C�[�W���O���I������瑀����󂯕t����
	if (spriteMenu_->GetPosition().x == easeMenuPosX_[TMEN_Menu].end)
	{
		if (input_->TriggerKey(DIK_SPACE))//����
		{
			if (menuCount_ == TSMI_Tutorial)//�`���[�g���A����I�񂾏ꍇ�`���[�g���A���֑J��
			{
				//�C�[�W���O�̃X�^���o�C
				for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
				easeStartStagePosX_.Standby(false);
				easeStartStagePosY_.Standby(false);
				easeCursorPosX_.Standby(true);
				isStartGame_ = true;
			}
			else if (menuCount_ == TSMI_StageSelect)//�X�e�[�W�Z���N�g��I�񂾂�X�e�[�W�Z���N�g�֑J��
			{
				//�C�[�W���O�̃X�^���o�C
				for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerMove_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isStageSelect_ = true;

			}
		}
		if (input_->TriggerKey(DIK_ESCAPE))//ESC�L�[�Ń^�C�g���ɖ߂�
		{
			//�C�[�W���O�̃X�^���o�C
			for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].Standby(true);
			for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].Standby(true);
			easeCursorPosX_.Standby(true);
			isBack_ = true;
			isMenu_ = false;
		}
	}

}

void TitleScene::FadeOut(const DirectX::XMFLOAT3& rgb)
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
		if (isStartGame_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
		}
	}
}

void TitleScene::Draw()
{
	//�w�i�X�v���C�g�`��O����

	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();				//�v���C���[
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Draw();				//�n��
	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)skydome->Draw();				//�V��(�w�i)
	if (!isStageSelect_)for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();	//�S�[��
	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//objF->Draw();
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
	spriteTitleBack_->Draw();				//�^�C�g����ʃX�v���C�g(�^�C�g���̌��)
	spriteTitle_->Draw();					//�^�C�g����ʃX�v���C�g
	spriteTitleDone_->Draw();				//�^�C�g������\���X�v���C�g
	spriteMenu_->Draw();					//�^�C�g�����j���[��ʃX�v���C�g
	spriteMenuTutorial_->Draw();			//�`���[�g���A���\���X�v���C�g
	spriteMenuStageSelect_->Draw();			//�X�e�[�W�Z���N�g�\���X�v���C�g
	spriteMenuDone_->Draw();				//�^�C�g�����j���[����\���X�v���C�g
	spriteBack_->Draw();					//�^�C�g�����j���[���^�C�g���ڍs�̃X�v���C�g
	spriteFadeInOut_->Draw();				//�t�F�[�h�C���A�E�g�X�v���C�g
	spriteLoad_->Draw();					//���[�h�X�v���C�g
	spriteStageInfoNow_->Draw();			//���݃X�e�[�W�X�v���C�g
	spriteCursor_->Draw();					//�J�[�\���X�v���C�g
	spriteStageName_->Draw();				//�X�e�[�W���X�v���C�g
}

void TitleScene::Finalize()
{
	//����
	audio_->Finalize();
}

void TitleScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);

	// ���f����ǂݍ���œo�^
	ModelMapping(modelPlayer_, "playerdash", true);		//���@���f��
	ModelMapping(modelSkydome_, "skydomet");			//�`���[�g���A���X�e�[�W���f��(�V��)
	ModelMapping(modelSkydomeStage1_, "skydome");		//�X�e�[�W1���f��(�V��)
	ModelMapping(modelSkydomeStage2_, "skydome2");		//�X�e�[�W2���f��(�V��)
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
			startPos_ = newObject->GetPosition();

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

void TitleScene::UpdateChangeColor()
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

void TitleScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
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
