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
	//工業地帯
	enemyFactory_ = std::make_unique<EnemyFactory>();
	gimmickFactory_ = std::make_unique<GimmickFactory>();

	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	//音声データ
	sound_ = audio_->SoundLoadWave("Resources/TestMusic.wav");

	//音声再生呼び出し例
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	camera_->Reset();
	//弾リセット
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Reset();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Reset();


	//モデル読み込み
	LoadModel();
	//レベルデータ読み込み
	if (stageNum_ == SL_Stage1_Area1)LoadLVData("test");
	else if (stageNum_ == SL_Stage1_Area2)LoadLVData("stage2");
	else if (stageNum_ == SL_Stage1_Area3)LoadLVData("stage3_2");
	else if (stageNum_ == SL_Stage1_AreaBoss)LoadLVData("stageboss1");

	else if (stageNum_ == SL_StageTutorial_Area1)LoadLVData("tutorial");
	else if (stageNum_ == SL_StageTutorial_Area2)LoadLVData("tutorial2");
	else if (stageNum_ == SL_StageTutorial_Area3)LoadLVData("tutorial3");
	else if (stageNum_ == SL_StageTutorial_Final)LoadLVData("tutorialf");

	//スプライト
	LoadSprite();

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	//パーティクル
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
	//死亡フラグの立った弾を削除
	playerBullets_.remove_if(
		[](std::unique_ptr<PlayerBullet>& pbullet) { return pbullet->IsDead(); });
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& ebullet) { return ebullet->IsDead(); });

	players_.remove_if(
		[](std::unique_ptr<Player>& player) {return player->IsDead(); });
	enemys_.remove_if(
		[](std::unique_ptr<BaseEnemy>& enemy) {return enemy->IsDead(); });

	//天球ぐるぐる
	for (Object3d*& skydome : skydomes_)
	{
		//天球回転用
		XMFLOAT3 rotSkydome = skydome->GetRotation();
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;

		skydome->SetRotation(rotSkydome);

		skydome->Update();
	}

	if (!isPause_)
	{
		//モデル呼び出し例
		for (std::unique_ptr<Player>& player : players_)
		{
			if (!isclear_ || !isGameover_)
			{
				//チュートリアル基本操作
				if (stageNum_ == SL_StageTutorial_Area1)player->Update(false, false);
				//チュートリアル奥側移動→攻撃→応用ステージ
				else if (stageNum_ == SL_StageTutorial_Area2)player->Update(true, false);
				//基本状態
				else player->Update();
			}

			lightGroup_->SetPointLightPos(0, player->GetWorldPosition());
			//かめおべら
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
		//弾更新
		for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets_) playerBullet->Update();
		
		for (std::unique_ptr<BaseEnemy>& enemy : enemys_)
		{
			enemy->Update();
			//ボス撃破
			if (enemy->BossDead())isclear_ = true;
		}
		//弾更新
		for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();

		for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();

		for (std::unique_ptr<Goal>& goal : goals_)
		{
			goal->Update();
			//クリア
			if (goal->IsGoal()) isclear_ = true;
		}
		for (std::unique_ptr<Item>& item : items_)
		{
			item->Update();
		}


		for (Object3d*& object : objects_) object->Update();

		//カメラ
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
		//Pause機能
		if (input_->TriggerKey(DIK_Q) && !isclear_ && !isGameover_)
		{
			//ここでイージングの準備
			easePause_.Standby(false);
			isBack_ = false;
			spritePause_->SetPosition({ easePause_.start,0.0f });

			isPause_ = true;
		}
	}
	else if (isPause_)
	{
		//イージングサンプル(ポーズ中に準備してもここがやってくれる)
		easePause_.ease_in_out_elastic();
		spritePause_->SetPosition({ easePause_.num_X,0.0f });

		if (input_->TriggerKey(DIK_W))
		{
			sceneManager_->ChangeScene("TITLE", stageNum_);
			isPause_ = false;
		}

		if (input_->TriggerKey(DIK_Q))
		{
			//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
			if (spritePause_->GetPosition().x == easePause_.end) easePause_.Standby(true);
			isBack_ = true;
		}
		//到達したらPause解除
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
	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	for (std::unique_ptr<Player>& player : players_)player->Draw();
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Draw();
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->Draw();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();
	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_) gimmick->Draw();
	for (std::unique_ptr<Goal>& goal : goals_)goal->Draw();
	for (std::unique_ptr<Item>& item : items_)item->Draw();
	for (auto& skydome : skydomes_)skydome->Draw();
	for (auto& object : objects_)object->Draw();
	
	//モデル描画後処理
	Object3d::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	pm_->Draw();
	for (std::unique_ptr<Player>& player : players_)player->DrawParticle();
	for (std::unique_ptr<Item>& item : items_)item->DrawParticle();
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//スプライト描画前処理
	spCommon_->PreDraw();
	//前景スプライト
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
	//終了処理
	audio_->Finalize();
	//解放
	//各種音声
	audio_->SoundUnLoad(&sound_);

	//パーティクル
	delete particle1_;
	delete particle2_;
	delete pm_;
	//ライト
	delete lightGroup_;
	//モデル

	for (Object3d*& object : objects_)delete object;
	for (Object3d*& skydome : skydomes_)delete skydome;
	objects_.clear();
	skydomes_.clear();

	//3Dモデル
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

	//スプライト
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
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);
	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData_->objects) {

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}
		//プレイヤー
		if (objectData.objectType.find("PLAYER") == 0)
		{
			//プレイヤー初期化
			std::unique_ptr<Player> newplayer;

			newplayer = Player::Create(model, modelPlayerBullet_, this);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newplayer->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newplayer->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newplayer->SetScale(scale);

			newplayer->SetCamera(camera_);
			newplayer->Update();
			//リストに登録
			players_.push_back(std::move(newplayer));
		}
		//敵
		else if (objectData.objectType.find("ENEMY") == 0)
		{
			//敵初期化
			std::unique_ptr<BaseEnemy> newenemy;
			std::unique_ptr<Player>& player = players_.front();

			newenemy = enemyFactory_->CreateEnemy(objectData.objectPattern,
				model, modelEnemyBullet_, player.get(), this);

			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newenemy->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newenemy->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newenemy->SetScale(scale);

			newenemy->SetCamera(camera_);
			newenemy->Update();
			//リストに登録
			enemys_.push_back(std::move(newenemy));
		}
		//仕掛け
		else if (objectData.objectType.find("GIMMICK") == 0)
		{
			//敵初期化
			std::unique_ptr<BaseGimmick> newGimmick;
			std::unique_ptr<Player>& player = players_.front();

			newGimmick = gimmickFactory_->CreateGimmick(objectData.objectPattern, model, player.get());

			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newGimmick->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newGimmick->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newGimmick->SetScale(scale);

			newGimmick->SetCamera(camera_);
			newGimmick->Update();
			//リストに登録
			gimmicks_.push_back(std::move(newGimmick));
		}
		//ゴール
		else if (objectData.objectType.find("GOAL") == 0)
		{
			//ゴール初期化
			std::unique_ptr<Goal> newgoal;
			newgoal = Goal::Create(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newgoal->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newgoal->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newgoal->SetScale(scale);

			newgoal->SetCamera(camera_);
			newgoal->Update();
			//リストに登録
			goals_.push_back(std::move(newgoal));
		}
		//アイテム
		else if (objectData.objectType.find("ITEM") == 0)
		{
			//アイテム初期化
			std::unique_ptr<Item> newitem;
			std::unique_ptr<Player>& player = players_.front();
			//ジャンプ
			if (objectData.objectPattern.find("JUMP") == 0)
				newitem = Item::Create(modelItemJump_, player.get(), SUBCOLLISION_ATTR_ITEM_JUMP);
			//回復アイテム
			else if (objectData.objectPattern.find("HEAL") == 0)
				newitem = Item::Create(modelItemHeal_, player.get(), SUBCOLLISION_ATTR_ITEM_HEAL);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newitem->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newitem->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newitem->SetScale(scale);

			newitem->SetCamera(camera_);
			newitem->Update();
			//リストに登録
			items_.push_back(std::move(newitem));
		}
		//当たり判定がないオブジェクト
		else if (objectData.objectType.find("NONE") == 0)
		{
			// モデルを指定して3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			newObject->SetCamera(camera_);


			// 配列に登録
			objects_.push_back(newObject);
		}
		//天球
		else if (objectData.objectType.find("SKYDOME") == 0)
		{
			// モデルを指定して3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			//オブジェクトにモデル紐付ける
			newObject->SetModel(model);

			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			newObject->SetCamera(camera_);
			// 配列に登録
			skydomes_.push_back(newObject);
		}
		//地形
		else
		{
			// モデルを指定して3Dオブジェクトを生成
			TouchableObject* newObject = TouchableObject::Create(model, true);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			newObject->SetCamera(camera_);


			// 配列に登録
			objects_.push_back(newObject);
		}

	}

}

void GamePlayScene::AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet)
{
	//リストに登録
	playerBullets_.push_back(std::move(playerBullet));
}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	//リストに登録
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GamePlayScene::LoadModel()
{
	// モデル読み込み
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

	//ダッシュ
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
	//ジャンプ
	if (input_->PushKey(DIK_SPACE))spriteTutorialHTPJump_->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	//奥側ジャンプ
	if (input_->PushKey(DIK_Z))spriteTutorialHTPMoveBack_->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	//攻撃
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
	//スプライト
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
