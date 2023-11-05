#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

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
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(GOSTI_MenuTex, "texture/gameover2.png");
	spriteGameOver_->Initialize(spCommon_, GOSTI_MenuTex);
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].start,menuPosY_[GOMEN_Menu] });

	spCommon_->LoadTexture(GOSTI_ContinueTex, "texture/continue.png");
	spriteContinue_->Initialize(spCommon_, GOSTI_ContinueTex);
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].start,menuPosY_[GOMEN_Continue] });

	spCommon_->LoadTexture(GOSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, GOSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].start,menuPosY_[GOMEN_StageSelect] });

	spCommon_->LoadTexture(GOSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, GOSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].start,menuPosY_[GOMEN_Title] });

	spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, GOSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].start,menuPosY_[GOMEN_SelectSpace] });

	spCommon_->LoadTexture(GOSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, GOSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });

	spCommon_->LoadTexture(GOSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, GOSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	const int remainderNum = stageNum_ % 10;//�]��ɂ���ăX�v���C�g��ς���
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage1.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage4) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stagef.png");

	spriteStageInfoNow_->Initialize(spCommon_, GOSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
	//�p�[�e�B�N��
	particle1_ = Particle::LoadFromParticleTexture("particle8.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	easeFadeInOut_.Standby(false);
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].Standby(false);


}

void GameOverScene::Update()
{
	if (menuCount_ <= GOSMI_Continue)menuCount_ = GOSMI_Continue;
	else if (menuCount_ >= GOSMI_Title)menuCount_ = GOSMI_Title;


	if (isContinue_)UpdateIsContinue();						//�R���e�B�j���[
	else if (isQuitStageSelect_)UpdateIsQuitStageSelect();	//�X�e�[�W�Z���N�g�J��
	else if (isQuitTitle_)UpdateIsQuitTitle();				//�^�C�g���J��
	else UpdateIsGameOver();								//���j���[���

	for (Object3d*& player : objPlayers_)
	{
		//���v���Z�b�g
		const ParticleManager::Preset smoke =
		{
			particle1_,
			player->GetPosition(),
			{ 0.0f ,2.0f,0.0f },
			{ 0.3f,3.0f,0.3f },
			{ 0.0f,0.001f,0.0f },
			3,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};

		if (!completeRotate_)
		{
			if (!isQuitStageSelect_)
			{
				pm1_->ActiveY(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
					smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);
			}
			//�v���C���[��]�p
			DirectX::XMFLOAT3 rotPlayer = player->GetRotation();

			const float rotSpeed = -0.3f;
			rotPlayer.y += rotSpeed;
			player->SetRotation(rotPlayer);
		}
		else
		{
			pm1_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, { -smoke.vel.y,smoke.vel.x,smoke.vel.z },
				smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);
		}
		player->Update();
	}
	for (Object3d*& stage : objStages_)
	{
		//�V����]�p
		DirectX::XMFLOAT3 rotStage = stage->GetRotation();

		const float rotSpeed = -0.2f;
		rotStage.y += rotSpeed;
		stage->SetRotation(rotStage);

		stage->Update();
	}
	for (Object3d*& goal : objGoals_)goal->Update();



	spriteGameOver_->Update();
	spriteContinue_->Update();
	spriteStageSelect_->Update();
	spriteTitle_->Update();
	spriteDone_->Update();
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();

	camera_->Update();
	lightGroup_->Update();
	pm1_->Update();

	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(false);
#endif // DEBUG
	imguiManager_->End();
}

void GameOverScene::UpdateIsGameOver()
{
	//�I�𒆂̃��j���[�J���[
	const DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,1.0f };
	//����w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 doneColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	UpdateChangeColor();

	//�C�[�W���O
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();

	//���W,�J���[�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].num_X,menuPosY_[GOMEN_SelectSpace] });
	spriteFadeInOut_->SetColor({ deepRed_.x,deepRed_.y, deepRed_.z, easeFadeInOut_.num_X });//�����x�����ς���

	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	if (menuCount_ == GOSMI_Continue)
	{
		spriteContinue_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == GOSMI_StageSelect)
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == GOSMI_Title)
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (menuCount_ == GOSMI_Continue)
		{
			for (Object3d*& player : objPlayers_)
			{
				easePlayerRotateContinue_[1].SetEasing(player->GetRotation().y,
					easePlayerRotateContinue_[1].end,
					easePlayerRotateContinue_[1].maxtime);
			}
			for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeEyeContinue_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetContinue_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateContinue_[i].Standby(false);
			easeContinuePosX_.Standby(false);
			easeContinuePosY_.Standby(false);
			//�R���e�B�j���[
			isContinue_ = true;
			isGameover_ = false;

		}
		else if (menuCount_ == GOSMI_StageSelect)
		{
			for (Object3d*& player : objPlayers_)
			{
				easePlayerRotateQuitStageSelect_[1].SetEasing(player->GetRotation().y,
					easePlayerRotateQuitStageSelect_[1].end,
					easePlayerRotateQuitStageSelect_[1].maxtime);
			}
			for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeEyeQuitStageSelect_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetQuitStageSelect_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateQuitStageSelect_[i].Standby(false);
			//�X�e�[�W�Z���N�g��
			isQuitStageSelect_ = true;
			isGameover_ = false;
		}
		else if (menuCount_ == GOSMI_Title)
		{
			for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].Standby(true);
			//�^�C�g����
			isQuitTitle_ = true;
			isGameover_ = false;

		}
	}

	spriteGameOver_->SetColor(doneColor);
	spriteDone_->SetColor(doneColor);

}

void GameOverScene::UpdateIsContinue()
{
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeContinue_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetContinue_[i].ease_in_out_expo();
	easeContinuePosX_.ease_in_out_expo();
	easeContinuePosY_.ease_in_out_expo();
	//���W�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu]});
	spriteContinue_->SetPosition({ easeContinuePosX_.num_X,easeContinuePosY_.num_X });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect]});
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title]});
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].num_X,menuPosY_[GOMEN_SelectSpace]});

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeContinue_[XYZ_X].num_X, easeEyeContinue_[XYZ_Y].num_X, easeEyeContinue_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetContinue_[XYZ_X].num_X, easeTargetContinue_[XYZ_Y].num_X, easeTargetContinue_[XYZ_Z].num_X });

	for (Object3d*& player : objPlayers_)
	{
		//��]
		if (!completeRotate_)
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateContinue_[i].ease_in_expo();
			player->SetRotation({ easePlayerRotateContinue_[XYZ_X].num_X,easePlayerRotateContinue_[XYZ_Y].num_X,
				easePlayerRotateContinue_[XYZ_Z].num_X });
			
			//��]���I�������
			if (player->GetRotation().x == easePlayerRotateContinue_[0].end)
			{
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveContinue_[i].Standby(false);
				completeRotate_ = true;
			}
		}
		else
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerMoveContinue_[i].ease_in_back();
			player->SetPosition({ easePlayerMoveContinue_[XYZ_X].num_X,easePlayerMoveContinue_[XYZ_Y].num_X,
				easePlayerMoveContinue_[XYZ_Z].num_X });
		}

		if (spriteDone_->GetPosition().x == easeMenuPosX_[GOMEN_SelectSpace].start)
		{
			FadeOut(white_);//��������
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				sceneManager_->ChangeScene("GAMEPLAY", stageNum_);
			}

		}
	}
}

void GameOverScene::UpdateIsQuitStageSelect()
{
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeQuitStageSelect_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetQuitStageSelect_[i].ease_in_out_expo();

	//���W�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu]});
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue]});
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect]});
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title]});
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].num_X,menuPosY_[GOMEN_SelectSpace]});

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeQuitStageSelect_[XYZ_X].num_X, easeEyeQuitStageSelect_[XYZ_Y].num_X, easeEyeQuitStageSelect_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetQuitStageSelect_[XYZ_X].num_X, easeTargetQuitStageSelect_[XYZ_Y].num_X, easeTargetQuitStageSelect_[XYZ_Z].num_X });

	for (Object3d*& player : objPlayers_)
	{
		//��]
		if (!completeRotate_)
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateQuitStageSelect_[i].ease_in_expo();
			player->SetRotation({ easePlayerRotateQuitStageSelect_[XYZ_X].num_X,easePlayerRotateQuitStageSelect_[XYZ_Y].num_X ,easePlayerRotateQuitStageSelect_[XYZ_Z].num_X });
			//��]���I�������
			if (player->GetRotation().x == easePlayerRotateQuitStageSelect_[XYZ_X].end)
			{
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveQuitStageSelect_[i].Standby(false);
				completeRotate_ = true;
			}
		}
		else
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerMoveQuitStageSelect_[i].ease_in_quint();
			player->SetPosition({ easePlayerMoveQuitStageSelect_[XYZ_X].num_X,easePlayerMoveQuitStageSelect_[XYZ_Y].num_X,easePlayerMoveQuitStageSelect_[XYZ_Z].num_X });
		}

		if (spriteDone_->GetPosition().x == easeMenuPosX_[GOMEN_SelectSpace].start)
		{
			FadeOut(black_);//��������
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
				else if (stageNum_ <= SL_StageTutorial_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);

			}

		}
	}
}

void GameOverScene::UpdateIsQuitTitle()
{
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();

	//���W�Z�b�g
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].num_X,menuPosY_[GOMEN_SelectSpace] });

	for (Object3d*& player : objPlayers_)
	{
		DirectX::XMFLOAT3 move = player->GetPosition();
		const DirectX::XMFLOAT3 speed = { 0.0f,-1.0f,0.0f };

		move.y += speed.y;
		player->SetPosition(move);

		if (spriteGameOver_->GetPosition().x == easeMenuPosX_[GOMEN_Menu].start)
		{
			FadeOut(black_);//��������
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				sceneManager_->ChangeScene("TITLE", stageNum_);
			}
		}
	}
}

void GameOverScene::Draw()
{
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& stage : objStages_)stage->Draw();
	if (!isQuitStageSelect_)for (Object3d*& goal : objGoals_)goal->Draw();

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

	//�O�i�X�v���C�g
	spCommon_->PreDraw();
	//�X�v���C�g�`��	
	spriteGameOver_->Draw();
	spriteContinue_->Draw();
	spriteStageSelect_->Draw();
	spriteTitle_->Draw();
	spriteDone_->Draw();
	spriteFadeInOut_->Draw();
	spriteLoad_->Draw();
	spriteStageInfoNow_->Draw();
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
	delete spriteFadeInOut_;
	delete spriteLoad_;
	delete spriteStageInfoNow_;

	//���f��
	//���x���f�[�^�p�I�u�W�F�N�g
	for (Object3d*& player : objPlayers_)delete player;
	for (Object3d*& stage : objStages_)delete stage;
	for (Object3d*& goal : objGoals_)delete goal;

	delete modelPlayer_;
	delete modelGoal_;
	delete modelStageTutorial_;
	delete modelStage1_;
	delete modelStage2_;

	//�J����
	delete camera_;
	//���C�g
	delete lightGroup_;
	//�p�[�e�B�N��
	delete particle1_;
	delete pm1_;
}

void GameOverScene::LoadLVData([[maybe_unused]] const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);

	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player", true);
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelStageTutorial_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");

	models_.insert(std::make_pair("player", modelPlayer_));
	models_.insert(std::make_pair("sphere", modelGoal_));
	models_.insert(std::make_pair("skydomet", modelStageTutorial_));
	models_.insert(std::make_pair("skydome", modelStage1_));
	models_.insert(std::make_pair("skydome2", modelStage2_));

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
			objStages_.push_back(newObject);
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

void GameOverScene::FadeOut(DirectX::XMFLOAT3 rgb)
{
	if (!isFadeOut_)
	{
		easeFadeInOut_.Standby(true);
		isFadeOut_ = true;
	}
	else
	{
		const XMFLOAT3 negapozi = { 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//�����x�����ς���
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�l�K�|�W�̉��p
		if (isContinue_)spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });

	}
}

void GameOverScene::UpdateChangeColor()
{
	//�F��ς���X�s�[�h
	float speedColor = 0.02f;

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
