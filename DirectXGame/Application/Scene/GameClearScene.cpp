#include "GameClearScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "TouchableObject.h"

using namespace DirectX;

DirectXCommon* GameClearScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameClearScene::input_ = Input::GetInstance();
Audio* GameClearScene::audio_ = Audio::GetInstance();
SceneManager* GameClearScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameClearScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* GameClearScene::camera_ = Camera::GetInstance();


void GameClearScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//�I�[�f�B�I
	audio_->Initialize();

	
	// ���_���W
	camera_->SetEye({ 0.0f, 6.0f, -365.0f });
	// �����_���W
	camera_->SetTarget({ 0.0f,6.0f,-260.0f });

	LoadModel();
	//���x���f�[�^�ǂݍ���
	LoadLVData("clear");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT StageTex = 00;
	spCommon_->LoadTexture(StageTex, "texture/gameclear.png");
	spriteGameClear_->Initialize(spCommon_, StageTex);
	//��
BGM = audio_->SoundLoadWave("Resources/sound/bgm/ending.wav");
	doneSE = audio_->SoundLoadWave("Resources/sound/se/done.wav");

	audio_->SoundPlayWave(audio_->GetXAudio2(), BGM, false);

}

void GameClearScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		audio_->SoundPlayWave(audio_->GetXAudio2(), doneSE, false);
		sceneManager_->ChangeScene("TITLE");
		
	}
	for (std::unique_ptr<Earth>& earth : earths_)earth->Update();

	for (Object3d*& object : objects) object->Update();

	spriteGameClear_->Update();
	camera_->Update();
	lightGroup_->Update();

}

void GameClearScene::Draw()
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
	spriteGameClear_->Draw();



}

void GameClearScene::Finalize()
{
	//����
	audio_->Finalize();
	audio_->SoundUnLoad(&BGM);
	audio_->SoundUnLoad(&doneSE);

	for (Object3d*& object : objects)delete object;
	objects.clear();
	delete modelSkydome;
	delete modelEarth_;
	models.clear();

	delete lightGroup_;
	//�X�v���C�g
	delete spriteGameClear_;

}

void GameClearScene::LoadLVData(const std::string& stagePath)
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

void GameClearScene::LoadModel()
{
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelEarth_ = Model::LoadFromOBJ("earth");
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("earth", modelEarth_));

}


