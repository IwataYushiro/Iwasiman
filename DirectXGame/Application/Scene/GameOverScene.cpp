#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>


DirectXCommon* GameOverScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();
Audio* GameOverScene::audio_ = Audio::GetInstance();
SceneManager* GameOverScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameOverScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* GameOverScene::camera_ = Camera::GetInstance();


void GameOverScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//�I�[�f�B�I
	audio_->Initialize();

	BGM = audio_->SoundLoadWave("Resources/sound/bgm/gameover.wav");
	doneSE = audio_->SoundLoadWave("Resources/sound/se/done.wav");

	audio_->SoundPlayWave(audio_->GetXAudio2(), BGM, false);

	// ���_���W
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// �����_���W
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//���x���f�[�^�ǂݍ���
	//LoadLVData();

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT StageTex = 00;
	spCommon_->LoadTexture(StageTex, "texture/gameover.png");
	spriteGameOver_->Initialize(spCommon_, StageTex);

}

void GameOverScene::Update()
{


	if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		audio_->SoundPlayWave(audio_->GetXAudio2(), doneSE, false);
		sceneManager_->ChangeScene("TITLE");

	}
	spriteGameOver_->Update();
	camera_->Update();
	lightGroup_->Update();

}

void GameOverScene::Draw()
{

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();


	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());


	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();

	//�O�i�X�v���C�g
//�w�i�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�X�v���C�g�`��
	spriteGameOver_->Draw();



}

void GameOverScene::Finalize()
{
	//����
	audio_->Finalize();
	audio_->SoundUnLoad(&BGM);
	audio_->SoundUnLoad(&doneSE);

	delete lightGroup_;
	//�X�v���C�g
	delete spriteGameOver_;

}


