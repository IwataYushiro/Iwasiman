#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();
	
	
	sceneFactory_ = new SceneFactory();
	sceneManager_->SetSceneFactory(sceneFactory_);
	sceneManager_->ChangeScene("TITLE");
}

void MyGame::Update()
{
	// DirectX���t���[�������@��������
	// �X�V������������
	Framework::Update();
	
}

void MyGame::Draw()
{
	postEffect_->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	postEffect_->PostDraw(dxCommon_->GetCommandList());

	//�`��O����
	dxCommon_->PreDraw();
	
	//�|�X�g�G�t�F�N�g�̕`��
	postEffect_->Draw(dxCommon_->GetCommandList());
	
	//ImGui�̕\��
	imguiManager_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	Framework::Finalize();
}
