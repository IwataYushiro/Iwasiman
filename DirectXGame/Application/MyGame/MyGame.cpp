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
	/*if (input_->TriggerMouse(0))
	{
		postCount++;
	}*/
	Variation = postCount % POST_NUM;
}

void MyGame::Draw()
{
	
	if (Variation == 11)
	{
		dxCommon_->PreDraw();

		//�|�X�g�G�t�F�N�g�̕`��
		sceneManager_->Draw();

		//ImGui�̕\��
		imguiManager_->Draw();
		//�`��㏈��
		dxCommon_->PostDraw();
	}
	else
	{
		PostDraw();
	}
}

void MyGame::Finalize()
{
	PostDelete();
	Framework::Finalize();
}

void MyGame::PostInitialize()
{
	
	pe = new PostEffect[POST_NUM];

	pe[0].Initialize(sprCommon_);
	pe[1].Initialize(sprCommon_, "GaussianBlur");
	pe[2].Initialize(sprCommon_, "Bloom");
	pe[3].Initialize(sprCommon_, "CrossFilter");
	pe[4].Initialize(sprCommon_, "Sepia");
	pe[5].Initialize(sprCommon_, "Cold");
	pe[6].Initialize(sprCommon_, "NegaPozi");
	pe[7].Initialize(sprCommon_, "GrayScale");
	pe[8].Initialize(sprCommon_, "Mosaic");
	pe[9].Initialize(sprCommon_, "UVShift");
	pe[10].Initialize(sprCommon_, "UVShiftBlur");
	
	
}

void MyGame::PostDraw()
{
	pe[Variation].PreDraw(dxCommon_->GetCommandList());
		sceneManager_->Draw();
		pe[Variation].PostDraw(dxCommon_->GetCommandList());

		//�`��O����
		dxCommon_->PreDraw();

		//�|�X�g�G�t�F�N�g�̕`��
		pe[Variation].Draw(dxCommon_->GetCommandList());

		//ImGui�̕\��
		imguiManager_->Draw();
		//�`��㏈��
		dxCommon_->PostDraw();

	
}


void MyGame::PostDelete()
{
	delete[] pe;
}
