#include "GamePlayScene.h"
#include "LevelLoaderJson.h"
#include "Player.h"
#include "CollisionManager.h"
#include "MeshCollider.h"
#include "TouchableObject.h"
#include <typeinfo>

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

DirectXCommon* GamePlayScene::dxCommon_ = DirectXCommon::GetInstance();
SpriteCommon* GamePlayScene::spCommon_ = SpriteCommon::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
Audio* GamePlayScene::audio_ = Audio::GetInstance();
Camera* GamePlayScene::camera_ = Camera::GetInstance();
SceneManager* GamePlayScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GamePlayScene::imguiManager_ = ImGuiManager::GetInstance();

void GamePlayScene::Initialize()
{
	colManager_ = CollisionManager::GetInstance();
	// �`�揉���������@��������
#pragma region �`�揉��������
	//�����f�[�^
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//�����Đ��Ăяo����
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	//OBJ�t�@�C�����烂�f���f�[�^��ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");
	modelGoal_ = Model::LoadFromOBJ("sphere");

	//�v���C���[�̏�����
	player_ = Player::Create(modelPlayer_);
	player_->SetCamera(camera_);
	player_->Update();

	//�S�[��������
	goal_ = Goal::Create(modelGoal_);
	goal_->SetCamera(camera_);
	goal_->Update();

	//�G������
	enemy_ = Enemy::Create(modelEnemy_);
	enemy_->SetCamera(camera_);
	enemy_->Update();

	//���x���f�[�^�ǂݍ���
	LoadLVData();

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	//�p�[�e�B�N��
	particle1_ = Particle::LoadFromParticleTexture("particle6.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	particle2_ = Particle::LoadFromParticleTexture("particle5.png");
	pm2_ = ParticleManager::Create();
	pm2_->SetParticleModel(particle2_);
	pm2_->SetCamera(camera_);

	//�G�Ɏ��@�̃A�h���X��n��
	enemy_->SetPlayer(player_);

}

void GamePlayScene::Update()
{
	//���f���Ăяo����
	player_->Update();
	enemy_->Update();
	goal_->Update();

	for (auto& object : objects) {
		object->Update();
	}

	lightGroup_->SetPointLightPos(0, player_->GetWorldPosition());
	////�����蔻��T���v��
	//Ray ray;
	//ray.start = { 10.0f,-10.0f,-60.0f,1.0f };
	//ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	//RaycastHit rcHit;

	//if (colManager_->RayCast(ray,&rcHit))
	//{
	//	pm1_->ActiveZ(particle1_, XMFLOAT3(rcHit.inter.m128_f32), XMFLOAT3(rcHit.inter.m128_f32), XMFLOAT3({ 0.1f,0.1f,0.1f }),
	//		XMFLOAT3(), 2, XMFLOAT2({ 3.0f,0.0f }));
	//}

	//�J����
	camera_->Update();
	lightGroup_->Update();
	pm1_->Update();
	pm2_->Update();

	if (input_->TriggerKey(DIK_RETURN))
	{
		camera_->Reset();
		sceneManager_->ChangeScene("TITLE");
	}


	if (goal_->IsGoal() || player_->GetPosition().y <= -60.0f)
	{
		camera_->Reset();
		sceneManager_->ChangeScene("TITLE");
	}
	colManager_->CheckAllCollisions();
}

void GamePlayScene::Draw()
{

	//�X�v���C�g�`��O����
	spCommon_->PreDraw();

	//�w�i�X�v���C�g

	//�G�t�F�N�g


	//���f��
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//���f���`��
	player_->Draw();
	enemy_->Draw();
	goal_->Draw();
	for (auto& object : objects) {
		object->Draw();
	}

	//���f���`��㏈��
	Object3d::PostDraw();
	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��
	pm1_->Draw();
	pm2_->Draw();
	player_->DrawParticle();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();
	//�O�i�X�v���C�g

	//ImGui�̕\��
}

void GamePlayScene::Finalize()
{
	//�I������
	audio_->Finalize();
	//���
	//�e�퉹��
	audio_->SoundUnLoad(&sound);

	//�p�[�e�B�N��
	delete particle1_;
	delete pm1_;
	delete particle2_;
	delete pm2_;
	//���C�g
	delete lightGroup_;
	//���f��

	for (Object3d*& object : objects)
	{
		delete object;
	}
	objects.clear();

	//3D���f��
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelSkydome;
	delete modelGround;
	delete modelBox;
	delete modelGoal_;
	//��Ռn
	delete player_;
	delete enemy_;
	delete goal_;

}

void GamePlayScene::LoadLVData()
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile("stage1");

	// ���f���ǂݍ���
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelBox = Model::LoadFromOBJ("sphere2");

	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("sphere2", modelBox));

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objectData : levelData->objects) {

		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		TouchableObject* newObject = TouchableObject::Create(model);
		

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
		newObject->Update();
		// �z��ɓo�^
		objects.push_back(newObject);
	}

}
