#include "Framework.h"
#include "LightGroup.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "ParticleManager.h"

using namespace IwasiEngine;

/*

*	Framework.cpp

*	�Q�[���S�̃t���[�����[�N

*/

void Framework::Initialize()
{
	//�C���X�^���X�擾
	//WinApp
	winApp_ = WinApp::GetInstance();
	//DXCommon
	dxCommon_ = DirectXCommon::GetInstance();
	//SpriteCommon
	sprCommon_ = SpriteCommon::GetInstance();
	//�I�[�f�B�I
	audio_ = Audio::GetInstance();
	//Input
	input_ = Input::GetInstance();
	//SceneManager
	sceneManager_ = SceneManager::GetInstance();
	//imgui
	imguiManager_ = ImGuiManager::GetInstance();

	//WinApp������
	winApp_->Initialize();
	//DirectX������
	dxCommon_->Initialize(winApp_);
	//�X�v���C�g���
	sprCommon_->Initialize(dxCommon_);
	//����
	input_->Initialize();
	//imgui
	imguiManager_->Initialize(winApp_, dxCommon_);
	//FBX
	FbxLoader::GetInstance()->Initialize(dxCommon_->GetDevice());
	//�ÓI������
	//3D�I�u�W�F�N�g(.obj)
	Object3d::StaticInitialize(dxCommon_->GetDevice());
	//3D�I�u�W�F�N�g(.fbx)
	ObjectFbx::StaticInitialize(dxCommon_->GetDevice());
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());
	//���C�g
	LightGroup::StaticInitialize(dxCommon_->GetDevice());

}

void Framework::Update()
{
	//Windows�̃��b�Z�[�W����
	if (winApp_->ProcessMessage())
	{
		//�Q�[���I��
		EndGame_ = true;
	}
	//�|�X�g�G�t�F�N�g
	//postEffect_->Update();

	//���͂̍X�V
	input_->Update();

	//�V�[���X�V
	sceneManager_->Update();


}

void Framework::Finalize()
{
	//�V�[��
	sceneManager_->Finalize();
	//FBX
	FbxLoader::GetInstance()->Finalize();
	//imgui
	imguiManager_->Finalize();
	//WinApp
	winApp_->Finalize();
}


void Framework::Run()
{
	//������
	Initialize();

	//�Q�[�����[�v
	while (true)
	{
		//�X�V
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