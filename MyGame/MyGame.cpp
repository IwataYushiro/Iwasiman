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
	object3D_->StaticInitialize(dxCommon_->GetDevice(), winApp_->window_width, winApp_->window_height);

	//OBJファイルからモデルデータを読み込む
	model_ = Model::LoadFromOBJ("triangle_mat");
	//3Dオブジェクト生成
	object3D_ = Object3d::Create();
	//オブジェクトにモデル紐付ける
	object3D_->SetModel(model_);

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
	XMFLOAT3 pos = object3D_->GetPosition();

	if (input_->PushKey(DIK_RIGHT))
	{
		pos.x += 0.1f;
	}
	if (input_->PushKey(DIK_LEFT))
	{
		pos.x -= 0.1f;
	}
	if (input_->PushKey(DIK_UP))
	{
		pos.y += 0.1f;
	}
	if (input_->PushKey(DIK_DOWN))
	{
		pos.y -= 0.1f;
	}

	object3D_->SetPosition(pos);

	object3D_->Update();

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
	object3D_->PreDraw(dxCommon_->GetCommandList());

	//モデル描画
	object3D_->Draw();

	//モデル描画後処理
	object3D_->PostDraw();

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
	delete object3D_;
	//3Dモデル
	delete model_;

	//基盤系
	delete imguiManager_;

	Framework::Finalize();
}
