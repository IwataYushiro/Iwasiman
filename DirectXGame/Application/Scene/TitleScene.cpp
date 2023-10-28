#include "TitleScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "EnumList.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

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

TitleScene::TitleScene(int stagenum) : stageNum_(stagenum) {}

void TitleScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();

	//�J����������
	camera_ = new Camera();
	//�I�[�f�B�I
	audio_->Initialize();

	// ���_���W
	camera_->SetEye({ easeEyeMenu_[0].start, easeEyeMenu_[1].start, easeEyeMenu_[2].start });
	// �����_���W
	camera_->SetTarget({ easeTargetMenu_[0].start, easeTargetMenu_[1].start, easeTargetMenu_[2].start });

	//���x���f�[�^�ǂݍ���
	if (stageNum_ == SL_Default)LoadLVData("scene/titlet");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/title1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/title2");
	else LoadLVData("scene/titlet");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(TSTI_TitleTex, "texture/title3.png");
	spriteTitle_->Initialize(spCommon_, TSTI_TitleTex);
	spriteTitle_->SetColor(backTitleColor_);
	spriteTitle_->SetPosition({ easeTitlePosX_[0].start,50.0f });

	spCommon_->LoadTexture(TSTI_TitleDoneTex, "texture/space2.png");
	spriteTitleDone_->Initialize(spCommon_, TSTI_TitleDoneTex);
	spriteTitleDone_->SetPosition({ easeTitlePosX_[1].start,600.0f });

	spCommon_->LoadTexture(TSTI_MenuTex, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, TSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[0].start,0.0f });
	spriteMenu_->SetColor(otherMenuColor_);

	spCommon_->LoadTexture(TSTI_MenuTutorialTex, "texture/titlemenut.png");
	spriteMenuTutorial_->Initialize(spCommon_, TSTI_MenuTutorialTex);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].start,easeStartStagePosY_.start });

	spCommon_->LoadTexture(TSTI_MenuStageSerectTex, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, TSTI_MenuStageSerectTex);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].start,300.0f });

	spCommon_->LoadTexture(TSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteMenuDone_->Initialize(spCommon_, TSTI_MenuDoneTex);
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].start,550.0f });
	spriteMenuDone_->SetColor(otherMenuColor_);

	spCommon_->LoadTexture(TSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, TSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[4].start,50.0f });
	spriteBack_->SetColor(backTitleColor_);

	spCommon_->LoadTexture(TSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, TSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.start });

	spCommon_->LoadTexture(TSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, TSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.end });//������

	spCommon_->LoadTexture(TSTI_StageInfoNowTex, "texture/stage1.png");
	spriteStageInfoNow_->Initialize(spCommon_, TSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//FBX
	//objF = ObjectFbx::Create();
	//modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	//objF->SetModelFBX(modelF);
	//objF->SetCamera(camera_);
	//objF->PlayAnimation();//�X�V�ŌĂԂƎ~�܂邩�璍��

	//�p�[�e�B�N��
	particle1_ = Particle::LoadFromParticleTexture("particle7.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	easeFadeInOut_.Standby(false);
}

void TitleScene::Update()
{

	//���j���[�㉺��
	if (menuCount_ <= TSMI_Tutorial)menuCount_ = TSMI_Tutorial;
	else if (menuCount_ >= TSMI_StageSelect)menuCount_ = TSMI_StageSelect;

	DirectX::XMFLOAT4 TitleDoneColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	//SetColor���O�ɌĂяo��
	UpdateChangeColor();

	if (isStartGame_)UpdateIsStartGame();
	else if (isStageSelect_)UpdateIsStageSelect();
	else if (isMenu_)UpdateIsMenu();
	else if (isBack_)UpdateIsBack();
	else
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
				isMenu_ = true;

			}
		}
	}
	spriteTitleDone_->SetColor(TitleDoneColor);
	spriteMenu_->SetColor(TitleDoneColor);
	spriteMenuDone_->SetColor(TitleDoneColor);



	spriteTitle_->Update();
	spriteTitleDone_->Update();
	spriteMenu_->Update();
	spriteMenuTutorial_->Update();
	spriteMenuStageSelect_->Update();
	spriteMenuDone_->Update();
	spriteBack_->Update();
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();

	for (Object3d*& player : objPlayers_)
	{
		//�p�[�e�B�N���J���[�̔��f�e�X�g��(���F)
		/*DirectX::XMFLOAT4 particleColorS =
		{
			MyMath::RandomMTFloat(0.0f,1.0f),
			MyMath::RandomMTFloat(0.0f,1.0f),
			MyMath::RandomMTFloat(0.0f,1.0f),
			1.0f
		};*/
		/*DirectX::XMFLOAT4 particleColorE =
		{
			MyMath::RandomMTFloat(0.0f,1.0f),
			MyMath::RandomMTFloat(0.0f,1.0f),
			MyMath::RandomMTFloat(0.0f,1.0f),
			0.0f
		};*/

		DirectX::XMFLOAT4 particleColorS[2] =
		{
			{MyMath::RandomMTFloat(0.0f,1.0f),0.0f,0.0f,1.0f},
			{MyMath::RandomMTFloat(0.7f,1.0f),0.0f,0.0f,1.0f}
		};

		DirectX::XMFLOAT4 particleColorE[2] =
		{
			{0.0f,0.0f,0.0f,0.0f},
			{1.0f,1.0f,1.0f,1.0f},
		};

		pm1_->ActiveX(particle1_, player->GetPosition(), { 0.0f ,2.0f,0.0f }, { -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f }, 3, { 1.0f, 0.0f }, particleColorS[0], particleColorE[0]);

		pm1_->Active(particle1_, player->GetPosition(), { 10.1f ,10.1f,10.1f }, { 0.1f,0.1f,0.1f },
			{ 0.0f,0.001f,0.0f }, 5, { 1.0f, 3.0f }, particleColorS[1], particleColorE[1]);

		player->Update();
	}

	for (Object3d*& ground : objGrounds_)
	{
		DirectX::XMFLOAT3 move = ground->GetPosition();
		DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };
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

	camera_->Update();
	lightGroup_->Update();
	pm1_->Update();

	//objF->Update();

	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(true);
#endif // _DEBUG

	imguiManager_->End();
}

void TitleScene::UpdateIsStartGame()
{
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < 3; i++)easeTargetGameStart_[i].ease_in_out_expo();
	easeStartStagePosX_.ease_out_expo();
	easeStartStagePosY_.ease_out_expo();

	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteMenuTutorial_->SetPosition({ easeStartStagePosX_.num_X,easeStartStagePosY_.num_X });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[4].num_X,50.0f });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeGameStart_[0].num_X, easeEyeGameStart_[1].num_X, easeEyeGameStart_[2].num_X });
	camera_->SetTarget({ easeTargetGameStart_[0].num_X, easeTargetGameStart_[1].num_X, easeTargetGameStart_[2].num_X });

	for (Object3d*& goal : objGoals_)
	{

		DirectX::XMFLOAT3 move = goal->GetPosition();
		DirectX::XMFLOAT3 speed = { -1.5f,0.0f,0.0f };
		//X�l�������܂ŗ�����C�[�W���O
		const float gameStartPos_ = 60.0f;

		move.x += speed.x;
		goal->SetPosition(move);
		if (goal->GetPosition().x <= gameStartPos_)
		{
			FadeOut(white_);//�Q�[���v���C�J�ڎ��͔�������
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//�`���[�g���A���X�e�[�W��
				sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);
			}
		};


	}

}

void TitleScene::UpdateIsStageSelect()
{
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < 3; i++)easeTargetGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < 3; i++)easePlayerMove_[i].ease_in_out_expo();

	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[4].num_X,50.0f });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeGameStart_[0].num_X, easeEyeGameStart_[1].num_X, easeEyeGameStart_[2].num_X });
	camera_->SetTarget({ easeTargetGameStart_[0].num_X, easeTargetGameStart_[1].num_X, easeTargetGameStart_[2].num_X });

	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerMove_[0].num_X,easePlayerMove_[1].num_X,easePlayerMove_[2].num_X });

		if (spriteMenu_->GetPosition().x == easeMenuPosX_[0].start)
		{
			FadeOut(black_);//��������
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//�X�e�[�W�I��
				if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
				else sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);//�`���[�g���A���ɔ�΂��Ɩ{���]�|
			}
		}
	}

}

void TitleScene::UpdateIsBack()
{
	//�C�[�W���O
	for (int i = 0; i < 2; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetMenu_[i].ease_out_expo();
	//���W�Z�b�g
	spriteTitle_->SetPosition({ easeTitlePosX_[0].num_X,50.0f });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[1].num_X,600.0f });
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[4].num_X,50.0f });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeMenu_[0].num_X, easeEyeMenu_[1].num_X, easeEyeMenu_[2].num_X });
	camera_->SetTarget({ easeTargetMenu_[0].num_X, easeTargetMenu_[1].num_X, easeTargetMenu_[2].num_X });

	if (easeMenuPosX_[4].num_X == easeMenuPosX_[4].start)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{

			for (int i = 0; i < 2; i++)easeTitlePosX_[i].Standby(false);
			for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(false);
			for (int i = 0; i < 3; i++)easeEyeMenu_[i].Standby(false);
			for (int i = 0; i < 3; i++)easeTargetMenu_[i].Standby(false);
			isMenu_ = true;
			isBack_ = false;
		}
	}
}

void TitleScene::UpdateIsMenu()
{
	DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };

	//�C�[�W���O
	for (int i = 0; i < 2; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetMenu_[i].ease_out_expo();

	//���W�Z�b�g
	spriteTitle_->SetPosition({ easeTitlePosX_[0].num_X,50.0f });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[1].num_X,600.0f });
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[4].num_X,50.0f });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeMenu_[0].num_X, easeEyeMenu_[1].num_X, easeEyeMenu_[2].num_X });
	camera_->SetTarget({ easeTargetMenu_[0].num_X, easeTargetMenu_[1].num_X, easeTargetMenu_[2].num_X });

	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	if (menuCount_ == TSMI_Tutorial)
	{
		spriteMenuTutorial_->SetColor(selectMenuColor);
		spriteMenuStageSelect_->SetColor(otherMenuColor_);
	}
	else if (menuCount_ == TSMI_StageSelect)
	{
		spriteMenuTutorial_->SetColor(otherMenuColor_);
		spriteMenuStageSelect_->SetColor(selectMenuColor);
	}


	if (easeMenuPosX_[0].num_X == easeMenuPosX_[0].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			if (menuCount_ == TSMI_Tutorial)
			{
				for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < 3; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < 3; i++)easeTargetGameStart_[i].Standby(false);
				easeStartStagePosX_.Standby(false);
				easeStartStagePosY_.Standby(false);
				isStartGame_ = true;
			}
			else if (menuCount_ == TSMI_StageSelect)
			{
				for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < 3; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < 3; i++)easeTargetGameStart_[i].Standby(false);
				for (int i = 0; i < 3; i++)easePlayerMove_[i].Standby(false);
				isStageSelect_ = true;

			}
		}
		if (input_->TriggerKey(DIK_Q))
		{
			for (int i = 0; i < 2; i++)easeTitlePosX_[i].Standby(true);
			for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < 3; i++)easeEyeMenu_[i].Standby(true);
			for (int i = 0; i < 3; i++)easeTargetMenu_[i].Standby(true);
			isBack_ = true;
			isMenu_ = false;
		}
	}

}

void TitleScene::FadeOut(DirectX::XMFLOAT3 rgb)
{
	if (!isFadeOut_)
	{
		easeFadeInOut_.Standby(true);
		isFadeOut_ = true;
	}
	else
	{
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//�����x�����ς���
		spriteLoad_->SetColor({ 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z, easeFadeInOut_.num_X });//�l�K�|�W�̉��p
		if (isStartGame_)spriteStageInfoNow_->SetColor({ 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��
	}
}

void TitleScene::Draw()
{
	//�w�i�X�v���C�g�`��O����


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

	//objF->Draw();
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
	spriteTitle_->Draw();
	spriteTitleDone_->Draw();
	spriteMenu_->Draw();
	spriteMenuTutorial_->Draw();
	spriteMenuStageSelect_->Draw();
	spriteMenuDone_->Draw();
	spriteBack_->Draw();
	spriteFadeInOut_->Draw();
	spriteLoad_->Draw();
	spriteStageInfoNow_->Draw();
}

void TitleScene::Finalize()
{
	//����
	audio_->Finalize();
	//�X�v���C�g
	delete spriteTitle_;
	delete spriteTitleDone_;
	delete spriteMenu_;
	delete spriteMenuTutorial_;
	delete spriteMenuStageSelect_;
	delete spriteMenuDone_;
	delete spriteBack_;
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
	delete particle1_;
	delete pm1_;
	//FBX
	//delete objF;
	//delete modelF;
}

void TitleScene::LoadLVData(const std::string& stagePath)
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

		if (objectData.objectType.find("PLAYER") == 0)
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
		else if (objectData.objectType.find("PLANE") == 0)
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
		else if (objectData.objectType.find("SKYDOME") == 0)
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
		else if (objectData.objectType.find("GOAL") == 0)
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

void TitleScene::UpdateChangeColor()
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
