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
	PostEffectDraw();//�|�X�g�G�t�F�N�g�`��
}

void MyGame::Finalize()
{
	Framework::Finalize();//�I������
}

void MyGame::PostInitialize()
{
	//�|�X�g�G�t�F�N�g������
	pe_ = std::make_unique<PostEffect>();
	pe_->Initialize(sprCommon_);
}

void MyGame::PostEffectDraw()
{
	//�|�X�g�G�t�F�N�g�̑O�ɃV�[����`��
	pe_->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	pe_->PostDraw(dxCommon_->GetCommandList());

	//�`��O����
	dxCommon_->PreDraw();

	//���̌�Ƀ|�X�g�G�t�F�N�g�̕`��
	pe_->Draw(dxCommon_->GetCommandList());

	//ImGui�̕\��
	imguiManager_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

