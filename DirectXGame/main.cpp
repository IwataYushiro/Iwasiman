#include "MyGame.h"

// Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Framework* game = new MyGame();
	
	game->Run();

	delete game;

	return 0;
}

