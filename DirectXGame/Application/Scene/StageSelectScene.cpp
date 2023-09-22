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

	UINT MenuTex = 01;
	spCommon_->LoadTexture(MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });

	UINT MenuTutorialTex = 02;
	spCommon_->LoadTexture(MenuTutorialTex, "texture/titlemenut.png");
	spriteStage_->Initialize(spCommon_, MenuTutorialTex);
	spriteStage_->SetPosition({ easeMenuPosX[1].start,150.0f });

	UINT MenuDoneTex = 04;
	spCommon_->LoadTexture(MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX[3].start,550.0f });

	objStage = Object3d::Create();
	modelStage = Model::LoadFromOBJ("skydome");
	objStage->SetModel(modelStage);
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
}

void StageSelectScene::Update()
{
	if (MenuCount <= 0)MenuCount = 0;
	else if (MenuCount >= 1)MenuCount = 1;

	rot.y += 0.5f;

	objStage->SetRotation(rot);

	spriteMenu_->Update();
	spriteStage_->Update();
	spriteDone_->Update();

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
	spriteStage_->Draw();
	spriteDone_->Draw();
}

void StageSelectScene::Finalize()
{
	//����
	audio_->Finalize();
	//sprite
	delete spriteMenu_;
	delete spriteStage_;
	delete spriteDone_;
	//�X�e�[�W
	delete objStage;
	delete modelStage;
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
