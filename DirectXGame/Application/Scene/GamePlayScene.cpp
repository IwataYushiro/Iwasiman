#include "GamePlayScene.h"
#include "LevelLoaderJson.h"

#include "CollisionManager.h"
#include "MeshCollider.h"
#include "TouchableObject.h"
#include <typeinfo>

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

DirectXCommon* GamePlayScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
Audio* GamePlayScene::audio_ = Audio::GetInstance();
Camera* GamePlayScene::camera_ = Camera::GetInstance();
SceneManager* GamePlayScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GamePlayScene::imguiManager_ = ImGuiManager::GetInstance();

void GamePlayScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	colManager_ = CollisionManager::GetInstance();
	// �`�揉���������@��������
#pragma region �`�揉��������
	//�����f�[�^
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//�X�v���C�g
	LoadSprite();

	//�����Đ��Ăяo����
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	//�e���Z�b�g
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Reset();
	}

	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelBox = Model::LoadFromOBJ("sphere2", true);

	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("enemy1", modelEnemy_));
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("sphere2", modelBox));

	//���x���f�[�^�ǂݍ���
	LoadLVData("stage1");

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	//�p�[�e�B�N��
	particle1_ = Particle::LoadFromParticleTexture("particle6.png");
	particle2_ = Particle::LoadFromParticleTexture("particle5.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(particle1_);
	pm_->SetCamera(camera_);

	isPause_ = false;
}

void GamePlayScene::Update()
{
	//���S�t���O�̗������e���폜
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });
	enemys_.remove_if(
		[](std::unique_ptr<Enemy>& enemy) { return enemy->IsDead(); });

	//�G�X�V
	for (std::unique_ptr<Enemy>& enemy : enemys_) enemy->Update();
	//�e�X�V
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) bullet->Update();


	if (!isPause_)
	{
		//���f���Ăяo����
		for (std::unique_ptr<Player>& player : players_)
		{
			if (!isclear || !isGameover) player->Update();
			lightGroup_->SetPointLightPos(0, player->GetWorldPosition());
			//���߂��ׂ�
			if (player->GetPosition().y <= -60.0f || player->IsDead())
			{
				isGameover = true;
			}
		}
		for (std::unique_ptr<Goal>& goal : goals_)
		{
			goal->Update();
			//�N���A
			if (goal->IsGoal())
			{
				isclear = true;
			}
		}
		for (auto& object : objects) object->Update();
		
		//�J����
		camera_->Update();
		lightGroup_->Update();
		pm_->Update();

		if (isGameover)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				camera_->Reset();
				sceneManager_->ChangeScene("TITLE");
			}
		}
		if (isclear)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				camera_->Reset();
				sceneManager_->ChangeScene("TITLE");
			}
		}
		colManager_->CheckAllCollisions();

		//Pause�@�\
		if (input_->TriggerKey(DIK_Q) && !isclear && !isGameover)
		{
			//�����ŃC�[�W���O�̏���
			es.Standby(false);
			isBack = false;
			spritePause_->SetPosition({ es.start,0.0f });

			isPause_ = true;
		}
	}
	else if (isPause_)
	{
		//�C�[�W���O�T���v��(�|�[�Y���ɏ������Ă�����������Ă����)
		es.ease_in_out_elastic();
		spritePause_->SetPosition({ es.num_X,0.0f });

		if (input_->TriggerKey(DIK_W))
		{
			sceneManager_->ChangeScene("TITLE");
			isPause_ = false;
		}

		if (input_->TriggerKey(DIK_Q))
		{
			//�����ŃC�[�W���O�̏����B�������I�����W�ɓ��B���Ă��Ȃ��Ǝ󂯕t���Ȃ�
			if (spritePause_->GetPosition().x == es.end) es.Standby(true);
			isBack = true;
		}
		//���B������Pause����
		if (spritePause_->GetPosition().x == es.start)
		{
			if (isBack)isPause_ = false;
		}
	}
	spritePause_->Update();

}

void GamePlayScene::Draw()
{
	//���f��
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//���f���`��
	for (std::unique_ptr<Player>& player : players_)player->Draw();
	for (std::unique_ptr<Enemy>& enemy : enemys_) enemy->Draw();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();
	for (std::unique_ptr<Goal>& goal : goals_)goal->Draw();
	for (auto& object : objects)object->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��
	pm_->Draw();
	for (std::unique_ptr<Player>& player : players_)player->DrawParticle();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	//�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	if (isPause_)spritePause_->Draw();
	else if (isclear)spriteClear_->Draw();
	else if (isGameover)spriteGameover_->Draw();
	else spritePauseInfo_->Draw();
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
	delete particle2_;
	delete pm_;
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
	//�X�v���C�g
	delete spritePause_;
	delete spriteClear_;
	delete spritePauseInfo_;
	delete spriteGameover_;

}

void GamePlayScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile(stagePath);

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objectData : levelData->objects) {

		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		if (objectData.objectType.find("PLAYER") == 0)
		{
			//�v���C���[������
			std::unique_ptr<Player> newplayer;
			newplayer = Player::Create(modelPlayer_);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newplayer->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newplayer->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newplayer->SetScale(scale);

			newplayer->SetCamera(camera_);
			newplayer->Update();
			//���X�g�ɓo�^
			players_.push_back(std::move(newplayer));
		}
		else if (objectData.objectType.find("ENEMY") == 0)
		{
			//�G������
			std::unique_ptr<Enemy> newenemy;
			std::unique_ptr<Player>& player = players_.front();
			newenemy = Enemy::Create(modelEnemy_, player.get(), this);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newenemy->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newenemy->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newenemy->SetScale(scale);

			newenemy->SetCamera(camera_);
			newenemy->Update();
			//���X�g�ɓo�^
			enemys_.push_back(std::move(newenemy));
		}
		else if (objectData.objectType.find("GOAL") == 0)
		{
			//�G������
			std::unique_ptr<Goal> newgoal;
			newgoal = Goal::Create(modelGoal_);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newgoal->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newgoal->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newgoal->SetScale(scale);

			newgoal->SetCamera(camera_);
			newgoal->Update();
			//���X�g�ɓo�^
			goals_.push_back(std::move(newgoal));
		}
		else//�n�`
		{

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

}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	//���X�g�ɓo�^
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GamePlayScene::LoadSprite()
{
	//�X�v���C�g
	spCommon_->LoadTexture(10, "texture/pausep.png");
	spritePause_->Initialize(spCommon_, 10);
	spritePause_->SetColor({ 1.0f,1.0f,1.0f,0.5f });

	spCommon_->LoadTexture(11, "texture/gameclear.png");
	spriteClear_->Initialize(spCommon_, 11);

	spCommon_->LoadTexture(12, "texture/pauseinfo.png");
	spritePauseInfo_->Initialize(spCommon_, 12);

	spCommon_->LoadTexture(13, "texture/gameover.png");
	spriteGameover_->Initialize(spCommon_, 13);

	spritePause_->Update();
	spriteClear_->Update();
	spritePauseInfo_->Update();
	spriteGameover_->Update();
}
