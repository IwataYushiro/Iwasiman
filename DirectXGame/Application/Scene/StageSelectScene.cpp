#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include "StageList.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

//�X�e�[�W�Z���N�g�V�[��

//�ÓI�����o�ϐ��̎���
DirectXCommon* StageSelectScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageSelectScene::input_ = Input::GetInstance();
Audio* StageSelectScene::audio_ = Audio::GetInstance();
SceneManager* StageSelectScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageSelectScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* StageSelectScene::camera_ = Camera::GetInstance();

StageSelectScene::StageSelectScene(int count) :menuCount_(count)
{
}

void StageSelectScene::Initialize()
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
	camera_->SetEye({ easeEyeStageSelect_[0].start, easeEyeStageSelect_[1].start, easeEyeStageSelect_[2].start });
	// �����_���W
	camera_->SetTarget({ easeTargetStageSelect_[0].start, easeTargetStageSelect_[1].start, easeTargetStageSelect_[2].start });

	//���x���f�[�^�ǂݍ���
	LoadLVData("scene/stageselect");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(SSSTI_MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, SSSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[0].start,0.0f });

	spCommon_->LoadTexture(SSSTI_MenuTutorialTex, "texture/titlemenut.png");
	spriteTutorial_->Initialize(spCommon_, SSSTI_MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].start,150.0f });

	spCommon_->LoadTexture(SSSTI_Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, SSSTI_Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX_[2].start,300.0f });

	spCommon_->LoadTexture(SSSTI_Menustage2Tex, "texture/stagetower.png");
	spriteStage2_->Initialize(spCommon_, SSSTI_Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX_[3].start,450.0f });

	spCommon_->LoadTexture(SSSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, SSSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[4].start,580.0f });

	spCommon_->LoadTexture(SSSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, SSSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[5].start,50.0f });
	spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });

	spCommon_->LoadTexture(SSSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SSSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ 0.0f,0.0f, 0.0f, easeFadeInOut_.start });

	modelStageTutorial_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");

	objStage_ = Object3d::Create();
	objStage_->SetModel(modelStageTutorial_);
	objStage_->SetCamera(camera_);
	objStage_->SetScale({ 7.0f,7.0f,7.0f });

	particle1_ = Particle::LoadFromParticleTexture("particle1.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	//�C�[�W���OStandby
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(false);
	for (int i = 0; i < 3; i++)easeEyeStageSelect_[i].Standby(false);
	for (int i = 0; i < 3; i++)easeTargetStageSelect_[i].Standby(false);
	easeFadeInOut_.Standby(false);

}

void StageSelectScene::Update()
{
	if (menuCount_ <= SSSMI_StageTutorial_Tutorial)menuCount_ = SSSMI_StageTutorial_Tutorial;
	else if (menuCount_ >= SSSMI_Stage2_TowerStage)menuCount_ = SSSMI_Stage2_TowerStage;

	if (isStageSelect_)UpdateIsStageSelect();
	else if (isDone_)UpdateIsDone();
	else if (isStart_) UpdateIsGameStart();
	else UpdateIsQuitTitle();

	for (Object3d*& player : objPlayers_)
	{
		pm1_->ActiveX(particle1_, player->GetPosition(), { 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f }, { 0.0f,0.001f,0.0f }, 3, { 1.0f, 0.0f });

		player->Update();
	}
	for (Object3d*& ground : objGrounds_)ground->Update();
	for (Object3d*& goal : objGoals_)goal->Update();


	rot_.y += 0.5f;

	objStage_->SetRotation(rot_);

	spriteMenu_->Update();
	spriteTutorial_->Update();
	spriteStage1_->Update();
	spriteStage2_->Update();
	spriteDone_->Update();
	spriteBack_->Update();
	spriteFadeInOut_->Update();

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
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeStageSelect_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetStageSelect_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });

	//�J���[�Z�b�g
	spriteFadeInOut_->SetColor({ 0.0f,0.0f, 0.0f, easeFadeInOut_.num_X });//�����x�����ς���

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeStageSelect_[0].num_X, easeEyeStageSelect_[1].num_X, easeEyeStageSelect_[2].num_X });
	camera_->SetTarget({ easeTargetStageSelect_[0].num_X, easeTargetStageSelect_[1].num_X, easeTargetStageSelect_[2].num_X });

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

	if (menuCount_ == SSSMI_StageTutorial_Tutorial)
	{
		objStage_->SetModel(modelStageTutorial_);
		spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteTutorial_->SetColor({ 0.1f + speedColor_,0.1f,0.1f,1.0f });
		spriteStage1_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
	}
	else if (menuCount_ == SSSMI_Stage1_SkyStage)
	{
		objStage_->SetModel(modelStage1_);
		spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteTutorial_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteStage1_->SetColor({ 0.1f + speedColor_,0.1f,0.1f,1.0f });
		spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
	}
	else if (menuCount_ == SSSMI_Stage2_TowerStage)
	{
		objStage_->SetModel(modelStage2_);
		spriteMenu_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteTutorial_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStage1_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteStage2_->SetColor({ 1.0f,speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f });
		spriteDone_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		spriteBack_->SetColor({ 0.5f,0.5f,1.0f,1.0f });
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(true);
		for (int i = 0; i < 3; i++)easeEyeDoneMenu_[i].Standby(false);
		for (int i = 0; i < 3; i++)easeTargetDoneMenu_[i].Standby(false);
		isDone_ = true;
		isStageSelect_ = false;
	}
	if (easeMenuPosX_[5].num_X == easeMenuPosX_[5].end)
	{
		if (input_->TriggerKey(DIK_Q))
		{
			for (int i = 0; i < 3; i++)easePlayerQuitMove_[i].Standby(false);
			for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(true);
			outStageSelect_ = true;
			isStageSelect_ = false;
		}
	}
}

void StageSelectScene::UpdateIsDone()
{
	//�C�[�W���O
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeDoneMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetDoneMenu_[i].ease_out_expo();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeDoneMenu_[0].num_X, easeEyeDoneMenu_[1].num_X, easeEyeDoneMenu_[2].num_X });
	camera_->SetTarget({ easeTargetDoneMenu_[0].num_X, easeTargetDoneMenu_[1].num_X, easeTargetDoneMenu_[2].num_X });

	//�C�[�W���O���I�������
	if (camera_->GetEye().x == easeEyeDoneMenu_[0].end)
	{
		for (int i = 0; i < 3; i++)easePlayerStartMove_[i].Standby(false);
		for (int i = 0; i < 3; i++)easeEyeGameStart_[i].Standby(false);
		for (int i = 0; i < 3; i++)easeTargetGameStart_[i].Standby(false);
		isStart_ = true;
		isDone_ = false;
	}
}

void StageSelectScene::UpdateIsGameStart()
{
	//�v���C���[��X�l�������܂ŗ�����t�F�[�h�A�E�g
	const float gameStartPosX_ = 200.0f;
	//�C�[�W���O
	for (int i = 0; i < 3; i++)easePlayerStartMove_[i].ease_in_out_expo();
	for (int i = 0; i < 3; i++)easeEyeGameStart_[i].ease_in_quint();
	for (int i = 0; i < 3; i++)easeTargetGameStart_[i].ease_in_quint();

	//�J�����Z�b�g
	camera_->SetEye({ easeEyeGameStart_[0].num_X, easeEyeGameStart_[1].num_X, easeEyeGameStart_[2].num_X });
	camera_->SetTarget({ easeTargetGameStart_[0].num_X, easeTargetGameStart_[1].num_X, easeTargetGameStart_[2].num_X });

	//�v���C���[���W���Z�b�g
	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerStartMove_[0].num_X,easePlayerStartMove_[1].num_X ,easePlayerStartMove_[2].num_X });;
		if (player->GetPosition().x >= gameStartPosX_)FadeOut({ 1.0f,1.0f,1.0f });//�Q�[���v���C�J�ڎ��͔�������
	}
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//�`���[�g���A���X�e�[�W
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);//�X�e�[�W1
		else if (menuCount_ == SSSMI_Stage2_TowerStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_AreaBoss);//�X�e�[�W2�͂܂�������

	}
}

void StageSelectScene::UpdateIsQuitTitle()
{
	//�C�[�W���O
	for (int i = 0; i < 3; i++)easePlayerQuitMove_[i].ease_in_expo();
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();

	//���W�Z�b�g
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });
	//���j���[�W�L�̃C�[�W���O���I�������t�F�[�h�A�E�g
	if (spriteMenu_->GetPosition().x == easeMenuPosX_[0].start) FadeOut({ 0.0f,0.0f,0.0f });//��������

	//�v���C���[���W���Z�b�g
	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerQuitMove_[0].num_X,easePlayerQuitMove_[1].num_X ,easePlayerQuitMove_[2].num_X });
	}
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("TITLE", SL_StageTutorial_StageID);
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("TITLE", SL_Stage1_StageID);
		else if (menuCount_ == SSSMI_Stage2_TowerStage)sceneManager_->ChangeScene("TITLE", SL_Stage2_StageID);
	}
}

void StageSelectScene::Draw()
{

	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& ground : objGrounds_)ground->Draw();
	for (Object3d*& goal : objGoals_)goal->Draw();
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
}

void StageSelectScene::Finalize()
{
	//����
	audio_->Finalize();
	//sprite
	delete spriteMenu_;
	delete spriteTutorial_;
	delete spriteStage1_;
	delete spriteStage2_;
	delete spriteDone_;
	delete spriteBack_;
	delete spriteFadeInOut_;

	//�X�e�[�W
	delete objStage_;
	//���x���f�[�^�p�I�u�W�F�N�g
	for (Object3d*& player : objPlayers_)delete player;
	for (Object3d*& ground : objGrounds_)delete ground;
	for (Object3d*& goal : objGoals_)delete goal;

	delete modelStageTutorial_;
	delete modelStage1_;
	delete modelStage2_;
	delete modelPlayer_;
	delete modelGround_;
	delete modelGoal_;
	//�p�[�e�B�N��
	delete particle1_;
	delete pm1_;

	//���C�g
	delete lightGroup_;
}

void StageSelectScene::FadeOut(DirectX::XMFLOAT3 rgb)
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

	}
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);

	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player", true);
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelGround_ = Model::LoadFromOBJ("ground");

	models_.insert(std::make_pair("player", modelPlayer_));
	models_.insert(std::make_pair("sphere", modelGoal_));
	models_.insert(std::make_pair("ground", modelGround_));

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
