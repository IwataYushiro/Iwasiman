#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
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
	Sprite* sprite2 = new Sprite();

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
	
	// DirectX初期化処理　ここまで


	// 描画初期化処理　ここから
#pragma region 描画初期化処理

	//入力
	input->Initialize(winApp);

	
	//ここでテクスチャを指定しよう
	UINT texindex1 = 00;
	sprCommon->LoadTexture(texindex1, "texture.png");
	sprite->Initialize(sprCommon, texindex1);
	
	uint16_t texindex2 = 01;
	sprCommon->LoadTexture(texindex2, "reimu.png");
	sprite2->Initialize(sprCommon, texindex2);

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

		XMFLOAT2 pos = sprite2->GetPosition();

		pos.x = 600.0f;
		pos.y = 300.0f;

		sprite->SetPosition(pos);
		sprite2->SetPosition(pos);
	
		sprite2->Update();
		// ここまで
		
		//描画前処理
		dxCommon->PreDraw();

		//背景スプライト描画
		sprCommon->PreDraw();
		sprite->Draw();
		sprite2->Draw();
		//モデル描画

		//前景スプライト描画 

		//描画後処理

		dxCommon->PostDraw();
		// 4.描画コマンドここまで

		// DirectX毎フレーム処理　ここまで
		
	}
	winApp->Finalize();

	//解放
	delete sprite;
	delete sprite2;

	delete sprCommon;
	delete input;
	delete dxCommon;
	delete winApp;
	//コンソールへの文字出力
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}