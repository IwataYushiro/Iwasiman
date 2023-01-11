#include "MyGame.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Framework* game = new MyGame();
	
	game->Run();

	delete game;

	return 0;
}

