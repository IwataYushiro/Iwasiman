#include "Framework.h"

void Framework::Initialize()
{
	//WinApp
	winApp_ = new WinApp();
	//DXCommon
	dxCommon_ = new DirectXCommon();
	//SpriteCommon
	sprCommon_ = new SpriteCommon();
	//�J����
	camera_ = new Camera();
	//Input
	input_ = new Input();

	//WinApp������
	winApp_->Initialize();
	//DirectX������
	dxCommon_->Initialize(winApp_);
	//�X�v���C�g���
	sprCommon_->Initialize(dxCommon_);
	//����
	input_->Initialize(winApp_);

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
	//WinApp
	winApp_->Finalize();

	//��՗�
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