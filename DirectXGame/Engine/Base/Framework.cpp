#include "Framework.h"
#include "Object3d.h"
#include "ParticleManager.h"

void Framework::Initialize()
{
	//WinApp
	winApp_ = new WinApp();
	//DXCommon
	dxCommon_ = new DirectXCommon();
	//SpriteCommon
	sprCommon_ = new SpriteCommon();
	//�I�[�f�B�I
	audio_ = new Audio();
	//Input
	input_ = new Input();
	//imgui
	imguiManager_ = new ImGuiManager();

	//WinApp������
	winApp_->Initialize();
	//DirectX������
	dxCommon_->Initialize(winApp_);
	//�X�v���C�g���
	sprCommon_->Initialize(dxCommon_);
	//�I�[�f�B�I
	audio_->Initialize();
	//����
	input_->Initialize(winApp_);
	//imgui
	imguiManager_->Initialize(winApp_, dxCommon_);

	Object3d::StaticInitialize(dxCommon_->GetDevice());
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());
}

void Framework::Update()
{
	//Windows�̃��b�Z�[�W����
	if (winApp_->ProcessMessage())
	{
		//�Q�[���I��
		EndGame_ = true;
	}
	//���͂̍X�V
	input_->Update();
	
}

void Framework::Finalize()
{
	//imgui
	imguiManager_->Finalize();
	//WinApp
	winApp_->Finalize();

	//��՗�
	delete imguiManager_;
	delete audio_;
	delete sprCommon_;
	delete input_;
	delete dxCommon_;
	delete winApp_;
}


void Framework::Run()
{
	//������
	Initialize();

	//�Q�[�����[�v
	while (true)
	{
		Update();

		//�I�����N�G�X�g�������烋�[�v�𔲂���
		if (IsEndRequest())
		{
			//�Q�[�����[�v�𔲂���
			break;
		}
		//�`�揈����������
		Draw();
		// �`�揈�������܂�

		// DirectX���t���[�������@�����܂�

	}
	//�I������
	Finalize();
}