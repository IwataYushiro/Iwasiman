#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;
using namespace IwasiEngine;

/*

*	MyGame.cpp

*	�Q�[���S��

*/

void MyGame::Initialize()
{
	//�t���[�����[�N������
	Framework::Initialize();
	
	//�V�[���t�@�N�g���[
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
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
	//�|�X�g�G�t�F�N�g�̑O�ɃV�[����`��
	sceneManager_->Draw();
	//�`��O����
	dxCommon_->PreDraw();
	//���̌�Ƀ|�X�g�G�t�F�N�g�̕`��
	sceneManager_->DrawPostEffect();
	//ImGui�̕\��
	imguiManager_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	Framework::Finalize();//�I������
}

