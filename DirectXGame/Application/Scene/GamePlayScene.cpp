#include "GamePlayScene.h"
#include "LevelLoaderJson.h"
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
	//�v���C���[�֌W
	player_ = new Player();
	//�G�֌W
	enemy_ = new Enemy();

	goal_ = new Goal();

	// �`�揉���������@��������
#pragma region �`�揉��������
	//�����f�[�^
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//�����Đ��Ăяo����
	audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	//3D�I�u�W�F�N�g�֌W
	//3D�I�u�W�F�N�g����
	object3DPlayer_ = Object3d::Create();
	object3DEnemy_ = Object3d::Create();
	objGoal_ = Object3d::Create();

	//OBJ�t�@�C�����烂�f���f�[�^��ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");
	modelGoal_ = Model::LoadFromOBJ("sphere");

	//�I�u�W�F�N�g�Ƀ��f���R�t����
	object3DPlayer_->SetModel(modelPlayer_);
	object3DEnemy_->SetModel(modelEnemy_);
	objGoal_->SetModel(modelGoal_);

	//�J�������R�Â�
	object3DPlayer_->SetCamera(camera_);
	object3DEnemy_->SetCamera(camera_);
	objGoal_->SetCamera(camera_);

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
	//�|�W�V����
	player_->Initialize(modelPlayer_, object3DPlayer_, input_, camera_);

	enemy_->Initialize(modelEnemy_, object3DEnemy_, camera_);

	goal_->Initialize(modelGoal_, objGoal_, camera_);
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
	ChackAllCollisions();
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
	
}

void GamePlayScene::Draw()
{

	//�X�v���C�g�`��O����
	spCommon_->PreDraw();

	//�w�i�X�v���C�g

	//�G�t�F�N�g
	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��
	pm1_->Draw();
	pm2_->Draw();
	player_->DrawParticle();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

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
	//3D�I�u�W�F�N�g
	delete object3DPlayer_;
	delete object3DEnemy_;
	delete objGoal_;

	for (Object3d*& object : objects)
	{
		delete object;
	}
	
	//3D���f��
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelSkydome;
	delete modelGround;
	delete modelGoal_;
	//��Ռn
	delete player_;
	delete enemy_;
	delete goal_;

}
//�Փ˔���Ɖ���
void GamePlayScene::ChackAllCollisions() {

	//����Ώ�A,B�̍��W
	XMFLOAT3 posA, posB;
	// A,B�̍��W�̋����p
	XMFLOAT3 posAB;
	//����Ώ�A,B�̔��a
	float radiusA;
	float radiusB;
	float radiiusAB;

	//���@�e���X�g���擾
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	//�G�e���X�g���擾
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetEnemyBullets();

#pragma region ���@�ƓG�e�̓����蔻��
	//���ꂼ��̔��a
	radiusA = 1.0f;
	radiusB = 1.0f;

	//���@�̍��W
	posA = player_->GetWorldPosition();

	//���@�ƑS�Ă̓G�e�̓����蔻��
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		//�G�e�̍��W
		posB = bullet->GetWorldPosition();
		//���WA,B�̋��������߂�
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//���Ƌ��̌�������
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			//���L�����̏Փˎ��R�[���o�b�N�֐����Ăяo��
			player_->OnCollision();
			//�G�e�̏Փˎ��R�[���o�b�N�֐����Ăяo��
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region ���e�ƓG�̓����蔻��
	//���ꂼ��̔��a
	radiusA = 5.0f;
	radiusB = 1.0f;

	//�G�̍��W
	posA = enemy_->GetWorldPosition();

	//�G�ƑS�Ă̒e�̓����蔻��
	for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
		//�e�̍��W
		posB = bullet->GetWorldPosition();
		//���WA,B�̋��������߂�
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//���Ƌ��̌�������
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			//�G�L�����̏Փˎ��R�[���o�b�N�֐����Ăяo��
			enemy_->OnCollisionPlayer();
			//���@�e�̏Փˎ��R�[���o�b�N�֐����Ăяo��
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region ���@�Ɖ��S�[���̓����蔻��
	//���ꂼ��̔��a
	radiusA = 3.0f;
	radiusB = 10.0f;

	//�G�̍��W
	posA = player_->GetWorldPosition();
	posB = goal_->GetWorldPosition();

	
		//���WA,B�̋��������߂�
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//���Ƌ��̌�������
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			camera_->Reset();
			sceneManager_->ChangeScene("TITLE");
		}
	
#pragma endregion

}

void GamePlayScene::LoadLVData()
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile("stage1");

	// ���f���ǂݍ���
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	
	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objectData : levelData->objects) {
		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

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
		DirectX::XMStoreFloat3(&scale,objectData.scale);
		newObject->SetScale(scale);

		newObject->SetCamera(camera_);
		// �z��ɓo�^
		objects.push_back(newObject);
	}

}
