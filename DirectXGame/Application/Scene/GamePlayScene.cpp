#include "GamePlayScene.h"
#include "LevelLoaderJson.h"

#include "CollisionManager.h"
#include "MeshCollider.h"
#include "TouchableObject.h"
#include "CollisionAttribute.h"

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


GamePlayScene::GamePlayScene(int stagenum) :stageNum(stagenum) {}

void GamePlayScene::Initialize()
{

	spCommon_ = SpriteCommon::GetInstance();
	colManager_ = CollisionManager::GetInstance();
	//�H�ƒn��
	enemyFactory = std::make_unique<EnemyFactory>();
	gimmickFactory = std::make_unique<GimmickFactory>();

	// �`�揉���������@��������
#pragma region �`�揉��������
	//�����f�[�^
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//�X�v���C�g
	LoadSprite();

	//�����Đ��Ăяo����
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	//�e���Z�b�g
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Reset();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Reset();


	//���f���ǂݍ���
	LoadModel();
	//���x���f�[�^�ǂݍ���
	if (stageNum == 1)LoadLVData("tstage_gimmick");
	else if (stageNum == 2)LoadLVData("stage2");
	else if (stageNum == 3)LoadLVData("stageboss1");

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
	playerBullets_.remove_if(
		[](std::unique_ptr<PlayerBullet>& pbullet) { return pbullet->IsDead(); });
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& ebullet) { return ebullet->IsDead(); });

	players_.remove_if(
		[](std::unique_ptr<Player>& player) {return player->IsDead(); });
	enemys_.remove_if(
		[](std::unique_ptr<BaseEnemy>& enemy) {return enemy->IsDead(); });

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
			if (player->IsDead())isGameover = true;

			//ImGui	
			imguiManager_->Begin();
			int plife[1] = { player->GetLife() };
			ImGui::Begin("Player");
			ImGui::SetWindowPos(ImVec2(200.0f, 200.0f));
			ImGui::SetWindowSize(ImVec2(150.0f, 50.0f));
			ImGui::InputInt("plife", plife);
			ImGui::End();
			imguiManager_->End();
		}
		//�e�X�V
		for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_) bullet->Update();

		for (std::unique_ptr<BaseEnemy>& enemy : enemys_)
		{
			enemy->Update();
			//�{�X���j
			if (enemy->BossDead())isclear = true;
		}
		
		for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();
		
		for (std::unique_ptr<Goal>& goal : goals_)
		{
			goal->Update();
			//�N���A
			if (goal->IsGoal()) isclear = true;
		}
		for (std::unique_ptr<Item>& item : items_)
		{
			item->Update();
		}


		for (Object3d*& object : objects) object->Update();

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
			sceneManager_->ChangeScene("STAGECLEAR", stageNum);
			isclear = false;
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
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Draw();
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->Draw();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();
	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_) gimmick->Draw();
	for (std::unique_ptr<Goal>& goal : goals_)goal->Draw();
	for (std::unique_ptr<Item>& item : items_)item->Draw();
	for (auto& object : objects)object->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��
	pm_->Draw();
	for (std::unique_ptr<Player>& player : players_)player->DrawParticle();
	for (std::unique_ptr<Item>& item : items_)item->DrawParticle();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	//�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	if (isPause_)spritePause_->Draw();
	else if (isclear)spriteClear_->Draw();
	else if (isGameover)spriteGameover_->Draw();
	else
	{
		spritePauseInfo_->Draw();
		for (std::unique_ptr<Item>& item : items_)
		{
			item->DrawSprite();
		}
	}
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

	for (Object3d*& object : objects)delete object;
	objects.clear();

	//3D���f��
	delete modelPlayer_;
	delete modelEnemy1_;
	delete modelBoss1_;
	delete modelBossCore1_;
	delete modelSkydome;
	delete modelItemJump_;
	delete modelItemHeal_;
	delete modelGround;
	delete modelBox;
	delete modelGoal_;

	models.clear();

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
	for (LevelData::ObjectData& objectData : levelData->objects) {

		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}
		//�v���C���[
		if (objectData.objectType.find("PLAYER") == 0)
		{
			//�v���C���[������
			std::unique_ptr<Player> newplayer;

			newplayer = Player::Create(model, this);
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
		//�G
		else if (objectData.objectType.find("ENEMY") == 0)
		{
			//�G������
			std::unique_ptr<BaseEnemy> newenemy;
			std::unique_ptr<Player>& player = players_.front();

			newenemy = enemyFactory->CreateEnemy(objectData.objectPattern,
				model, player.get(), this);

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
		//�d�|��
		else if (objectData.objectType.find("GIMMICK") == 0)
		{
			//�G������
			std::unique_ptr<BaseGimmick> newGimmick;
			std::unique_ptr<Player>& player = players_.front();

			newGimmick = gimmickFactory->CreateGimmick(objectData.objectPattern, model, player.get());

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newGimmick->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newGimmick->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newGimmick->SetScale(scale);

			newGimmick->SetCamera(camera_);
			newGimmick->Update();
			//���X�g�ɓo�^
			gimmicks_.push_back(std::move(newGimmick));
		}
		//�S�[��
		else if (objectData.objectType.find("GOAL") == 0)
		{
			//�S�[��������
			std::unique_ptr<Goal> newgoal;
			newgoal = Goal::Create(model);
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
		//�A�C�e��
		else if (objectData.objectType.find("ITEM") == 0)
		{
			//�A�C�e��������
			std::unique_ptr<Item> newitem;
			std::unique_ptr<Player>& player = players_.front();
			//�W�����v
			if (objectData.objectPattern.find("JUMP") == 0)
				newitem = Item::Create(modelItemJump_, player.get(), SUBCOLLISION_ATTR_ITEM_JUMP);
			//�񕜃A�C�e��
			else if (objectData.objectPattern.find("HEAL") == 0)
				newitem = Item::Create(modelItemHeal_, player.get(), SUBCOLLISION_ATTR_ITEM_HEAL);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newitem->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newitem->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newitem->SetScale(scale);

			newitem->SetCamera(camera_);
			newitem->Update();
			//���X�g�ɓo�^
			items_.push_back(std::move(newitem));
		}
		//�n�`
		else
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


			// �z��ɓo�^
			objects.push_back(newObject);
		}

	}

}

void GamePlayScene::AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet)
{
	//���X�g�ɓo�^
	playerBullets_.push_back(std::move(playerBullet));
}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	//���X�g�ɓo�^
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GamePlayScene::LoadModel()
{
	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy1_ = Model::LoadFromOBJ("enemy1");
	modelBoss1_ = Model::LoadFromOBJ("boss1");
	modelBossCore1_ = Model::LoadFromOBJ("core1");
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelItemJump_ = Model::LoadFromOBJ("itemjump");
	modelItemHeal_ = Model::LoadFromOBJ("itemheal");
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelBox = Model::LoadFromOBJ("sphere2", true);

	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("enemy1", modelEnemy1_));
	models.insert(std::make_pair("boss1", modelBoss1_));
	models.insert(std::make_pair("core1", modelBossCore1_));
	models.insert(std::make_pair("sphere", modelGoal_));
	models.insert(std::make_pair("Itemjump", modelItemJump_));
	models.insert(std::make_pair("itemheal", modelItemHeal_));
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("sphere2", modelBox));

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
