#include "TitleScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>


DirectXCommon* TitleScene::dxCommon_ = DirectXCommon::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
Audio* TitleScene::audio_ = Audio::GetInstance();
SceneManager* TitleScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* TitleScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* TitleScene::camera_ = Camera::GetInstance();


void TitleScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//�I�[�f�B�I
	audio_->Initialize();

	//�J����
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//camera_->SetTarget({ 90.0f,0.0f,0.0f });
	//camera_->SetEye({ -10.0f,2.0f,0.0f });

	// ���_���W
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// �����_���W
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//���x���f�[�^�ǂݍ���
	//LoadLVData();

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture/title2.png");
	spriteTitle_->Initialize(spCommon_, titleTex);

	UINT titleMenuTex = 01;
	spCommon_->LoadTexture(titleMenuTex, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, titleMenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });

	UINT titleMenuTutorialTex = 02;
	spCommon_->LoadTexture(titleMenuTutorialTex, "texture/titlemenut.png");
	spriteMenuTutorial_->Initialize(spCommon_, titleMenuTutorialTex);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].start,150.0f });

	UINT titleMenuStageSerectTex = 03;
	spCommon_->LoadTexture(titleMenuStageSerectTex, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, titleMenuStageSerectTex);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].start,300.0f });

	UINT titleMenuDoneTex = 04;
	spCommon_->LoadTexture(titleMenuDoneTex, "texture/titlemenud.png");
	spriteMenuDone_->Initialize(spCommon_, titleMenuDoneTex);
	spriteMenuDone_->SetPosition({ easeMenuPosX[3].start,550.0f });


	//FBX
	objF = ObjectFbx::Create();
	modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	objF->SetModelFBX(modelF);
	objF->SetCamera(camera_);
	objF->PlayAnimation();//�X�V�ŌĂԂƎ~�܂邩�璍��

	//�p�[�e�B�N��
	/*particle1_ = Particle::LoadFromParticleTexture("particle2.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);*/

}

void TitleScene::Update()
{
	if (MenuCount <= 0)MenuCount = 0;
	else if (MenuCount >= 1)MenuCount = 1;
	if (isMenu)
	{
		//�C�[�W���O
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 4; i++)easeMenuPosX[i].ease_out_expo();

		//���W�Z�b�g
		spriteTitle_->SetPosition({ easeTitlePosX.num_X,0.0f });
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteMenuDone_->SetPosition({ easeMenuPosX[3].num_X,550.0f });

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))MenuCount--;
		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))MenuCount++;

		if (isColorReverse_)speedColor -= 0.01f;
		else speedColor += 0.01f;
		
		if (speedColor >= 0.9f)
		{
			isColorReverse_ = true;
		}
		if (speedColor <= 0.0f)
		{
			isColorReverse_ = false;
		}

		if (MenuCount == 0)
		{
			spriteMenuTutorial_->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
			spriteMenuStageSelect_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		}
		else if (MenuCount == 1)
		{
			spriteMenuTutorial_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteMenuStageSelect_->SetColor({ 0.1f+speedColor,0.1f,0.1f,1.0f });
		}

		if (input_->TriggerKey(DIK_SPACE))
		{
			if (MenuCount == 0)
			{
				//�`���[�g���A���X�e�[�W
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 1);
			}
			else if (MenuCount == 1)
			{
				//�X�e�[�W�I��
				camera_->Reset();
				sceneManager_->ChangeScene("STAGESELECT", 1);

			}
		}
	}
	else
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			easeTitlePosX.Standby(false);
			for (int i = 0; i < 4; i++)easeMenuPosX[i].Standby(false);
			isMenu = true;

		}

	}

	spriteTitle_->Update();
	spriteMenu_->Update();
	spriteMenuTutorial_->Update();
	spriteMenuStageSelect_->Update();
	spriteMenuDone_->Update();

	/*for (auto& object : objects) {
		object->Update();
	}*/
	camera_->Update();
	lightGroup_->Update();
	//pm1_->Update();

	objF->Update();
	imguiManager_->Begin();

	ImGui::Begin("Prototype");
	ImGui::SetWindowPos(ImVec2(0.0f, 600.0f));
	ImGui::SetWindowSize(ImVec2(800.0f, 100.0f));
	ImGui::Text("How To Play");
	ImGui::Text("WASD Move + SHIFT Dash  SPACE Jump Z Move Back");
	//ImGui::Text(" test -> gaussianblur -> bloom -> Glare -> sepia -> cold -> ");
	//ImGui::Text(" nagapozi -> grayscale -> mosaic -> uv shift -> uv shift blur -> posteffect off");
	ImGui::Text(" Space Sample Game Start      Enter Return Title");

	ImGui::End();
	imguiManager_->End();
}

void TitleScene::Draw()
{
	//�w�i�X�v���C�g�`��O����

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();


	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	/*for (auto& object : objects) {
		object->Draw();
	}*/
	//���f���`��㏈��
	Object3d::PostDraw();

	//Fbx���f���`��O����
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	objF->Draw();
	//Fbx���f���`��㏈��
	ObjectFbx::PostDraw();


	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	//�X�v���C�g�`��
	spriteTitle_->Draw();
	spriteMenu_->Draw();
	spriteMenuTutorial_->Draw();
	spriteMenuStageSelect_->Draw();
	spriteMenuDone_->Draw();

}

void TitleScene::Finalize()
{
	//����
	audio_->Finalize();
	//�X�v���C�g
	delete spriteTitle_;
	delete spriteMenu_;
	delete spriteMenuTutorial_;
	delete spriteMenuStageSelect_;
	delete spriteMenuDone_;

	//�v���C���[
	delete object3DPlayer_;
	delete modelPlayer_;
	//���x���f�[�^�p�I�u�W�F�N�g
	/*for (Object3d*& object : objects)
	{
		delete object;
	}
	delete modelSkydome;
	delete modelGround;
	delete modelFighter;
	delete modelSphere;*/

	//���C�g
	delete lightGroup_;
	//�p�[�e�B�N��
	//delete particle1_;
	//delete pm1_;
	//FBX
	delete objF;
	delete modelF;
}

void TitleScene::LoadLVData()
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile("test");

	// ���f���ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player", true);
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelFighter = Model::LoadFromOBJ("chr_sword", true);
	modelSphere = Model::LoadFromOBJ("sphere", true);

	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("chr_sword", modelFighter));
	models.insert(std::make_pair("sphere", modelSphere));

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (auto& objectData : levelData->objects) {
		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
		Object3d* newObject = Object3d::Create();
		//�I�u�W�F�N�g�Ƀ��f���R�t����
		newObject->SetModel(model);

		// ���W
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		// ��]�p
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, objectData.rot);
		newObject->SetRotation(rot);

		// ���W
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, objectData.scale);
		newObject->SetScale(scale);

		newObject->SetCamera(camera_);
		// �z��ɓo�^
		objects.push_back(newObject);
	}

}

