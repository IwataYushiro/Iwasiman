#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	BaseScene* scene = new TitleScene();

	sceneManager_->SetNextScene(scene);
	
}

void MyGame::Update()
{
	// DirectX���t���[�������@��������
	// �X�V������������
	Framework::Update();

}

void MyGame::Draw()
{
	//�`��O����
	dxCommon_->PreDraw();

	sceneManager_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	Framework::Finalize();
}
