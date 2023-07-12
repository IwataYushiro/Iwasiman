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
	//プレイヤー関係
	player_ = new Player();
	//敵関係
	enemy_ = new Enemy();

	goal_ = new Goal();

	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	//音声データ
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//音声再生呼び出し例
	audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	//3Dオブジェクト関係
	//3Dオブジェクト生成
	object3DPlayer_ = Object3d::Create();
	object3DEnemy_ = Object3d::Create();
	objGoal_ = Object3d::Create();

	//OBJファイルからモデルデータを読み込む
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");
	modelGoal_ = Model::LoadFromOBJ("sphere");

	//オブジェクトにモデル紐付ける
	object3DPlayer_->SetModel(modelPlayer_);
	object3DEnemy_->SetModel(modelEnemy_);
	objGoal_->SetModel(modelGoal_);

	//カメラも紐づけ
	object3DPlayer_->SetCamera(camera_);
	object3DEnemy_->SetCamera(camera_);
	objGoal_->SetCamera(camera_);

	//レベルデータ読み込み
	LoadLVData();

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	//パーティクル
	particle1_ = Particle::LoadFromParticleTexture("particle6.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	particle2_ = Particle::LoadFromParticleTexture("particle5.png");
	pm2_ = ParticleManager::Create();
	pm2_->SetParticleModel(particle2_);
	pm2_->SetCamera(camera_);
	//ポジション
	player_->Initialize(modelPlayer_, object3DPlayer_, input_, camera_);

	enemy_->Initialize(modelEnemy_, object3DEnemy_, camera_);

	goal_->Initialize(modelGoal_, objGoal_, camera_);
	//敵に自機のアドレスを渡す
	enemy_->SetPlayer(player_);

}

void GamePlayScene::Update()
{
	//モデル呼び出し例
	player_->Update();
	enemy_->Update();
	goal_->Update();

	for (auto& object : objects) {
		object->Update();
	}
	lightGroup_->SetPointLightPos(0, player_->GetWorldPosition());
	ChackAllCollisions();
	//カメラ
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

	//スプライト描画前処理
	spCommon_->PreDraw();

	//背景スプライト

	//エフェクト
	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	pm1_->Draw();
	pm2_->Draw();
	player_->DrawParticle();
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	player_->Draw();
	enemy_->Draw();
	goal_->Draw();
	for (auto& object : objects) {
		object->Draw();
	}
	//モデル描画後処理
	Object3d::PostDraw();

	//前景スプライト

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
	delete pm1_;
	delete particle2_;
	delete pm2_;
	//ライト
	delete lightGroup_;
	//モデル
	//3Dオブジェクト
	delete object3DPlayer_;
	delete object3DEnemy_;
	delete objGoal_;

	for (Object3d*& object : objects)
	{
		delete object;
	}
	
	//3Dモデル
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelSkydome;
	delete modelGround;
	delete modelGoal_;
	//基盤系
	delete player_;
	delete enemy_;
	delete goal_;

}
//衝突判定と応答
void GamePlayScene::ChackAllCollisions() {

	//判定対象A,Bの座標
	XMFLOAT3 posA, posB;
	// A,Bの座標の距離用
	XMFLOAT3 posAB;
	//判定対象A,Bの半径
	float radiusA;
	float radiusB;
	float radiiusAB;

	//自機弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	//敵弾リストを取得
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetEnemyBullets();

#pragma region 自機と敵弾の当たり判定
	//それぞれの半径
	radiusA = 1.0f;
	radiusB = 1.0f;

	//自機の座標
	posA = player_->GetWorldPosition();

	//自機と全ての敵弾の当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		//敵弾の座標
		posB = bullet->GetWorldPosition();
		//座標A,Bの距離を求める
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//球と球の交差判定
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			//自キャラの衝突時コールバック関数を呼び出す
			player_->OnCollision();
			//敵弾の衝突時コールバック関数を呼び出す
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region 自弾と敵の当たり判定
	//それぞれの半径
	radiusA = 5.0f;
	radiusB = 1.0f;

	//敵の座標
	posA = enemy_->GetWorldPosition();

	//敵と全ての弾の当たり判定
	for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
		//弾の座標
		posB = bullet->GetWorldPosition();
		//座標A,Bの距離を求める
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//球と球の交差判定
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			//敵キャラの衝突時コールバック関数を呼び出す
			enemy_->OnCollisionPlayer();
			//自機弾の衝突時コールバック関数を呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自機と仮ゴールの当たり判定
	//それぞれの半径
	radiusA = 3.0f;
	radiusB = 10.0f;

	//敵の座標
	posA = player_->GetWorldPosition();
	posB = goal_->GetWorldPosition();

	
		//座標A,Bの距離を求める
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//球と球の交差判定
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			camera_->Reset();
			sceneManager_->ChangeScene("TITLE");
		}
	
#pragma endregion

}

void GamePlayScene::LoadLVData()
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile("stage1");

	// モデル読み込み
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	
	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

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
		DirectX::XMStoreFloat3(&scale,objectData.scale);
		newObject->SetScale(scale);

		newObject->SetCamera(camera_);
		// 配列に登録
		objects.push_back(newObject);
	}

}
