#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "TouchableObject.h"

using namespace DirectX;

DirectXCommon* GameOverScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();
Audio* GameOverScene::audio_ = Audio::GetInstance();
SceneManager* GameOverScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameOverScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* GameOverScene::camera_ = Camera::GetInstance();


void GameOverScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//�I�[�f�B�I
	audio_->Initialize();

	XMFLOAT3 eye = camera_->GetEye();
	eye = { 0.0f, 6.0f, -365.0f };
	camera_->SetEye(eye);
	// �����_���W
	XMFLOAT3 target = camera_->GetEye();
	target = { 0.0f,6.0f,-260.0f };
	camera_->SetTarget(target);

	LoadModel();
	//���x���f�[�^�ǂݍ���
	LoadLVData("gameover");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT StageTex = 00;
	spCommon_->LoadTexture(StageTex, "texture/gameover.png");
	spriteGameOver_->Initialize(spCommon_, StageTex);

	BGM = audio_->SoundLoadWave("Resources/sound/bgm/gameover.wav");
	doneSE = audio_->SoundLoadWave("Resources/sound/se/done.wav");

	audio_->SoundPlayWave(audio_->GetXAudio2(), BGM, false);

}

void GameOverScene::Update()
{


	if (input_->TriggerKey(DIK_SPACE))
	{
		audio_->SoundPlayWave(audio_->GetXAudio2(), doneSE, false);
		sceneManager_->ChangeScene("TITLE");

	}


	for (std::unique_ptr<Earth>& earth : earths_)earth->Update();

	for (Object3d*& object : objects) object->Update();

	spriteGameOver_->Update();
	camera_->Update();
	lightGroup_->Update();

}

void GameOverScene::Draw()
{

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();


	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	for (std::unique_ptr<Earth>& earth : earths_)earth->Draw();
	for (auto& object : objects)object->Draw();
	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());


	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�O�i�X�v���C�g
//�w�i�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�X�v���C�g�`��
	spriteGameOver_->Draw();



}

void GameOverScene::Finalize()
{
	//����
	audio_->Finalize();
	audio_->SoundUnLoad(&BGM);
	audio_->SoundUnLoad(&doneSE);

	for (Object3d*& object : objects)delete object;
	objects.clear();
	delete modelEnemy1_;
	delete modelEnemy1Power_;
	delete modelEnemy1Guard_;
	delete modelEnemy1Speed_;
	delete modelEnemy1Death_;
	delete modelEnemy2_;
	delete modelEnemy2Power_;
	delete modelEnemy2Guard_;
	delete modelEnemy2Speed_;
	delete modelEnemy2Death_;
	delete modelSkydome;
	delete modelEarth_;
	models.clear();


	delete lightGroup_;
	//�X�v���C�g
	delete spriteGameOver_;

}

void GameOverScene::LoadLVData(const std::string& stagePath)
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

		if (objectData.objectType.find("EARTH") == 0)
		{
			//�S�[��������
			std::unique_ptr<Earth> newearth;
			newearth = Earth::Create(model);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newearth->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newearth->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newearth->SetScale(scale);

			newearth->SetCamera(camera_);
			newearth->Update();
			//���X�g�ɓo�^
			earths_.push_back(std::move(newearth));
		}
		//�n�`
		else
		{
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

}

void GameOverScene::LoadModel()
{
	modelEnemy1_ = Model::LoadFromOBJ("enemy1");
	modelEnemy1Power_ = Model::LoadFromOBJ("enemy1p");
	modelEnemy1Guard_ = Model::LoadFromOBJ("enemy1g");
	modelEnemy1Speed_ = Model::LoadFromOBJ("enemy1s");
	modelEnemy1Death_ = Model::LoadFromOBJ("enemy1d");
	modelEnemy2_ = Model::LoadFromOBJ("enemy2");
	modelEnemy2Power_ = Model::LoadFromOBJ("enemy2p");
	modelEnemy2Guard_ = Model::LoadFromOBJ("enemy2g");
	modelEnemy2Speed_ = Model::LoadFromOBJ("enemy2s");
	modelEnemy2Death_ = Model::LoadFromOBJ("enemy2d");
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelEarth_ = Model::LoadFromOBJ("earth2");

	models.insert(std::make_pair("enemy1", modelEnemy1_));
	models.insert(std::make_pair("enemy1p", modelEnemy1Power_));
	models.insert(std::make_pair("enemy1g", modelEnemy1Guard_));
	models.insert(std::make_pair("enemy1s", modelEnemy1Speed_));
	models.insert(std::make_pair("enemy1d", modelEnemy1Death_));
	models.insert(std::make_pair("enemy2", modelEnemy2_));
	models.insert(std::make_pair("enemy2p", modelEnemy2Power_));
	models.insert(std::make_pair("enemy2g", modelEnemy2Guard_));
	models.insert(std::make_pair("enemy2s", modelEnemy2Speed_));
	models.insert(std::make_pair("enemy2d", modelEnemy2Death_));
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("earth2", modelEarth_));

}


