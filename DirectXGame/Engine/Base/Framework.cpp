#include "Framework.h"
#include "DirectionalLight.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "ParticleManager.h"

void Framework::Initialize()
{
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
	//�J����
	camera_ = Camera::GetInstance();
	
	//WinApp������
	winApp_->Initialize();
	//DirectX������
	dxCommon_->Initialize(winApp_);
	//�X�v���C�g���
	sprCommon_->Initialize(dxCommon_);
	//����
	input_->Initialize(winApp_);
	//imgui
	imguiManager_->Initialize(winApp_, dxCommon_);
	//FBX
	FbxLoader::GetInstance()->Initialize(dxCommon_->GetDevice());
	
	Object3d::StaticInitialize(dxCommon_->GetDevice());
	ObjectFbx::StaticInitialize(dxCommon_->GetDevice());
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());
	//���C�g
	DirectionalLight::StaticInitialize(dxCommon_->GetDevice());
	
	

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
	sceneManager_->Update();
	imguiManager_->Begin();
#ifdef _DEBUG
	camera_->DebugCamera();
	
#endif // DEBUG
ImGui::Begin("Post Loop");
	ImGui::SetWindowPos(ImVec2(0, 600));
	ImGui::SetWindowSize(ImVec2(800, 100));
	ImGui::Text("0 change");
	ImGui::Text(" test -> gaussianblur -> bloom -> Glare -> sepia -> cold -> ");
	ImGui::Text(" nagapozi -> grayscale -> mosaic -> uv shift -> uv shift blur -> posteffect off");

	ImGui::End();
	imguiManager_->End();
	
}

void Framework::Finalize()
{
	//scene
	sceneManager_->Finalize();
	delete sceneFactory_;
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