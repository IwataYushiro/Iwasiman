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
	Object3d::StaticInitialize(dxCommon_->GetDevice(), winApp_->window_width, winApp_->window_height);
	
	//OBJファイルからモデルデータを読み込む
	
	modelSkyDome_ = Model::LoadFromOBJ("enemybullet");
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");
	
	//3Dオブジェクト生成
	object3DSkyDome_ = Object3d::Create();
	object3DPlayer_ = Object3d::Create();
	object3DEnemy_ = Object3d::Create();

	//オブジェクトにモデル紐付ける
	object3DSkyDome_->SetModel(modelSkyDome_);
	object3DPlayer_->SetModel(modelPlayer_);
	object3DEnemy_->SetModel(modelEnemy_);
	
	//ポジション
	player_->Initialize(modelPlayer_, object3DPlayer_, input_);

	object3DEnemy_->SetPosition({ 0.0f,0.0f,1000.0f });
	
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
	XMFLOAT3 enemyPos = object3DEnemy_->GetPosition();

	enemyPos.z -= 0.2f;

	object3DEnemy_->SetPosition(enemyPos);

	object3DSkyDome_->Update();

	object3DEnemy_->Update();
	

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
	sprite_->Draw();

	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	
	//モデル描画
	object3DSkyDome_->Draw();
	player_->Draw();
	object3DEnemy_->Draw();

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
	delete object3DSkyDome_;
	//3Dモデル
	delete modelPlayer_;
	delete modelEnemy_;
	delete modelSkyDome_;

	//基盤系
	delete imguiManager_;
	delete player_;

	Framework::Finalize();
}
