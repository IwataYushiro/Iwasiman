#include "StageClearScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "StageList.h"

#include <cassert>
#include <sstream>
#include <iomanip>


DirectXCommon* StageClearScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageClearScene::input_ = Input::GetInstance();
Audio* StageClearScene::audio_ = Audio::GetInstance();
SceneManager* StageClearScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageClearScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* StageClearScene::camera_ = Camera::GetInstance();


StageClearScene::StageClearScene(int stagenum) :stageNum_(stagenum){}

void StageClearScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//�I�[�f�B�I
	audio_->Initialize();

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
	if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(StageTex, "texture/gameclear.png");
	else if (stageNum_ == SL_Stage2_AreaBoss)spCommon_->LoadTexture(StageTex, "texture/gameclear.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(StageTex, "texture/gameclear.png");
	else spCommon_->LoadTexture(StageTex, "texture/stageclear.png");
	spriteStageClear_->Initialize(spCommon_, StageTex);

}

void StageClearScene::Update()
{


	if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		if (stageNum_ == SL_Stage1_AreaBoss) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
		else if (stageNum_ == SL_StageTutorial_Final) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
		else sceneManager_->ChangeScene("GAMEPLAY", ++stageNum_);
	}
	imguiManager_->Begin();
	imguiManager_->End();

	spriteStageClear_->Update();

	camera_->Update();
	lightGroup_->Update();

}

void StageClearScene::Draw()
{
	//�w�i�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�X�v���C�g�`��
	spriteStageClear_->Draw();


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


}

void StageClearScene::Finalize()
{
	delete lightGroup_;
	//�X�v���C�g
	delete spriteStageClear_;

}
