#include "MyGame.h"
#include "SceneFactory.h"

using namespace DirectX;

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
	if (input_->TriggerKey(DIK_0))
	{
		postCount++;
	}
	
}

void MyGame::Draw()
{
	pe1->PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	pe1->PostDraw(dxCommon_->GetCommandList());

	//�`��O����
	dxCommon_->PreDraw();
	
	//�|�X�g�G�t�F�N�g�̕`��
	//PostDraw();
	pe1->Draw(dxCommon_->GetCommandList());

	//ImGui�̕\��
	imguiManager_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	PostDelete();
	Framework::Finalize();
}

void MyGame::PostInitialize()
{
	pe1 = new PostEffect();

	pe1->Initialize(sprCommon_, "Test");

}

void MyGame::PostDraw()
{
	//int Variation = postCount % POST_NUM;
	//�|�X�g�G�t�F�N�g�̕`��
	//pe[Variation].Draw(dxCommon_->GetCommandList());
	

	
}


void MyGame::PostDelete()
{
	//delete[] pe;

	delete pe1;
	delete pe2;
}
