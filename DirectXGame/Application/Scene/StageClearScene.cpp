#include "StageClearScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

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

StageClearScene::StageClearScene(int stagenum) :stageNum_(stagenum) {}

void StageClearScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();

	//�J����
	camera_ = new Camera();
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
	Object3d::SetLightGroup(lightGroup_);

	//�X�v���C�g
	if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_Stage2_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else spCommon_->LoadTexture(SCSTI_MenuTex, "texture/stageclear2.png");

	spriteStageClear_->Initialize(spCommon_, SCSTI_MenuTex);
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].start,menuPosY_[SCMEN_Menu] });

	spCommon_->LoadTexture(SCSTI_NextStageTex, "texture/nextstage.png");
	spriteNextStage_->Initialize(spCommon_, SCSTI_NextStageTex);
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].start,menuPosY_[SCMEN_NextStage] });

	spCommon_->LoadTexture(SCSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, SCSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].start,menuPosY_[SCMEN_StageSelect] });

	spCommon_->LoadTexture(SCSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, SCSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].start,menuPosY_[SCMEN_Title] });

	spCommon_->LoadTexture(SCSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, SCSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].start,menuPosY_[SCMEN_SelectSpace] });

	spCommon_->LoadTexture(SCSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SCSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });

	spCommon_->LoadTexture(SCSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, SCSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	const int remainderNum = stageNum_ % 10;//�]��ɂ���ăX�v���C�g��ς���
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stagef.png");

	spriteStageInfoNow_->Initialize(spCommon_, SCSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//�p�[�e�B�N��
	particleClear_ = Particle::LoadFromParticleTexture("particle1.png");
	pmClear_ = ParticleManager::Create();
	pmClear_->SetParticleModel(particleClear_);
	pmClear_->SetCamera(camera_);

	particleSmoke_ = Particle::LoadFromParticleTexture("particle8.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_);
	pmSmoke_->SetCamera(camera_);

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

	easeFadeInOut_.Standby(false);
	for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].Standby(false);
}

void StageClearScene::Update()
{
	if (isNextStage_)UpdateIsNextStage();
	else if (isStageSelect_)UpdateIsStageSelect();
	else if (isQuitTitle_)UpdateIsQuitTitle();
	else UpdateIsMenu();

	for (Object3d*& player : objPlayers_)
	{
		//���v���Z�b�g
		const ParticleManager::Preset smoke =
		{
			particleSmoke_,
			player->GetPosition(),
			{ 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			3,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};

		pmSmoke_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		player->Update();
	}

	for (Object3d*& ground : objGrounds_)
	{
		DirectX::XMFLOAT3 move = ground->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };
		//X�l�������܂ŗ����烋�[�v
		const float returnPos_ = -120.0f;

		move.x += speed.x;
		ground->SetPosition(move);
		if (ground->GetPosition().x <= returnPos_)ground->SetPosition(startPos_);

		ground->Update();
	}

	for (Object3d*& skydome : objSkydomes_)
	{
		//�V����]�p
		DirectX::XMFLOAT3 rotSkydome = skydome->GetRotation();

		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);

		skydome->Update();
	}
	for (Object3d*& goal : objGoals_)goal->Update();



	spriteStageClear_->Update();
	spriteNextStage_->Update();
	spriteStageSelect_->Update();
	spriteTitle_->Update();
	spriteDone_->Update();
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();

	camera_->Update();
	lightGroup_->Update();
	pmClear_->Update();
	pmSmoke_->Update();

	imguiManager_->Begin();
	imguiManager_->End();

}

void StageClearScene::UpdateIsNextStage()
{
	//�C�[�W���O
	for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].ease_out_expo();

	//���W�Z�b�g
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].num_X, easeEyeStageClear_[XYZ_Y].num_X, easeEyeStageClear_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].num_X, easeTargetStageClear_[XYZ_Y].num_X, easeTargetStageClear_[XYZ_Z].num_X });

	for (Object3d*& player : objPlayers_)
	{
		//���v���Z�b�g
		const ParticleManager::Preset clear =
		{
			particleClear_,
			{player->GetPosition().x+70.0f,player->GetPosition().y + 30.0f ,player->GetPosition().z},
			{ 0.0f ,60.0f,75.0f },
			{  RandomMTFloat(-15.0f,-8.0f),0.0f,0.0f},
			{ 0.0f,0.001f,0.0f },
			3,
			{ RandomMTFloat(4.0f,6.0f), 0.0f },
			{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f},
			{ RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f }
		};

		pmClear_->ActiveX(clear.particle, clear.startPos, clear.pos, clear.vel,
			clear.acc, clear.num, clear.scale, clear.startColor, clear.endColor);

	}

	for (Object3d*& goal : objGoals_)
	{

		DirectX::XMFLOAT3 move = goal->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.5f,0.0f,0.0f };
		//X�l�������܂ŗ�����C�[�W���O
		const float gameStartPos_ = 60.0f;

		move.x += speed.x;
		goal->SetPosition(move);
		if (goal->GetPosition().x <= gameStartPos_)
		{
			FadeOut(white_);//�Q�[���v���C�J�ڎ��͔�������
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
	for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easePlayerMoveStageSelect_[i].ease_in_out_expo();

	//���W�Z�b�g
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].num_X, easeEyeStageClear_[XYZ_Y].num_X, easeEyeStageClear_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].num_X, easeTargetStageClear_[XYZ_Y].num_X, easeTargetStageClear_[XYZ_Z].num_X });

	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerMoveStageSelect_[XYZ_X].num_X,easePlayerMoveStageSelect_[XYZ_Y].num_X,easePlayerMoveStageSelect_[XYZ_Z].num_X });

		if (spriteStageClear_->GetPosition().x == easeMenuPosX_[SCMEN_Menu].start)
		{
			FadeOut(black_);//��������
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
	for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	//���W�Z�b�g
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });

	if (spriteStageClear_->GetPosition().x == easeMenuPosX_[SCMEN_Menu].start)FadeOut(black_);//��������
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
	else
	{
		if (menuCount_ <= SCSMI_StageSelect)menuCount_ = SCSMI_StageSelect;
		else if (menuCount_ >= SCSMI_Title)menuCount_ = SCSMI_Title;
	}

	//�I�𒆂̃��j���[�J���[
	const DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,0.5f };
	//�X�e�[�W�N���A�̃J���[
	const DirectX::XMFLOAT4 clearColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	//����w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };
	UpdateChangeColor();

	//�C�[�W���O
	easeMenuPosX_[SCMEN_Menu].ease_out_expo();
	for (int i = SCMEN_NextStage; i < SCMEN_Num; i++)easeMenuPosX_[i].ease_in_expo();
	easeFadeInOut_.ease_in_out_quint();

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


	if (menuCount_ == SCSMI_NextStage)
	{
		spriteNextStage_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == SCSMI_StageSelect)
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == SCSMI_Title)
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
	}

	if (spriteDone_->GetPosition().x == easeMenuPosX_[SCMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{

			if (menuCount_ == SCSMI_NextStage)
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].Standby(false);
				isNextStage_ = true;
			}
			else if (menuCount_ == SCSMI_StageSelect)
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveStageSelect_[i].Standby(false);
				isStageSelect_ = true;
			}
			else if (menuCount_ == SCSMI_Title)
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].Standby(true);
				isQuitTitle_ = true;
			}


		}
	}
	
	//�N���A���o�v���Z�b�g
	const ParticleManager::Preset clear =
	{
		particleClear_,
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

	spriteStageClear_->SetColor(clearColor);
	spriteDone_->SetColor(doneColor);
}

void StageClearScene::Draw()
{
	//�w�i�X�v���C�g�`��O����
	spCommon_->PreDraw();

	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& ground : objGrounds_)ground->Draw();
	for (Object3d*& skydome : objSkydomes_)skydome->Draw();
	if (!isStageSelect_)for (Object3d*& goal : objGoals_)goal->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());


	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());
	pmClear_->Draw();
	pmSmoke_->Draw();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	//�O�i�X�v���C�g
	spCommon_->PreDraw();
	//�X�v���C�g�`��	
	spriteStageClear_->Draw();
	if (!isFinalStage_)spriteNextStage_->Draw();
	spriteStageSelect_->Draw();
	spriteTitle_->Draw();
	spriteDone_->Draw();
	spriteFadeInOut_->Draw();
	spriteLoad_->Draw();
	spriteStageInfoNow_->Draw();
	
}

void StageClearScene::FadeOut(DirectX::XMFLOAT3 rgb)
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
		if (isNextStage_)spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });
	}
}

void StageClearScene::Finalize()
{
	//����
	audio_->Finalize();
	//�X�v���C�g
	delete spriteStageClear_;
	delete spriteNextStage_;
	delete spriteStageSelect_;
	delete spriteTitle_;
	delete spriteDone_;
	delete spriteFadeInOut_;
	delete spriteLoad_;
	delete spriteStageInfoNow_;

	//���x���f�[�^�p�I�u�W�F�N�g
	for (Object3d*& player : objPlayers_)delete player;
	for (Object3d*& ground : objGrounds_)delete ground;
	for (Object3d*& skydome : objSkydomes_)delete skydome;
	for (Object3d*& goal : objGoals_)delete goal;

	delete modelPlayer_;
	delete modelSkydome_;
	delete modelSkydomeStage1_;
	delete modelSkydomeStage2_;
	delete modelGround_;
	delete modelGoal_;

	//�J����
	delete camera_;
	//���C�g
	delete lightGroup_;
	//�p�[�e�B�N��
	delete particleClear_;
	delete pmClear_;

	delete particleSmoke_;
	delete pmSmoke_;
}

void StageClearScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);

	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player", true);
	modelSkydome_ = Model::LoadFromOBJ("skydomet");
	modelSkydomeStage1_ = Model::LoadFromOBJ("skydome");
	modelSkydomeStage2_ = Model::LoadFromOBJ("skydome2");
	modelGround_ = Model::LoadFromOBJ("ground");
	modelGoal_ = Model::LoadFromOBJ("sphere");

	models_.insert(std::make_pair("player", modelPlayer_));
	models_.insert(std::make_pair("skydomet", modelSkydome_));
	models_.insert(std::make_pair("skydome", modelSkydomeStage1_));
	models_.insert(std::make_pair("skydome2", modelSkydomeStage2_));
	models_.insert(std::make_pair("ground", modelGround_));
	models_.insert(std::make_pair("sphere", modelGoal_));

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
			Object3d* newObject = Object3d::Create();
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

			newObject->SetCamera(camera_);
			// �z��ɓo�^
			objPlayers_.push_back(newObject);
		}
		else if (objectData.objectType.find("PLANE") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			Object3d* newObject = Object3d::Create();
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

			newObject->SetCamera(camera_);
			// �z��ɓo�^
			objGrounds_.push_back(newObject);
		}
		else if (objectData.objectType.find("SKYDOME") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			Object3d* newObject = Object3d::Create();
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

			newObject->SetCamera(camera_);
			// �z��ɓo�^
			objSkydomes_.push_back(newObject);
		}
		else if (objectData.objectType.find("GOAL") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			Object3d* newObject = Object3d::Create();
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

			newObject->SetCamera(camera_);
			// �z��ɓo�^
			objGoals_.push_back(newObject);
		}

	}

}

void StageClearScene::UpdateChangeColor()
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

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.9f,0.0f };//{max,min}

	if (selectColor_.x >= maxAndMinSpeedColor.x)
	{
		isColorReverse_ = true;
	}
	if (selectColor_.x <= maxAndMinSpeedColor.y)
	{
		isColorReverse_ = false;
	}
}
