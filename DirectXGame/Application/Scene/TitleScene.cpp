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
	
	//�v���C���[�֌W
	player_ = new Player();
	//3D�I�u�W�F�N�g����
	object3DPlayer_ = Object3d::Create();
	//OBJ�t�@�C�����烂�f���f�[�^��ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("sphere", true);
	//�I�u�W�F�N�g�Ƀ��f���R�t����
	object3DPlayer_->SetModel(modelPlayer_);
	//�J�������R�Â�
	object3DPlayer_->SetCamera(camera_);
	//�v���C���[������
	player_->Initialize(modelPlayer_, object3DPlayer_, input_, camera_);
	
	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture/title.png");
	spriteTitle_->Initialize(spCommon_, titleTex);

	//FBX
	objF = ObjectFbx::Create();
	modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	objF->SetModelFBX(modelF);
	objF->SetCamera(camera_);
	camera_->SetEye({ 0.0f,0.0f,-70.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });*/

	//�p�[�e�B�N��
	particle1_ = Particle::LoadFromParticleTexture("particle2.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);
}

void TitleScene::Update()
{
	/*if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		sceneManager_->ChangeScene("GAMEPLAY");
	}*/

	//spriteTitle_->Update();
	//�������̕�
	pm1_->Active(particle1_, { -40.0f ,0.0f,0.0f } ,{ 20.0f ,20.0f,20.0f }, { 3.0f,3.0f,3.0f }, { 0.0f,0.001f,0.0f }, 3, { 3.0f, 0.0f });

	//�������̕�
	pm1_->ActiveX(particle1_, { 25.0f ,10.0f,0.0f }, { -4.2f,0.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
	//�������̕�(���C�h)
	//pm1_->ActiveX(particle1_, { 25.0f ,100.0f,0.0f }, { -4.2f,0.2f,0.0f }, { 0.0f,0.001f,0.0f }, 10, { 3.0f, 0.0f });
	//�c�����̕�
	//pm1_->ActiveY(particle1_, { 10.0f ,25.0f,0.0f }, { 0.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
	//�S���ʂɂ͂�����(�N���A���o�Ɏg������)
	//pm1_->ActiveZ(particle1_, { 0.0f ,0.0f,25.0f }, { 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 10, { 3.0f, 0.0f });
	
	//�΂炯��悤��
	//pm1_->ActiveY(particle1_, { 10.0f ,25.0f,0.0f }, { 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
	
	//��Ƃ��J
	//pm1_->ActiveY(particle1_, { 150.0f ,100.0f,0.0f }, { 0.0f,-5.2f,0.0f }, { 0.0f,0.001f,0.0f }, 5, { 5.0f, 0.0f });

	player_->Update();
	pm1_->Update();

	camera_->Update();
	objF->Update();
}

void TitleScene::Draw()
{
	//�w�i�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�X�v���C�g�`��
	//spriteTitle_->Draw();


	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	pm1_->Draw();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();


	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	player_->Draw();
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
	//�v���C���[
	delete object3DPlayer_;
	delete modelPlayer_;
	delete player_;

	//�p�[�e�B�N��
	delete particle1_;
	delete pm1_;
	//FBX
	delete objF;
	delete modelF;
}
