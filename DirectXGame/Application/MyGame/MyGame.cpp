#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;

/*

*	MyGame.cpp

*	�Q�[���S��

*/

void MyGame::Initialize()
{
	Framework::Initialize();
	
	
	sceneFactory_ = new SceneFactory();
	sceneManager_->SetSceneFactory(sceneFactory_);
	sceneManager_->ChangeScene("TITLE");

	//�|�X�g�G�t�F�N�g
	PostInitialize();
}

void MyGame::Update()
{
	// DirectX���t���[�������@��������
	// �X�V������������
	Framework::Update();
}

void MyGame::Draw()
{
	PostDraw();
}

void MyGame::Finalize()
{
	PostDelete();
	Framework::Finalize();
}

void MyGame::PostInitialize()
{
	pe_ = new PostEffect();
	pe_->Initialize(sprCommon_,"Cold");
}

void MyGame::PostDraw()
{
	pe_->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	pe_->PostDraw(dxCommon_->GetCommandList());

	//�`��O����
	dxCommon_->PreDraw();

	//�|�X�g�G�t�F�N�g�̕`��
	pe_->Draw(dxCommon_->GetCommandList());

	//ImGui�̕\��
	imguiManager_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}


void MyGame::PostDelete()
{
	delete pe_;
}
