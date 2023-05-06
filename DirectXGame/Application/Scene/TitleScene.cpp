#include "TitleScene.h"
#include "FbxLoader.h"

DirectXCommon* TitleScene::dxCommon_ = DirectXCommon::GetInstance();
SpriteCommon* TitleScene::spCommon_ = SpriteCommon::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
Audio* TitleScene::audio_ = Audio::GetInstance();
SceneManager* TitleScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* TitleScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* TitleScene::camera_ = Camera::GetInstance();


void TitleScene::Initialize()
{
	//�I�[�f�B�I
	audio_->Initialize();

	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture/title.png");
	spriteTitle_->Initialize(spCommon_, titleTex);

	FbxLoader::GetInstance()->LoadModelFromFile("spheref");
	//�f�o�C�X�Z�b�g
	ObjectFbx::SetDevice(dxCommon_->GetDevice());
	//�J�����Z�b�g
	ObjectFbx::SetCamera(camera_);
	//�O���t�B�b�N�X�p�C�v���C������
	ObjectFbx::CreateGraphicsPipeline();
}

void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GAMEPLAY");
	}
	spriteTitle_->Update();
}

void TitleScene::Draw()
{
	//�w�i�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�X�v���C�g�`��
	spriteTitle_->Draw();


	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();


	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//���f���`��㏈��
	Object3d::PostDraw();

	//�O�i�X�v���C�g

	//ImGui�̕\��
	imguiManager_->Draw();
}

void TitleScene::Finalize()
{
	//�X�v���C�g
	delete spriteTitle_;
}
