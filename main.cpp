#include "WinApp.h"
#include "DirectXCommon.h"
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

	Input* input = new Input();

#pragma endregion

#pragma region Windows初期化
	winApp->Initialize();
#pragma endregion
	// DirectX初期化処理　ここから

	//DirectX初期化
	dxCommon->Initialize(winApp);
	
	//スプライト関係
	sprCommon->Initialize(dxCommon);
	
	//3Dオブジェクト関係
	Object3d::StaticInitialize(dxCommon->GetDevice(), winApp->window_width, winApp->window_height);
	
	//OBJファイルからモデルデータを読み込む
	Model* model = Model::LoadFromOBJ("triangle_mat");
	//3Dオブジェクト生成
	Object3d* object3d = Object3d::Create();
	//オブジェクトにモデル紐付ける
	object3d->SetModel(model);

	// DirectX初期化処理　ここまで


	// 描画初期化処理　ここから
#pragma region 描画初期化処理

	//入力
	input->Initialize(winApp);

	
	//ここでテクスチャを指定しよう
	UINT texindex = 00;
	sprCommon->LoadTexture(texindex, "texture.png");
	sprite->Initialize(sprCommon, texindex);

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
		
		// 更新処理ここから
		input->Update();

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
	winApp->Finalize();

	//解放
	//スプライト
	delete sprite;

	//モデル
	//3Dオブジェクト
	delete object3d;
	//3Dモデル
	delete model;

	//基盤系
	delete sprCommon;
	delete input;
	delete dxCommon;
	delete winApp;
	//コンソールへの文字出力
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}