#include "GamePlayScene.h"
#include "LevelLoaderJson.h"

#include "CollisionManager.h"
#include "MeshCollider.h"
#include "TouchableObject.h"
#include "CollisionAttribute.h"
#include "StageList.h"

#include <cassert>
#include <sstream>
#include <iomanip>

#include "StageList.h"

using namespace DirectX;

DirectXCommon* GamePlayScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
Audio* GamePlayScene::audio_ = Audio::GetInstance();
Camera* GamePlayScene::camera_ = Camera::GetInstance();
SceneManager* GamePlayScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GamePlayScene::imguiManager_ = ImGuiManager::GetInstance();


GamePlayScene::GamePlayScene(int stagenum) :stageNum_(stagenum) {}

void GamePlayScene::Initialize()
{

	spCommon_ = SpriteCommon::GetInstance();
	colManager_ = CollisionManager::GetInstance();
	//�H�ƒn��
	enemyFactory_ = std::make_unique<EnemyFactory>();
	gimmickFactory_ = std::make_unique<GimmickFactory>();

	// �`�揉���������@��������
#pragma region �`�揉��������
	//�����f�[�^
	sound_ = audio_->SoundLoadWave("Resources/TestMusic.wav");

	//�����Đ��Ăяo����
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	camera_->Reset();
	//�e���Z�b�g
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Reset();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Reset();


	//���f���ǂݍ���
	LoadModel();
	//���x���f�[�^�ǂݍ���
	if (stageNum_ == SL_Stage1_Area1)LoadLVData("test");
	else if (stageNum_ == SL_Stage1_Area2)LoadLVData("stage2");
	else if (stageNum_ == SL_Stage1_Area3)LoadLVData("stage3_2");
	else if (stageNum_ == SL_Stage1_AreaBoss)LoadLVData("stageboss1");

	else if (stageNum_ == SL_StageTutorial_Area1)LoadLVData("tutorial");
	else if (stageNum_ == SL_StageTutorial_Area2)LoadLVData("tutorial2");
	else if (stageNum_ == SL_StageTutorial_Area3)LoadLVData("tutorial3");
	else if (stageNum_ == SL_StageTutorial_Final)LoadLVData("tutorialf");

	//�X�v���C�g
	LoadSprite();

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
	if (stageNum_ >= SL_StageTutorial_Area1)for (int i = 0; i < 6; i++)easeInfo_[i].Standby(false);
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

	//�V�����邮��
	for (Object3d*& skydome : skydomes_)
	{
		//�V����]�p
		XMFLOAT3 rotSkydome = skydome->GetRotation();
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;

		skydome->SetRotation(rotSkydome);

		skydome->Update();
	}

	if (!isPause_)
	{
		//���f���Ăяo����
		for (std::unique_ptr<Player>& player : players_)
		{
			if (!isclear_ || !isGameover_)
			{
				//�`���[�g���A����{����
				if (stageNum_ == SL_StageTutorial_Area1)player->Update(false, false);
				//�`���[�g���A�������ړ����U�������p�X�e�[�W
				else if (stageNum_ == SL_StageTutorial_Area2)player->Update(true, false);
				//��{���
				else player->Update();
			}

			lightGroup_->SetPointLightPos(0, player->GetWorldPosition());
			//���߂��ׂ�
			if (player->IsDead())isGameover_ = true;

			//ImGui	
			imguiManager_->Begin();
			int plife[1] = { player->GetLife() };
			ImGui::Begin("Player");
			ImGui::SetWindowPos(ImVec2(1090.0f, 50.0f));
			ImGui::SetWindowSize(ImVec2(150.0f, 50.0f));
			ImGui::InputInt("plife", plife);
			ImGui::End();
			imguiManager_->End();
		}
		//�e�X�V
		for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets_) playerBullet->Update();
		
		for (std::unique_ptr<BaseEnemy>& enemy : enemys_)
		{
			enemy->Update();
			//�{�X���j
			if (enemy->BossDead())isclear_ = true;
		}
		//�e�X�V
		for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();

		for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();

		for (std::unique_ptr<Goal>& goal : goals_)
		{
			goal->Update();
			//�N���A
			if (goal->IsGoal()) isclear_ = true;
		}
		for (std::unique_ptr<Item>& item : items_)
		{
			item->Update();
		}


		for (Object3d*& object : objects_) object->Update();

		//�J����
		camera_->Update();
		lightGroup_->Update();
		pm_->Update();

		if (isGameover_)
		{
			sceneManager_->ChangeScene("GAMEOVER", stageNum_);
			isGameover_ = false;
		}
		if (isclear_)
		{
			sceneManager_->ChangeScene("STAGECLEAR", stageNum_);
			isclear_ = false;
		}
		colManager_->CheckAllCollisions();
		//Pause�@�\
		if (input_->TriggerKey(DIK_Q) && !isclear_ && !isGameover_)
		{
			//�����ŃC�[�W���O�̏���
			easePause_.Standby(false);
			isBack_ = false;
			spritePause_->SetPosition({ easePause_.start,0.0f });

			isPause_ = true;
		}
	}
	else if (isPause_)
	{
		//�C�[�W���O�T���v��(�|�[�Y���ɏ������Ă�����������Ă����)
		easePause_.ease_in_out_elastic();
		spritePause_->SetPosition({ easePause_.num_X,0.0f });

		if (input_->TriggerKey(DIK_W))
		{
			sceneManager_->ChangeScene("TITLE", stageNum_);
			isPause_ = false;
		}

		if (input_->TriggerKey(DIK_Q))
		{
			//�����ŃC�[�W���O�̏����B�������I�����W�ɓ��B���Ă��Ȃ��Ǝ󂯕t���Ȃ�
			if (spritePause_->GetPosition().x == easePause_.end) easePause_.Standby(true);
			isBack_ = true;
		}
		//���B������Pause����
		if (spritePause_->GetPosition().x == easePause_.start)
		{
			if (isBack_)isPause_ = false;
		}
	}
	spritePause_->Update();

	if (stageNum_ == SL_StageTutorial_Area1)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			nullptr, nullptr, spriteTutorialInfo1_);
	}
	else if (stageNum_ == SL_StageTutorial_Area2)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			spriteTutorialHTPMoveBack_, nullptr, spriteTutorialInfo2_);
	}
	else if (stageNum_ == SL_StageTutorial_Area3)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			spriteTutorialHTPMoveBack_, spriteTutorialHTPAttack_, spriteTutorialInfo3_);
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			spriteTutorialHTPMoveBack_, spriteTutorialHTPAttack_, spriteTutorialInfo4_);
	}
	UpdateTutorialSprite();
	
	
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
	for (auto& skydome : skydomes_)skydome->Draw();
	for (auto& object : objects_)object->Draw();
	
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
	else
	{
		spritePauseInfo_->Draw();
		for (std::unique_ptr<Item>& item : items_)
		{
			item->DrawSprite();
		}

		if (stageNum_ == SL_StageTutorial_Area1)
		{
			DrawTutorialSprite(spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
				nullptr, nullptr, spriteTutorialInfo1_);
		}

		else if (stageNum_ == SL_StageTutorial_Area2)
		{
			DrawTutorialSprite(spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
				spriteTutorialHTPMoveBack_, nullptr, spriteTutorialInfo2_);
		}
		else if (stageNum_ == SL_StageTutorial_Area3)
		{
			DrawTutorialSprite(spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
				spriteTutorialHTPMoveBack_, spriteTutorialHTPAttack_, spriteTutorialInfo3_);
		}
		else if (stageNum_ == SL_StageTutorial_Final)
		{
			DrawTutorialSprite(spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
				spriteTutorialHTPMoveBack_, spriteTutorialHTPAttack_, spriteTutorialInfo4_);
		}
	}
}

void GamePlayScene::Finalize()
{
	//�I������
	audio_->Finalize();
	//���
	//�e�퉹��
	audio_->SoundUnLoad(&sound_);

	//�p�[�e�B�N��
	delete particle1_;
	delete particle2_;
	delete pm_;
	//���C�g
	delete lightGroup_;
	//���f��

	for (Object3d*& object : objects_)delete object;
	for (Object3d*& skydome : skydomes_)delete skydome;
	objects_.clear();
	skydomes_.clear();

	//3D���f��
	delete modelPlayer_;
	delete modelPlayerBullet_;
	delete modelEnemy1_;
	delete modelEnemyBullet_;
	delete modelBoss1_;
	delete modelBossCore1_;
	delete modelStageT_;
	delete modelStage1_;
	delete modelStage2_;
	delete modelItemJump_;
	delete modelItemHeal_;
	delete modelSpike_;
	delete modelGround_;
	delete modelBox_;
	delete modelGoal_;

	models_.clear();

	//�X�v���C�g
	delete spritePause_;
	delete spritePauseInfo_;

	delete spriteTutorialInfo1_;
	delete spriteTutorialInfo2_;
	delete spriteTutorialInfo3_;
	delete spriteTutorialInfo4_;

	delete spriteTutorialHTPDash_;
	delete spriteTutorialHTPMove_;
	delete spriteTutorialHTPJump_;
	delete spriteTutorialHTPMoveBack_;
	delete spriteTutorialHTPAttack_;
}

void GamePlayScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);
	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (LevelData::ObjectData& objectData : levelData_->objects) {

		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}
		//�v���C���[
		if (objectData.objectType.find("PLAYER") == 0)
		{
			//�v���C���[������
			std::unique_ptr<Player> newplayer;

			newplayer = Player::Create(model, modelPlayerBullet_, this);
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

			newenemy = enemyFactory_->CreateEnemy(objectData.objectPattern,
				model, modelEnemyBullet_, player.get(), this);

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

			newGimmick = gimmickFactory_->CreateGimmick(objectData.objectPattern, model, player.get());

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
		//�����蔻�肪�Ȃ��I�u�W�F�N�g
		else if (objectData.objectType.find("NONE") == 0)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			Object3d* newObject = Object3d::Create();
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
			objects_.push_back(newObject);
		}
		//�V��
		else if (objectData.objectType.find("SKYDOME") == 0)
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
			skydomes_.push_back(newObject);
		}
		//�n�`
		else
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			TouchableObject* newObject = TouchableObject::Create(model, true);
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
			objects_.push_back(newObject);
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
	modelPlayerBullet_ = Model::LoadFromOBJ("playerbullet");
	modelEnemy1_ = Model::LoadFromOBJ("enemy1");
	modelEnemyBullet_ = Model::LoadFromOBJ("enemybullet");
	modelBoss1_ = Model::LoadFromOBJ("boss1");
	modelBossCore1_ = Model::LoadFromOBJ("core1");
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelItemJump_ = Model::LoadFromOBJ("itemjump");
	modelItemHeal_ = Model::LoadFromOBJ("itemheal");
	modelSpike_ = Model::LoadFromOBJ("spikeball");
	modelStageT_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");
	modelGround_ = Model::LoadFromOBJ("ground");
	modelBox_ = Model::LoadFromOBJ("sphere2", true);

	models_.insert(std::make_pair("player", modelPlayer_));
	models_.insert(std::make_pair("playerbullet", modelPlayerBullet_));
	models_.insert(std::make_pair("enemy1", modelEnemy1_));
	models_.insert(std::make_pair("enemybullet", modelEnemyBullet_));
	models_.insert(std::make_pair("boss1", modelBoss1_));
	models_.insert(std::make_pair("core1", modelBossCore1_));
	models_.insert(std::make_pair("sphere", modelGoal_));
	models_.insert(std::make_pair("Itemjump", modelItemJump_));
	models_.insert(std::make_pair("itemheal", modelItemHeal_));
	models_.insert(std::make_pair("spikeball", modelSpike_));
	models_.insert(std::make_pair("skydomet", modelStageT_));
	models_.insert(std::make_pair("skydome", modelStage1_));
	models_.insert(std::make_pair("skydome2", modelStage2_));
	models_.insert(std::make_pair("ground", modelGround_));
	models_.insert(std::make_pair("sphere2", modelBox_));

}

void GamePlayScene::SettingTutorialEase(int num, Sprite* s1, Sprite* s2,
	Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)
{
	switch (num)
	{
	case 0:
		if (s1 != nullptr)s1->SetPosition({ easeInfo_[0].start,50.0f });
		if (s2 != nullptr)s2->SetPosition({ easeInfo_[1].start,50.0f });
		if (s3 != nullptr)s3->SetPosition({ easeInfo_[2].start,110.0f });
		if (s4 != nullptr)s4->SetPosition({ easeInfo_[3].start,110.0f });
		if (s5 != nullptr)s5->SetPosition({ easeInfo_[4].start,170.0f });
		if (s6 != nullptr)s6->SetPosition({ easeInfo_[5].start,250.0f });
		break;
	case 1:
		for (int i = 0; i < 6; i++)easeInfo_[i].ease_out_expo();
		if (s1 != nullptr)s1->SetPosition({ easeInfo_[0].num_X,50.0f  });
		if (s2 != nullptr)s2->SetPosition({ easeInfo_[1].num_X,50.0f });
		if (s3 != nullptr)s3->SetPosition({ easeInfo_[2].num_X,110.0f });
		if (s4 != nullptr)s4->SetPosition({ easeInfo_[3].num_X,110.0f });
		if (s5 != nullptr)s5->SetPosition({ easeInfo_[4].num_X,110.0f });
		if (s6 != nullptr)s6->SetPosition({ easeInfo_[5].num_X,200.0f });
		break;
	}

}

void GamePlayScene::UpdateTutorialSprite()
{
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

	spriteTutorialHTPDash_->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPMove_->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPJump_->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPMoveBack_->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPAttack_->SetColor({ 0.1f,0.1f,0.1f,1.0f });

	//�_�b�V��
	if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
	{
		if (input_->PushKey(DIK_A))spriteTutorialHTPDash_->SetColor({ 0.1f,0.1f,1.0f,1.0f });
		if (input_->PushKey(DIK_D))spriteTutorialHTPDash_->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	}
	else
	{
		if (input_->PushKey(DIK_A))spriteTutorialHTPMove_->SetColor({ 0.1f,0.1f,1.0f,1.0f });
		if (input_->PushKey(DIK_D))spriteTutorialHTPMove_->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	}
	//�W�����v
	if (input_->PushKey(DIK_SPACE))spriteTutorialHTPJump_->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	//�����W�����v
	if (input_->PushKey(DIK_Z))spriteTutorialHTPMoveBack_->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	//�U��
	if (input_->PushKey(DIK_X))spriteTutorialHTPAttack_->SetColor({ 1.0f,0.1f,0.1f,1.0f });

	spriteTutorialInfo1_->SetColor({ 0.1f + speedColor_,0.1f,0.1f,1.0f });
	spriteTutorialInfo2_->SetColor({ 0.1f + speedColor_,0.1f,0.1f,1.0f });
	spriteTutorialInfo3_->SetColor({ 0.1f + speedColor_,0.1f,0.1f,1.0f });
	spriteTutorialInfo4_->SetColor({ 0.1f,0.1f,0.1f + speedColor_,1.0f });

	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();

	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();
}

void GamePlayScene::DrawTutorialSprite(Sprite* s1, Sprite* s2, Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)
{
	if (s1 != nullptr)s1->Draw();
	if (s2 != nullptr)s2->Draw();
	if (s3 != nullptr)s3->Draw();
	if (s4 != nullptr)s4->Draw();
	if (s5 != nullptr)s5->Draw();
	if (s6 != nullptr)s6->Draw();
}

void GamePlayScene::LoadSprite()
{
	//�X�v���C�g
	spCommon_->LoadTexture(10, "texture/pausep.png");
	spritePause_->Initialize(spCommon_, 10);
	spritePause_->SetColor({ 1.0f,1.0f,1.0f,0.5f });

	spCommon_->LoadTexture(12, "texture/pauseinfo.png");
	spritePauseInfo_->Initialize(spCommon_, 12);

	spCommon_->LoadTexture(1000, "texture/info/tinfo1.png");//1
	spriteTutorialInfo1_->Initialize(spCommon_, 1000);

	spCommon_->LoadTexture(1001, "texture/info/tinfo2.png");//2
	spriteTutorialInfo2_->Initialize(spCommon_, 1001);

	spCommon_->LoadTexture(1002, "texture/info/tinfo3.png");//3
	spriteTutorialInfo3_->Initialize(spCommon_, 1002);

	spCommon_->LoadTexture(1003, "texture/info/tinfo4.png");//4
	spriteTutorialInfo4_->Initialize(spCommon_, 1003);

	spCommon_->LoadTexture(1004, "texture/info/moveinfo.png");//1~
	spriteTutorialHTPMove_->Initialize(spCommon_, 1004);
	spCommon_->LoadTexture(1005, "texture/info/dashinfo.png");//1~
	spriteTutorialHTPDash_->Initialize(spCommon_, 1005);
	spCommon_->LoadTexture(1006, "texture/info/jumpinfo.png");//1~
	spriteTutorialHTPJump_->Initialize(spCommon_, 1006);
	spCommon_->LoadTexture(1007, "texture/info/backmoveinfo1.png");//2~
	spriteTutorialHTPMoveBack_->Initialize(spCommon_, 1007);
	spCommon_->LoadTexture(1008, "texture/info/attackinfo.png");//3~
	spriteTutorialHTPAttack_->Initialize(spCommon_, 1008);

	if (stageNum_ == SL_StageTutorial_Area1)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			nullptr, nullptr, spriteTutorialInfo1_);
	}

	else if (stageNum_ == SL_StageTutorial_Area2)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			spriteTutorialHTPMoveBack_, nullptr, spriteTutorialInfo2_);
	}
	else if (stageNum_ == SL_StageTutorial_Area3)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			spriteTutorialHTPMoveBack_, spriteTutorialHTPAttack_, spriteTutorialInfo3_);
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove_, spriteTutorialHTPDash_, spriteTutorialHTPJump_,
			spriteTutorialHTPMoveBack_, spriteTutorialHTPAttack_, spriteTutorialInfo4_);
	}

	spritePause_->Update();
	spritePauseInfo_->Update();

	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();

	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();

}
