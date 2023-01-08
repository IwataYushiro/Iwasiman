#include "WinApp.h"
#include "Audio.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Object3d.h"
#include "SpriteCommon.h"
#include "Sprite.h"

using namespace DirectX;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#pragma region ポインタ置き場
	WinApp* winApp = new WinApp();

	DirectXCommon* dxCommon = new DirectXCommon();

	SpriteCommon* sprCommon = new SpriteCommon();

	Sprite* sprite = new Sprite();

	Audio* audio = new Audio();

	Input* input = new Input();

	ImGuiManager* imguiManager = new ImGuiManager();

#pragma endregion

#pragma region Windows初期化
	winApp->Initialize();
#pragma endregion
	// DirectX初期化処理　ここから

	//DirectX初期化
	dxCommon->Initialize(winApp);
	
	//スプライト基盤
	sprCommon->Initialize(dxCommon);
	//入力
	input->Initialize(winApp);
	//imgui
	imguiManager->Initialize(winApp);

	// DirectX初期化処理　ここまで

	//一旦ここでimguiテスト
	
	// 描画初期化処理　ここから
#pragma region 描画初期化処理
	
	//音声データ
	audio->Initialize();

	Audio::SoundData sound = audio->SoundLordWave("Resources/TestMusic.wav");
	//音声再生呼び出し例
	audio->SoundPlayWave(audio->GetXAudio2(), sound);

	//ここでテクスチャを指定しよう
	UINT texindex = 00;
	sprCommon->LoadTexture(texindex, "texture.png");
	sprite->Initialize(sprCommon, texindex);
	
	//3Dオブジェクト関係
	Object3d::StaticInitialize(dxCommon->GetDevice(), winApp->window_width, winApp->window_height);
	
	//OBJファイルからモデルデータを読み込む
	Model* model = Model::LoadFromOBJ("triangle_mat");
	//3Dオブジェクト生成
	Object3d* object3d = Object3d::Create();
	//オブジェクトにモデル紐付ける
	object3d->SetModel(model);

#pragma endregion
	// 描画初期化処理　ここまで

	//ゲームループ
	while (true)
	{
		//Windowsのメッセージ処理
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		// DirectX毎フレーム処理　ここから
		
		input->Update();
		// 更新処理ここから
		
		//スプライト呼び出し例
		sprite->Update();
	
		//モデル呼び出し例
		object3d->Update();
		// ここまで
		
		//描画前処理
		dxCommon->PreDraw();

		//背景スプライト描画
		
		//スプライト描画前処理
		sprCommon->PreDraw();
		//スプライト描画
		sprite->Draw();
		
		//モデル描画

		//モデル描画前処理
		object3d->PreDraw(dxCommon->GetCommandList());

		//モデル描画
		object3d->Draw();

		//モデル描画後処理
		object3d->PostDraw();

		//前景スプライト描画 

		//描画後処理

		dxCommon->PostDraw();
		// 4.描画コマンドここまで

		// DirectX毎フレーム処理　ここまで
		
	}
	//終了処理
	audio->Finalize();
	winApp->Finalize();

	//解放
	//各種音声
	audio->SoundUnLoad(&sound);
	//音声全体
	delete audio;

	//スプライト
	delete sprite;

	//モデル
	//3Dオブジェクト
	delete object3d;
	//3Dモデル
	delete model;

	//基盤系
	delete imguiManager;
	delete sprCommon;
	delete input;
	delete dxCommon;
	delete winApp;
	//コンソールへの文字出力
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}