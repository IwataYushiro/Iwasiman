#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

DirectXCommon* StageSelectScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageSelectScene::input_ = Input::GetInstance();
Audio* StageSelectScene::audio_ = Audio::GetInstance();
SceneManager* StageSelectScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageSelectScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* StageSelectScene::camera_ = Camera::GetInstance();

StageSelectScene::StageSelectScene()
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
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// �����_���W
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//���x���f�[�^�ǂݍ���
	//LoadLVData("enemytest");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT MenuTex = 00;
	spCommon_->LoadTexture(MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });

	UINT MenuTutorialTex = 01;
	spCommon_->LoadTexture(MenuTutorialTex, "texture/titlemenut.png");
	spriteTutorial_->Initialize(spCommon_, MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX[1].start,150.0f });

	UINT Menustage1Tex = 02;
	spCommon_->LoadTexture(Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX[2].start,300.0f });

	UINT Menustage2Tex = 03;
	spCommon_->LoadTexture(Menustage2Tex, "texture/stagetower.png");
	spriteStage2_->Initialize(spCommon_, Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX[3].start,450.0f });

	UINT MenuDoneTex = 04;
	spCommon_->LoadTexture(MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX[4].start,580.0f });

	UINT BackTex = 05;
	spCommon_->LoadTexture(BackTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, BackTex);
	spriteBack_->SetPosition({ easeMenuPosX[5].start,50.0f });
	spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });

	modelStageTutorial = Model::LoadFromOBJ("skydomet");
	modelStage1 = Model::LoadFromOBJ("skydome");
	modelStage2 = Model::LoadFromOBJ("skydome2");
	
	objStage = Object3d::Create();
	objStage->SetModel(modelStageTutorial);
	objStage->SetCamera(camera_);
	objStage->SetScale({ 7.0f,7.0f,7.0f });

	//FBX
	//objF = ObjectFbx::Create();
	//modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	//objF->SetModelFBX(modelF);
	//objF->SetCamera(camera_);
	//objF->PlayAnimation();//�X�V�ŌĂԂƎ~�܂邩�璍��

	//�p�[�e�B�N��
	/*particle1_ = Particle::LoadFromParticleTexture("particle2.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);*/

	easeTitlePosX.Standby(false);
	for (int i = 0; i < 6; i++)easeMenuPosX[i].Standby(false);
}

void StageSelectScene::Update()
{
	if (MenuCount <= 0)MenuCount = 0;
	else if (MenuCount >= 2)MenuCount = 2;
	if (!outStageSerect)
	{
		//�C�[�W���O
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 6; i++)easeMenuPosX[i].ease_out_expo();

		//���W�Z�b�g
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteStage1_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteStage2_->SetPosition({ easeMenuPosX[3].num_X,450.0f });
		spriteDone_->SetPosition({ easeMenuPosX[4].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[5].num_X,50.0f });

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))MenuCount--;
		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))MenuCount++;

		if (isColorReverse_)speedColor -= 0.02f;
		else speedColor += 0.02f;

		if (speedColor >= 0.9f)
		{
			isColorReverse_ = true;
		}
		if (speedColor <= 0.0f)
		{
			isColorReverse_ = false;
		}

		if (MenuCount == 0)
		{
			objStage->SetModel(modelStageTutorial);
			spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteTutorial_->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
			spriteStage1_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
		}
		else if (MenuCount == 1)
		{
			objStage->SetModel(modelStage1);
			spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteTutorial_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteStage1_->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
			spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
		}
		else if (MenuCount == 2)
		{
			objStage->SetModel(modelStage2);
			spriteMenu_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteTutorial_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteStage1_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteStage2_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
			spriteDone_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteBack_->SetColor({ 0.5f,0.5f,1.0f,1.0f });
		}

		if (input_->TriggerKey(DIK_SPACE))
		{
			if (MenuCount == 0)
			{
				//�`���[�g���A���X�e�[�W
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 1);
			}
			else if (MenuCount == 1)
			{
				//�X�e�[�W1
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 3);

			}
			else if (MenuCount == 2)
			{
				//�X�e�[�W2
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 4);

			}
		}
		if (easeMenuPosX[5].num_X == easeMenuPosX[5].end)
		{
			if (input_->TriggerKey(DIK_Q))
			{
				easeTitlePosX.Standby(true);
				for (int i = 0; i < 6; i++)easeMenuPosX[i].Standby(true);
				outStageSerect = true;
			}
		}
	}
	else
	{
		//�C�[�W���O
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 6; i++)easeMenuPosX[i].ease_out_expo();

		//���W�Z�b�g
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteStage1_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteStage2_->SetPosition({ easeMenuPosX[3].num_X,450.0f });
		spriteDone_->SetPosition({ easeMenuPosX[4].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[5].num_X,50.0f });

		if (easeMenuPosX[5].num_X == easeMenuPosX[5].start)
		{
			sceneManager_->ChangeScene("TITLE");
		}
	}
	rot.y += 0.5f;

	objStage->SetRotation(rot);

	spriteMenu_->Update();
	spriteTutorial_->Update();
	spriteStage1_->Update();
	spriteStage2_->Update();
	spriteDone_->Update();
	spriteBack_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage->Update();
}

void StageSelectScene::Draw()
{

	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	
	objStage->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	//�X�v���C�g�`��	
	spriteMenu_->Draw();
	spriteTutorial_->Draw();
	spriteStage1_->Draw();
	spriteStage2_->Draw();
	spriteDone_->Draw();
	spriteBack_->Draw();
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
	//�X�e�[�W
	delete objStage;

	delete modelStageTutorial;
	delete modelStage1;
	delete modelStage2;
	
	//���C�g
	delete lightGroup_;
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile(stagePath);
	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (LevelData::ObjectData& objectData : levelData->objects) {

		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}
		
		//�n�`
		// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		TouchableObject* newObject = TouchableObject::Create(model, false);
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
		objects.push_back(newObject);
		

	}

}
