#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	//Sprite
	sprite_ = new Sprite();
	//Audio
	audio_ = new Audio();
	//imgui
	imguiManager_ = new ImGuiManager();

Object3d::StaticInitialize(dxCommon_->GetDevice(), winApp_->window_width, winApp_->window_height);
	
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
	UINT texindex = 00;
	sprCommon_->LoadTexture(texindex, "texture.png");
	sprite_->Initialize(sprCommon_, texindex);

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
	
	//ポジション
	player_->Initialize(modelPlayer_, object3DPlayer_, input_);

	enemy_->Initialize(modelEnemy_, object3DEnemy_);
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

	//スプライト呼び出し例
	sprite_->Update();

	//モデル呼び出し例
	player_->Update();
	enemy_->Update();

	//imgui
	imguiManager_->Update();
	// ここまで
}

void MyGame::Draw()
{
	//描画前処理
	dxCommon_->PreDraw();

	//背景スプライト
	//スプライト描画前処理
	sprCommon_->PreDraw();
	//スプライト描画
	//sprite_->Draw();

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
	delete sprite_;

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
