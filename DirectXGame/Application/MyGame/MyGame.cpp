#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	scene = new GamePlayScene();
	scene->Initialize();
	
}

void MyGame::Update()
{

	// DirectX���t���[�������@��������
	// �X�V������������
	Framework::Update();

	scene->Update();
}

void MyGame::Draw()
{
	//�`��O����
	dxCommon_->PreDraw();

	scene->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	scene->Finalize();
	
	Framework::Finalize();
}
