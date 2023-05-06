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

	//FBX
	objF = ObjectFbx::Create();
	modelF = FbxLoader::GetInstance()->LoadModelFromFile("cube2");
	objF->SetModelFBX(modelF);
	objF->SetCamera(camera_);
	camera_->SetEye({ 0.0f,0.0f,200.0f });
}

void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		sceneManager_->ChangeScene("GAMEPLAY");
	}
	spriteTitle_->Update();
	camera_->Update();
	objF->Update();
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

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	objF->Draw();
	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�O�i�X�v���C�g

	//ImGui�̕\��
	imguiManager_->Draw();
}

void TitleScene::Finalize()
{
	//�X�v���C�g
	delete spriteTitle_;
	//FBX
	delete objF;
	delete modelF;
}
