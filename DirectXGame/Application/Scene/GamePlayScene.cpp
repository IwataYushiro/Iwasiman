#include "GamePlayScene.h"
#include "LevelLoaderJson.h"

#include "CollisionManager.h"
#include "MeshCollider.h"
#include "TouchableObject.h"

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
	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	//音声データ
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//スプライト
	LoadSprite();

	//音声再生呼び出し例
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	//弾リセット
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Reset();
	}

	//モデル読み込み
	LoadModel();
	//レベルデータ読み込み
	LoadLVData("stage1");

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
}

void GamePlayScene::Update()
{
	//死亡フラグの立った弾を削除
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });
	enemys_.remove_if(
		[](std::unique_ptr<Enemy>& enemy) { return enemy->IsDead(); });

	//敵更新
	for (std::unique_ptr<Enemy>& enemy : enemys_) enemy->Update();
	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) bullet->Update();


	if (!isPause_)
	{
		//モデル呼び出し例
		for (std::unique_ptr<Player>& player : players_)
		{
			if (!isclear || !isGameover) player->Update();
			lightGroup_->SetPointLightPos(0, player->GetWorldPosition());
			//かめおべら
			if (player->GetPosition().y <= -60.0f || player->IsDead())isGameover = true;
		}
		for (std::unique_ptr<Goal>& goal : goals_)
		{
			goal->Update();
			//クリア
			if (goal->IsGoal()) isclear = true;
		}
		for (std::unique_ptr<ItemJump>& itemj : jItems_)
		{
			
			if (itemj->IsGet())spriteItemJumpBar_->SetColor({ 1.0f, 1.0f,1.0f, itemj->GetEasing().num_X});
			else spriteItemJumpBar_->SetColor({ 1.0f, 1.0f, 1.0f,itemj->GetEasing().start});
			itemj->Update();
		}
		for (std::unique_ptr<ItemHeal>& itemh : hItems_)
		{
			itemh->Update();
		}

		for (auto& object : objects) object->Update();

		//カメラ
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

		//Pause機能
		if (input_->TriggerKey(DIK_Q) && !isclear && !isGameover)
		{
			//ここでイージングの準備
			es.Standby(false);
			isBack = false;
			spritePause_->SetPosition({ es.start,0.0f });

			isPause_ = true;
		}
	}
	else if (isPause_)
	{
		//イージングサンプル(ポーズ中に準備してもここがやってくれる)
		es.ease_in_out_elastic();
		spritePause_->SetPosition({ es.num_X,0.0f });

		if (input_->TriggerKey(DIK_W))
		{
			sceneManager_->ChangeScene("TITLE");
			isPause_ = false;
		}

		if (input_->TriggerKey(DIK_Q))
		{
			//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
			if (spritePause_->GetPosition().x == es.end) es.Standby(true);
			isBack = true;
		}
		//到達したらPause解除
		if (spritePause_->GetPosition().x == es.start)
		{
			if (isBack)isPause_ = false;
		}
	}
	spritePause_->Update();
	spriteItemJumpBar_->Update();
	
		//ImGui	
		imguiManager_->Begin();
		int plife[1] = { players_.front()->GetLife() };
		ImGui::Begin("Player");
		ImGui::SetWindowPos(ImVec2(200.0f, 200.0f));
		ImGui::SetWindowSize(ImVec2(150.0f, 50.0f));
		ImGui::InputInt("plife", plife);
		ImGui::End();
		imguiManager_->End();
	
}

void GamePlayScene::Draw()
{
	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	for (std::unique_ptr<Player>& player : players_)player->Draw();
	for (std::unique_ptr<Enemy>& enemy : enemys_) enemy->Draw();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();
	for (std::unique_ptr<Goal>& goal : goals_)goal->Draw();
	for (std::unique_ptr<ItemJump>& itemj : jItems_)itemj->Draw();
	for (std::unique_ptr<ItemHeal>& itemh : hItems_)itemh->Draw();
	for (auto& object : objects)object->Draw();

	//モデル描画後処理
	Object3d::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	pm_->Draw();
	for (std::unique_ptr<Player>& player : players_)player->DrawParticle();
	for (std::unique_ptr<ItemHeal>& itemh : hItems_)itemh->DrawParticle();
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//スプライト描画前処理
	spCommon_->PreDraw();
	//前景スプライト
	if (isPause_)spritePause_->Draw();
	else if (isclear)spriteClear_->Draw();
	else if (isGameover)spriteGameover_->Draw();
	else
	{
		spritePauseInfo_->Draw();
		for (std::unique_ptr<ItemJump>& itemj : jItems_)
		{
			if (itemj->IsGet())spriteItemJumpBar_->Draw();
		}
	}
}

void GamePlayScene::Finalize()
{
	//終了処理
	audio_->Finalize();
	//解放
	//各種音声
	audio_->SoundUnLoad(&sound);

	//パーティクル
	delete particle1_;
	delete particle2_;
	delete pm_;
	//ライト
	delete lightGroup_;
	//モデル

	for (Object3d*& object : objects)delete object;
	objects.clear();

	//3Dモデル
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelSkydome;
	delete modelItemJump_;
	delete modelItemHeal_;
	delete modelGround;
	delete modelBox;
	delete modelGoal_;
	//スプライト
	delete spritePause_;
	delete spriteClear_;
	delete spritePauseInfo_;
	delete spriteGameover_;
	delete spriteItemJumpBar_;

}

void GamePlayScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile(stagePath);

	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData->objects) {

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}
		//プレイヤー
		if (objectData.objectType.find("PLAYER") == 0)
		{
			//プレイヤー初期化
			std::unique_ptr<Player> newplayer;
			newplayer = Player::Create(modelPlayer_);
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
			std::unique_ptr<Enemy> newenemy;
			std::unique_ptr<Player>& player = players_.front();
			newenemy = Enemy::Create(modelEnemy_, player.get(), this);
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
		//ゴール
		else if (objectData.objectType.find("GOAL") == 0)
		{
			//ゴール初期化
			std::unique_ptr<Goal> newgoal;
			newgoal = Goal::Create(modelGoal_);
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
			//ジャンプ
			if (objectData.objectPattern.find("JUMP") == 0)
			{
				//アイテム初期化
				std::unique_ptr<ItemJump> newitemj;
				std::unique_ptr<Player>& player = players_.front();
				newitemj = ItemJump::Create(modelItemJump_, player.get());
				// 座標
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, objectData.trans);
				newitemj->SetPosition(pos);

				// 回転角
				DirectX::XMFLOAT3 rot;
				DirectX::XMStoreFloat3(&rot, objectData.rot);
				newitemj->SetRotation(rot);

				// 座標
				DirectX::XMFLOAT3 scale;
				DirectX::XMStoreFloat3(&scale, objectData.scale);
				newitemj->SetScale(scale);

				newitemj->SetCamera(camera_);
				newitemj->Update();
				//リストに登録
				jItems_.push_back(std::move(newitemj));
			}
			//回復アイテム
			else if (objectData.objectPattern.find("HEAL") == 0)
			{
				//アイテム初期化
				std::unique_ptr<ItemHeal> newitemh;
				std::unique_ptr<Player>& player = players_.front();
				newitemh = ItemHeal::Create(modelItemHeal_, player.get());
				// 座標
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, objectData.trans);
				newitemh->SetPosition(pos);

				// 回転角
				DirectX::XMFLOAT3 rot;
				DirectX::XMStoreFloat3(&rot, objectData.rot);
				newitemh->SetRotation(rot);

				// 座標
				DirectX::XMFLOAT3 scale;
				DirectX::XMStoreFloat3(&scale, objectData.scale);
				newitemh->SetScale(scale);

				newitemh->SetCamera(camera_);
				newitemh->Update();
				//リストに登録
				hItems_.push_back(std::move(newitemh));
			}
		}
		//地形
		else
		{
			// モデルを指定して3Dオブジェクトを生成
			TouchableObject* newObject = TouchableObject::Create(model);
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
			newObject->Update();

			// 配列に登録
			objects.push_back(newObject);
		}

	}

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
	modelEnemy_ = Model::LoadFromOBJ("enemy1");
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelItemJump_ = Model::LoadFromOBJ("itemjump");
	modelItemHeal_ = Model::LoadFromOBJ("itemheal");
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelBox = Model::LoadFromOBJ("sphere2", true);

	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("enemy1", modelEnemy_));
	models.insert(std::make_pair("sphere", modelGoal_));
	models.insert(std::make_pair("itemjump", modelItemJump_));
	models.insert(std::make_pair("itemheal", modelItemHeal_));
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("sphere2", modelBox));

}

void GamePlayScene::LoadSprite()
{
	//スプライト
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

	//アイテム関係
	spCommon_->LoadTexture(30, "itemtex/itemjumpbar.png");
	spriteItemJumpBar_->Initialize(spCommon_, 30);
	spriteItemJumpBar_->SetPosition({ 0.0f,100.0f });

	spriteItemJumpBar_->Update();
}
