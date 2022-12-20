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

	Input* input = new Input();

#pragma endregion

#pragma region Windows初期化
	winApp->Initialize();
#pragma endregion
	// DirectX初期化処理　ここから

	//DirectX初期化
	dxCommon->Initialize(winApp);
	// DirectX初期化処理　ここまで

	// 描画初期化処理　ここから
#pragma region 描画初期化処理

	//入力
	input->Initialize(winApp);

	//スプライト関係
	sprCommon->Initialize(dxCommon);
	sprite->Initialize(sprCommon,input);

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
		float flipX = sprite->GetFlipX();
		float flipY = sprite->GetFlipY();
		XMFLOAT2 pos = sprite->GetPosition();
		
		//flipX = true;
		flipY = true;

		pos.x = 300.0f;
		pos.y = 300.0f;

		sprite->SetFlipX(flipX);
		sprite->SetFlipY(flipY);
		sprite->SetPosition(pos);

		sprite->Update();
		// ここまで
		
		//描画前処理
		dxCommon->PreDraw();

		//背景スプライト描画
		sprCommon->PreDraw();
		sprite->Draw();
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
	delete sprCommon;
	delete input;
	delete dxCommon;
	delete winApp;
	//コンソールへの文字出力
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}