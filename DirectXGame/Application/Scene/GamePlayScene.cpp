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
	//工業地帯
	enemyFactory = std::make_unique<EnemyFactory>();
	gimmickFactory = std::make_unique<GimmickFactory>();

	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	//音声データ
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");

	//音声再生呼び出し例
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	camera_->Reset();
	//弾リセット
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Reset();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Reset();


	//モデル読み込み
	LoadModel();
	//レベルデータ読み込み
	if (stageNum == 1)LoadLVData("test");
	else if (stageNum == 2)LoadLVData("stage2");
	else if (stageNum == 3)LoadLVData("stage3_2");
	else if (stageNum == 4)LoadLVData("stageboss1");

	else if (stageNum == 100)LoadLVData("tutorial");
	else if (stageNum == 101)LoadLVData("tutorial2");
	else if (stageNum == 102)LoadLVData("tutorial3");
	else if (stageNum == 103)LoadLVData("tutorialf");

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
	if (stageNum >= 100)for (int i = 0; i < 6; i++)easeInfo[i].Standby(false);
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

	//弾更新
	


	if (!isPause_)
	{
		//モデル呼び出し例
		for (std::unique_ptr<Player>& player : players_)
		{
			if (!isclear || !isGameover)
			{
				//チュートリアル基本操作
				if (stageNum == 100)player->Update(false, false);
				//チュートリアル奥側移動→攻撃→応用ステージ
				else if (stageNum == 101)player->Update(true, false);
				//基本状態
				else player->Update();
			}

			lightGroup_->SetPointLightPos(0, player->GetWorldPosition());
			//かめおべら
			if (player->IsDead())isGameover = true;

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
			if (enemy->BossDead())isclear = true;
		}
		//弾更新
		for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();

		for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();

		for (std::unique_ptr<Goal>& goal : goals_)
		{
			goal->Update();
			//クリア
			if (goal->IsGoal()) isclear = true;
		}
		for (std::unique_ptr<Item>& item : items_)
		{
			item->Update();
		}


		for (Object3d*& object : objects) object->Update();

		//カメラ
		camera_->Update();
		lightGroup_->Update();
		pm_->Update();

		if (isGameover)
		{
			sceneManager_->ChangeScene("GAMEOVER", stageNum);
			isGameover = false;
		}
		if (isclear)
		{
			sceneManager_->ChangeScene("STAGECLEAR", stageNum);
			isclear = false;
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

	if (stageNum == 100)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			nullptr, nullptr, spriteTutorialInfo1);
	}
	else if (stageNum == 101)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			spriteTutorialHTPMoveBack, nullptr, spriteTutorialInfo2);
	}
	else if (stageNum == 102)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			spriteTutorialHTPMoveBack, spriteTutorialHTPAttack, spriteTutorialInfo3);
	}
	else if (stageNum == 103)
	{
		SettingTutorialEase(1, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			spriteTutorialHTPMoveBack, spriteTutorialHTPAttack, spriteTutorialInfo4);
	}
	UpdateTutorialSprite();
	//天球ぐるぐる
	for (Object3d*& skydome : skydomes)
	{
		//天球回転用
		XMFLOAT3 rotSkydome = skydome->GetPosition();
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;

		skydome->SetRotation(rotSkydome);

		skydome->Update();
	}
	
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
	for (auto& skydome : skydomes)skydome->Draw();
	for (auto& object : objects)object->Draw();

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

		if (stageNum == 100)
		{
			DrawTutorialSprite(spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
				nullptr, nullptr, spriteTutorialInfo1);
		}

		else if (stageNum == 101)
		{
			DrawTutorialSprite(spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
				spriteTutorialHTPMoveBack, nullptr, spriteTutorialInfo2);
		}
		else if (stageNum == 102)
		{
			DrawTutorialSprite(spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
				spriteTutorialHTPMoveBack, spriteTutorialHTPAttack, spriteTutorialInfo3);
		}
		else if (stageNum == 103)
		{
			DrawTutorialSprite(spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
				spriteTutorialHTPMoveBack, spriteTutorialHTPAttack, spriteTutorialInfo4);
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
	for (Object3d*& skydome : skydomes)delete skydome;
	objects.clear();
	skydomes.clear();

	//3Dモデル
	delete modelPlayer_;
	delete modelPlayerBullet_;
	delete modelEnemy1_;
	delete modelEnemyBullet_;
	delete modelBoss1_;
	delete modelBossCore1_;
	delete modelStageT;
	delete modelStage1;
	delete modelStage2;
	delete modelItemJump_;
	delete modelItemHeal_;
	delete modelSpike_;
	delete modelGround;
	delete modelBox;
	delete modelGoal_;

	models.clear();

	//スプライト
	delete spritePause_;
	delete spritePauseInfo_;

	delete spriteTutorialInfo1;
	delete spriteTutorialInfo2;
	delete spriteTutorialInfo3;
	delete spriteTutorialInfo4;

	delete spriteTutorialHTPDash;
	delete spriteTutorialHTPMove;
	delete spriteTutorialHTPJump;
	delete spriteTutorialHTPMoveBack;
	delete spriteTutorialHTPAttack;
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

			newenemy = enemyFactory->CreateEnemy(objectData.objectPattern,
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

			newGimmick = gimmickFactory->CreateGimmick(objectData.objectPattern, model, player.get());

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
			objects.push_back(newObject);
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
			skydomes.push_back(newObject);
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
			objects.push_back(newObject);
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
	modelStageT = Model::LoadFromOBJ("skydomet");
	modelStage1 = Model::LoadFromOBJ("skydome");
	modelStage2 = Model::LoadFromOBJ("skydome2");
	modelGround = Model::LoadFromOBJ("ground");
	modelBox = Model::LoadFromOBJ("sphere2", true);

	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("playerbullet", modelPlayerBullet_));
	models.insert(std::make_pair("enemy1", modelEnemy1_));
	models.insert(std::make_pair("enemybullet", modelEnemyBullet_));
	models.insert(std::make_pair("boss1", modelBoss1_));
	models.insert(std::make_pair("core1", modelBossCore1_));
	models.insert(std::make_pair("sphere", modelGoal_));
	models.insert(std::make_pair("Itemjump", modelItemJump_));
	models.insert(std::make_pair("itemheal", modelItemHeal_));
	models.insert(std::make_pair("spikeball", modelSpike_));
	models.insert(std::make_pair("skydomet", modelStageT));
	models.insert(std::make_pair("skydome", modelStage1));
	models.insert(std::make_pair("skydome2", modelStage2));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("sphere2", modelBox));

}

void GamePlayScene::SettingTutorialEase(int num, Sprite* s1, Sprite* s2,
	Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)
{
	switch (num)
	{
	case 0:
		if (s1 != nullptr)s1->SetPosition({ easeInfo[0].start,50.0f });
		if (s2 != nullptr)s2->SetPosition({ easeInfo[1].start,50.0f });
		if (s3 != nullptr)s3->SetPosition({ easeInfo[2].start,110.0f });
		if (s4 != nullptr)s4->SetPosition({ easeInfo[3].start,110.0f });
		if (s5 != nullptr)s5->SetPosition({ easeInfo[4].start,170.0f });
		if (s6 != nullptr)s6->SetPosition({ easeInfo[5].start,250.0f });
		break;
	case 1:
		for (int i = 0; i < 6; i++)easeInfo[i].ease_out_expo();
		if (s1 != nullptr)s1->SetPosition({ easeInfo[0].num_X,50.0f  });
		if (s2 != nullptr)s2->SetPosition({ easeInfo[1].num_X,50.0f });
		if (s3 != nullptr)s3->SetPosition({ easeInfo[2].num_X,110.0f });
		if (s4 != nullptr)s4->SetPosition({ easeInfo[3].num_X,110.0f });
		if (s5 != nullptr)s5->SetPosition({ easeInfo[4].num_X,110.0f });
		if (s6 != nullptr)s6->SetPosition({ easeInfo[5].num_X,200.0f });
		break;
	}

}

void GamePlayScene::UpdateTutorialSprite()
{
	if (isColorReverse_)speedColor -= 0.02f;
	else speedColor += 0.02f;

	if (speedColor >= 0.9f)
	{
		isColorReverse_ = true;
	}
	if (speedColor <= 0.0f)
	{
		isColorReverse_ = false;
	}

	spriteTutorialHTPDash->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPMove->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPJump->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPMoveBack->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	spriteTutorialHTPAttack->SetColor({ 0.1f,0.1f,0.1f,1.0f });

	//ダッシュ
	if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
	{
		if (input_->PushKey(DIK_A))spriteTutorialHTPDash->SetColor({ 0.1f,0.1f,1.0f,1.0f });
		if (input_->PushKey(DIK_D))spriteTutorialHTPDash->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	}
	else
	{
		if (input_->PushKey(DIK_A))spriteTutorialHTPMove->SetColor({ 0.1f,0.1f,1.0f,1.0f });
		if (input_->PushKey(DIK_D))spriteTutorialHTPMove->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	}
	//ジャンプ
	if (input_->PushKey(DIK_SPACE))spriteTutorialHTPJump->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	//奥側ジャンプ
	if (input_->PushKey(DIK_Z))spriteTutorialHTPMoveBack->SetColor({ 1.0f,0.1f,0.1f,1.0f });
	//攻撃
	if (input_->PushKey(DIK_X))spriteTutorialHTPAttack->SetColor({ 1.0f,0.1f,0.1f,1.0f });

	spriteTutorialInfo1->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
	spriteTutorialInfo2->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
	spriteTutorialInfo3->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
	spriteTutorialInfo4->SetColor({ 0.1f,0.1f,0.1f + speedColor,1.0f });

	spriteTutorialInfo1->Update();
	spriteTutorialInfo2->Update();
	spriteTutorialInfo3->Update();
	spriteTutorialInfo4->Update();

	spriteTutorialHTPMove->Update();
	spriteTutorialHTPDash->Update();
	spriteTutorialHTPJump->Update();
	spriteTutorialHTPMoveBack->Update();
	spriteTutorialHTPAttack->Update();
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
	spriteTutorialInfo1->Initialize(spCommon_, 1000);

	spCommon_->LoadTexture(1001, "texture/info/tinfo2.png");//2
	spriteTutorialInfo2->Initialize(spCommon_, 1001);

	spCommon_->LoadTexture(1002, "texture/info/tinfo3.png");//3
	spriteTutorialInfo3->Initialize(spCommon_, 1002);

	spCommon_->LoadTexture(1003, "texture/info/tinfo4.png");//4
	spriteTutorialInfo4->Initialize(spCommon_, 1003);

	spCommon_->LoadTexture(1004, "texture/info/moveinfo.png");//1~
	spriteTutorialHTPMove->Initialize(spCommon_, 1004);
	spCommon_->LoadTexture(1005, "texture/info/dashinfo.png");//1~
	spriteTutorialHTPDash->Initialize(spCommon_, 1005);
	spCommon_->LoadTexture(1006, "texture/info/jumpinfo.png");//1~
	spriteTutorialHTPJump->Initialize(spCommon_, 1006);
	spCommon_->LoadTexture(1007, "texture/info/backmoveinfo1.png");//2~
	spriteTutorialHTPMoveBack->Initialize(spCommon_, 1007);
	spCommon_->LoadTexture(1008, "texture/info/attackinfo.png");//3~
	spriteTutorialHTPAttack->Initialize(spCommon_, 1008);

	if (stageNum == 100)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			nullptr, nullptr, spriteTutorialInfo1);
	}

	else if (stageNum == 101)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			spriteTutorialHTPMoveBack, nullptr, spriteTutorialInfo2);
	}
	else if (stageNum == 102)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			spriteTutorialHTPMoveBack, spriteTutorialHTPAttack, spriteTutorialInfo3);
	}
	else if (stageNum == 103)
	{
		SettingTutorialEase(0, spriteTutorialHTPMove, spriteTutorialHTPDash, spriteTutorialHTPJump,
			spriteTutorialHTPMoveBack, spriteTutorialHTPAttack, spriteTutorialInfo4);
	}

	spritePause_->Update();
	spritePauseInfo_->Update();

	spriteTutorialInfo1->Update();
	spriteTutorialInfo2->Update();
	spriteTutorialInfo3->Update();
	spriteTutorialInfo4->Update();

	spriteTutorialHTPMove->Update();
	spriteTutorialHTPDash->Update();
	spriteTutorialHTPJump->Update();
	spriteTutorialHTPMoveBack->Update();
	spriteTutorialHTPAttack->Update();

}
