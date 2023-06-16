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
	PostEffect::PreDraw(dxCommon_->GetCommandList());
	sceneManager_->Draw();
	PostEffect::PostDraw(dxCommon_->GetCommandList());

	//�`��O����
	dxCommon_->PreDraw();
	
	//�|�X�g�G�t�F�N�g�̕`��
	PostDraw();
	
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
	pe1_ = new PostEffect();
	pe1_->Initialize(sprCommon_);

	pe2_ = new PostEffect();
	pe2_->Initialize(sprCommon_, "Cold");

	pe3_ = new PostEffect();
	pe3_->Initialize(sprCommon_, "Sepia");
}

void MyGame::PostDraw()
{
	//���v
	const int POST_NUM = 3;
	//�|�X�g�G�t�F�N�g�̕`��
	if (postCount % POST_NUM == 0)
	{
		pe1_->Draw(dxCommon_->GetCommandList());
	}
	else if (postCount % POST_NUM == 1)
	{
		pe2_->Draw(dxCommon_->GetCommandList());
	}
	else if (postCount % POST_NUM == 2)
	{
		pe3_->Draw(dxCommon_->GetCommandList());
	}
}


void MyGame::PostDelete()
{
	delete pe1_;
	delete pe2_;
	delete pe3_;
}
