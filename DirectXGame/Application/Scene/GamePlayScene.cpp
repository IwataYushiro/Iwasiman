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

	//OBJファイルからモデルデータを読み込む
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");
	modelGoal_ = Model::LoadFromOBJ("sphere");

	//プレイヤーの初期化
	player_ = Player::Create(modelPlayer_);
	player_->SetCamera(camera_);
	player_->Update();

	//ゴール初期化
	goal_ = Goal::Create(modelGoal_);
	goal_->SetCamera(camera_);
	goal_->Update();

	UpdateEnemyPopCommands();

	//弾リセット
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Reset();
	}
	// モデル読み込み
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelBox = Model::LoadFromOBJ("sphere2", true);

	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("sphere2", modelBox));

	//レベルデータ読み込み
	LoadLVData();

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
		player_->Update();

		goal_->Update();

		for (auto& object : objects) object->Update();

		lightGroup_->SetPointLightPos(0, player_->GetWorldPosition());

		//カメラ
		camera_->Update();
		lightGroup_->Update();
		pm_->Update();

		//かめおべら
		if (player_->GetPosition().y <= -60.0f)
		{
			isGameover = true;
		}
		if (isGameover)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				camera_->Reset();
				sceneManager_->ChangeScene("TITLE");
			}
		}
		//クリア
		else if (goal_->IsGoal())
		{
			isclear = true;
		}
		if (isclear)
		{
			//ここでイージングの準備
			es.Standby(false);
			isBack = false;
			spriteClear_->SetPosition({ es.start,0.0f });

			//イージングサンプル(ポーズ中に準備してもここがやってくれる)
			es.ease_in_cubic();
			spriteClear_->SetPosition({ es.num_X,0.0f });
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
		spriteClear_->Update();
		spriteGameover_->Update();
	}
	else if (isPause_)
	{
		//イージングサンプル(ポーズ中に準備してもここがやってくれる)
		es.ease_in_out_circ();
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
	

}

void GamePlayScene::Draw()
{
	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	player_->Draw();
	for (std::unique_ptr<Enemy>& enemy : enemys_) enemy->Draw();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();
	goal_->Draw();
	for (auto& object : objects)object->Draw();
	
	//モデル描画後処理
	Object3d::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	pm_->Draw();
	player_->DrawParticle();
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//スプライト描画前処理
	spCommon_->PreDraw();
	//前景スプライト
	if (isPause_)spritePause_->Draw();
	else if (isclear)spriteClear_->Draw();
	else if (isGameover)spriteGameover_->Draw();
	else spritePauseInfo_->Draw();
	//ImGuiの表示
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

	for (Object3d*& object : objects)
	{
		delete object;
	}
	objects.clear();

	//3Dモデル
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelSkydome;
	delete modelGround;
	delete modelBox;
	delete modelGoal_;
	//スプライト
	delete spritePause_;
	delete spriteClear_;
	delete spritePauseInfo_;
	delete spriteGameover_;
	//基盤系
	delete player_;
	delete goal_;

}

void GamePlayScene::LoadLVData()
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile("stage1");

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

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

void GamePlayScene::LoadEnemyPopData()
{
	//ファイルを開く
	std::ifstream file;
	file.open("Resources/csv/enemyPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GamePlayScene::UpdateEnemyPopCommands()
{
	LoadEnemyPopData();
	//1行分の文字列を入れる関数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の戦闘文字列を取得
		getline(line_stream, word, ',');

		// "//"=コメント
		if (word.find("//") == 0)
		{
			//コメント行を飛ばす
			continue;
		}
		//POP
		if (word.find("POP") == 0)
		{
			//x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			//z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());
			//敵発生
			//敵初期化
			std::unique_ptr<Enemy> newenemy;
			newenemy = Enemy::Create(modelEnemy_, player_, this);
			newenemy->SetPosition(XMFLOAT3(x, y, z));
			newenemy->SetCamera(camera_);

			newenemy->Update();

			//リストに登録
			enemys_.push_back(std::move(newenemy));

		}
	}
}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	//リストに登録
	enemyBullets_.push_back(std::move(enemyBullet));
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
}
