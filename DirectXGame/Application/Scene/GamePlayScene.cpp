#include "GamePlayScene.h"
#include "TitleScene.h"

using namespace DirectX;

DirectXCommon* GamePlayScene::dxCommon_ = DirectXCommon::GetInstance();
SpriteCommon* GamePlayScene::spCommon_ = SpriteCommon::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
Audio* GamePlayScene::audio_ = Audio::GetInstance();
SceneManager* GamePlayScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GamePlayScene::imguiManager_ = ImGuiManager::GetInstance();

void GamePlayScene::Initialize()
{
	//カメラ
	camera_ = new Camera();
	//プレイヤー関係
	player_ = new Player();
	//敵関係
	enemy_ = new Enemy();

	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	//音声データ
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//音声再生呼び出し例
	audio_->SoundPlayWave(audio_->GetXAudio2(), sound);

	UINT gameClearTex = 02;
	spCommon_->LoadTexture(gameClearTex, "texture/gameclear.png");
	spriteGameClear_->Initialize(spCommon_, gameClearTex);
	spriteGameClear_->SetAnchorPoint({ 0.5f,0.5f });

	UINT gameOverTex = 03;
	spCommon_->LoadTexture(gameOverTex, "texture/gameover.png");
	spriteGameOver_->Initialize(spCommon_, gameOverTex);
	spriteGameOver_->SetAnchorPoint({ -0.5f,-0.5f });

	//3Dオブジェクト関係

	//3Dオブジェクト生成
	object3DPlayer_ = Object3d::Create();
	object3DEnemy_ = Object3d::Create();

	//OBJファイルからモデルデータを読み込む
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");

	//オブジェクトにモデル紐付ける
	object3DPlayer_->SetModel(modelPlayer_);
	object3DEnemy_->SetModel(modelEnemy_);
	//カメラも紐づけ
	object3DPlayer_->SetCamera(camera_);
	object3DEnemy_->SetCamera(camera_);
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
	//敵に自機のアドレスを渡す
	enemy_->SetPlayer(player_);

}

void GamePlayScene::Update()
{
	pm1_->Active(particle1_, 120.0f, 0.2f, 0.001f, 15, { 10.0f, 0.0f });
	pm2_->Active(particle2_, 100.0f, 0.2f, 0.001f, 5, { 6.0f,0.0f });

	//モデル呼び出し例
	player_->Update();
	enemy_->Update();

	ChackAllCollisions();

	spriteGameClear_->Update();
	spriteGameOver_->Update();

	//カメラ
	camera_->Update();

	pm1_->Update();
	pm2_->Update();

	if (input_->TriggerKey(DIK_RETURN))
	{
		BaseScene* scene = new TitleScene();

		sceneManager_->SetNextScene(scene);
	}
}

void GamePlayScene::Draw()
{

	//スプライト描画前処理
	spCommon_->PreDraw();

	//背景スプライト

	spriteGameClear_->Draw();
	spriteGameOver_->Draw();

	//エフェクト
	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	pm1_->Draw();
	pm2_->Draw();

	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	player_->Draw();
	enemy_->Draw();

	//モデル描画後処理
	Object3d::PostDraw();

	//前景スプライト

	//ImGuiの表示

	imguiManager_->Draw();
}

void GamePlayScene::Finalize()
{
	//終了処理
	audio_->Finalize();
	//解放
	//各種音声
	audio_->SoundUnLoad(&sound);
	//スプライト
	delete spriteTitle_;
	delete spriteHowToPlay_;
	delete spriteGameClear_;
	delete spriteGameOver_;
	//パーティクル
	delete particle1_;
	delete pm1_;
	delete particle2_;
	delete pm2_;
	//モデル
	//3Dオブジェクト
	delete object3DPlayer_;
	delete object3DEnemy_;

	//3Dモデル
	delete modelPlayer_;
	delete modelEnemy_;

	//基盤系
	delete player_;
	delete enemy_;

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

}