#include "MyGame.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	MyGame game;
	//初期化
	game.Initialize();

	//ゲームループ
	while (true)
	{
		game.Update();

		//終了リクエストが来たらループを抜ける
		if (game.IsEndRequest())
		{
			//ゲームループを抜ける
			break;
		}
		//描画処理ここから
		game.Draw();
		// 描画処理ここまで

		// DirectX毎フレーム処理　ここまで

	}
	//終了処理
	game.Finalize();

	return 0;
}

