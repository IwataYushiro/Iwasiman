#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	scene_ = new TitleScene();
	scene_->Initialize();
	
}

void MyGame::Update()
{

	// DirectX���t���[�������@��������
	// �X�V������������
	Framework::Update();

	scene_->Update();
}

void MyGame::Draw()
{
	//�`��O����
	dxCommon_->PreDraw();

	scene_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	scene_->Finalize();
	delete scene_;
	
	Framework::Finalize();
}
