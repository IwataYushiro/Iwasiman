#include "MyGame.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	std::unique_ptr<IwasiEngine::Framework> game = std::make_unique<MyGame>();	//ポリモーフィズム
	game->Run();																//実行
	return 0;																	//終わり
}