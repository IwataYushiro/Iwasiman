#include "MyGame.h"
// Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();	//�|�����[�t�B�Y��
	game->Run();													//���s
	return 0;														//�I���
}