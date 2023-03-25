#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();

	//Audio
	audio_ = new Audio();
	//imgui
	imguiManager_ = new ImGuiManager();

	Object3d::StaticInitialize(dxCommon_->GetDevice());
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());

	//プレイヤー関係
	player_ = new Player();
	//敵関係
	enemy_ = new Enemy();

#pragma region Windows初期化
#pragma endregion
	// DirectX初期化処理　ここから
	//imgui
	imguiManager_->Initialize(winApp_, dxCommon_);

	// DirectX初期化処理　ここまで
	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	//音声データ
	audio_->Initialize();
	sound = audio_->SoundLordWave("Resources/TestMusic.wav");
	//音声再生呼び出し例
	audio_->SoundPlayWave(audio_->GetXAudio2(), sound);

	//ここでテクスチャを指定しよう
	UINT titleTex = 00;
	sprCommon_->LoadTexture(titleTex, "texture/title.png");
	spriteTitle_->Initialize(sprCommon_, titleTex);

	UINT howtoplayTex = 01;
	sprCommon_->LoadTexture(howtoplayTex, "texture/howtoplay.png");
	spriteHowToPlay_->Initialize(sprCommon_, howtoplayTex);

	UINT gameClearTex = 02;
	sprCommon_->LoadTexture(gameClearTex, "texture/gameclear.png");
	spriteGameClear_->Initialize(sprCommon_, gameClearTex);

	UINT gameOverTex = 03;
	sprCommon_->LoadTexture(gameOverTex, "texture/gameover.png");
	spriteGameOver_->Initialize(sprCommon_, gameOverTex);

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
	p1=ParticleManager::Create();
	p1->SetCamera(camera_);
	//ポジション
	player_->Initialize(modelPlayer_, object3DPlayer_, input_, camera_);

	enemy_->Initialize(modelEnemy_, object3DEnemy_, camera_);
	//敵に自機のアドレスを渡す
	enemy_->SetPlayer(player_);

	//シーン
	scene_ = title;

#pragma endregion
	// 描画初期化処理　ここまで
}

void MyGame::Update()
{

	// DirectX毎フレーム処理　ここから
	// 更新処理ここから
	Framework::Update();

	switch (scene_)
	{
	case title:
		for (int i = 0; i < 20; i++)
		{
			//X,Y,Z全て{-20.0f,20.0f}でランダムに分布
			const float md_pos = 40.0f;
			XMFLOAT3 pos{};
			pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			//X,Y,Z全て{0.1f,0.1f}でランダムに分布
			const float md_vel = 0.2f;
			XMFLOAT3 vel{};
			vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			//重力に見立ててYのみ{0.001f,0}でランダムに分布
			XMFLOAT3 acc{};
			const float md_acc = 0.001f;
			acc.y = -(float)rand() / RAND_MAX * md_acc;

			//追加
			p1->Add(60, pos, vel, acc, 3.0f, 0.0f);
		}

		p1->Update();
		if (input_->TriggerKey(DIK_SPACE))
		{
			player_->Reset();
			enemy_->Reset();
			scene_ = howtoplay;

			break;
		}
		spriteTitle_->Update();
		break;

	case howtoplay:
		if (input_->TriggerKey(DIK_SPACE))
		{
			scene_ = stage;
			break;
		}
		spriteHowToPlay_->Update();
		break;

	case stage:
		//カメラ
		camera_->Update();

		//モデル呼び出し例
		player_->Update();
		enemy_->Update();

		ChackAllCollisions();

		if (player_->IsDead())
		{
			scene_ = gameover;
			break;
		}
		if (enemy_->IsDead())
		{
			scene_ = clear;
			break;
		}
		break;

	case clear:
		if (input_->TriggerKey(DIK_SPACE))
		{
			scene_ = title;
			break;
		}
		spriteGameClear_->Update();

		break;
	case gameover:
		if (input_->TriggerKey(DIK_SPACE))
		{
			scene_ = title;
			break;
		}
		spriteGameOver_->Update();

		break;


	}

	//imgui
	imguiManager_->Update();
	// ここまで
}

void MyGame::Draw()
{
	//描画前処理
	dxCommon_->PreDraw();


	//スプライト描画前処理
	sprCommon_->PreDraw();

	//背景スプライト
	switch (scene_)
	{
	case title:
		//スプライト描画
		//spriteTitle_->Draw();
		break;
	
	case howtoplay:
		spriteHowToPlay_->Draw();
		break;
	
	case stage:

		break;
	
	case clear:
		spriteGameClear_->Draw();
		break;

	case gameover:
		spriteGameOver_->Draw();
		break;


	}

	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//モデル描画
	switch (scene_)
	{
	case title:

		break;
	case howtoplay:

		break;
	case stage:
player_->Draw();
	enemy_->Draw();

		break;
	case clear:

		break;
	case gameover:

		break;

	}
	
	//モデル描画後処理
	Object3d::PostDraw();
	//エフェクト
	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	switch (scene_)
	{
	case title:
		p1->Draw();
		break;
	case howtoplay:

		break;
	case stage:

		break;
	case clear:

		break;
	case gameover:

		break;

	}

	//エフェクト描画後処理
	ParticleManager::PostDraw();
	//前景スプライト

	//ImGuiの表示
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	//終了処理
	imguiManager_->Finalize();
	audio_->Finalize();

	//解放
	//各種音声
	audio_->SoundUnLoad(&sound);
	//音声全体
	delete audio_;

	//スプライト
	delete spriteTitle_;
	delete spriteHowToPlay_;
	delete spriteGameClear_;
	delete spriteGameOver_;
	//パーティクル
	delete p1;
	//delete p2;
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
	delete imguiManager_;


	Framework::Finalize();
}

//衝突判定と応答
void MyGame::ChackAllCollisions() {

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