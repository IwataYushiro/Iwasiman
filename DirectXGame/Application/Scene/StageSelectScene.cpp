#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include "StageList.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

DirectXCommon* StageSelectScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageSelectScene::input_ = Input::GetInstance();
Audio* StageSelectScene::audio_ = Audio::GetInstance();
SceneManager* StageSelectScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageSelectScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* StageSelectScene::camera_ = Camera::GetInstance();

StageSelectScene::StageSelectScene(int count) :menuCount_(count)
{
}

void StageSelectScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	//カメラ
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//camera_->SetTarget({ 90.0f,0.0f,0.0f });
	//camera_->SetEye({ -10.0f,2.0f,0.0f });

	// 視点座標
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// 注視点座標
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//レベルデータ読み込み
	//LoadLVData("enemytest");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(SSSTI_MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, SSSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[0].start,0.0f });

	spCommon_->LoadTexture(SSSTI_MenuTutorialTex, "texture/titlemenut.png");
	spriteTutorial_->Initialize(spCommon_, SSSTI_MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].start,150.0f });

	spCommon_->LoadTexture(SSSTI_Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, SSSTI_Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX_[2].start,300.0f });

	spCommon_->LoadTexture(SSSTI_Menustage2Tex, "texture/stagetower.png");
	spriteStage2_->Initialize(spCommon_, SSSTI_Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX_[3].start,450.0f });

	spCommon_->LoadTexture(SSSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, SSSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[4].start,580.0f });

	spCommon_->LoadTexture(SSSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, SSSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[5].start,50.0f });
	spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });

	modelStageTutorial_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");
	
	objStage_ = Object3d::Create();
	objStage_->SetModel(modelStageTutorial_);
	objStage_->SetCamera(camera_);
	objStage_->SetScale({ 7.0f,7.0f,7.0f });

	//FBX
	//objF = ObjectFbx::Create();
	//modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	//objF->SetModelFBX(modelF);
	//objF->SetCamera(camera_);
	//objF->PlayAnimation();//更新で呼ぶと止まるから注意

	//パーティクル
	/*particle1_ = Particle::LoadFromParticleTexture("particle2.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);*/

	easeTitlePosX_.Standby(false);
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(false);

}

void StageSelectScene::Update()
{
	if (menuCount_ <= SSSMI_StageTutorial_Tutorial)menuCount_ = SSSMI_StageTutorial_Tutorial;
	else if (menuCount_ >= SSSMI_Stage2_TowerStage)menuCount_ = SSSMI_Stage2_TowerStage;
	if (!outStageSerect_)
	{
		//イージング
		easeTitlePosX_.ease_out_expo();
		for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();

		//座標セット
		spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
		spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
		spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
		spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
		spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

		if (isColorReverse_)speedColor_ -= 0.02f;
		else speedColor_ += 0.02f;

		if (speedColor_ >= 0.9f)
		{
			isColorReverse_ = true;
		}
		if (speedColor_ <= 0.0f)
		{
			isColorReverse_ = false;
		}

		if (menuCount_ == SSSMI_StageTutorial_Tutorial)
		{
			objStage_->SetModel(modelStageTutorial_);
			spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteTutorial_->SetColor({ 0.1f + speedColor_,0.1f,0.1f,1.0f });
			spriteStage1_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
		}
		else if (menuCount_ == SSSMI_Stage1_SkyStage)
		{
			objStage_->SetModel(modelStage1_);
			spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteTutorial_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteStage1_->SetColor({ 0.1f + speedColor_,0.1f,0.1f,1.0f });
			spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
		}
		else if (menuCount_ == SSSMI_Stage2_TowerStage)
		{
			objStage_->SetModel(modelStage2_);
			spriteMenu_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteTutorial_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteStage1_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteStage2_->SetColor({ 1.0f,speedColor_ + 0.1f,speedColor_ + 0.1f,1.0f });
			spriteDone_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteBack_->SetColor({ 0.5f,0.5f,1.0f,1.0f });
		}

		if (input_->TriggerKey(DIK_SPACE))
		{
			if (menuCount_ == SSSMI_StageTutorial_Tutorial)
			{
				//チュートリアルステージ
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);
			}
			else if (menuCount_ == SSSMI_Stage1_SkyStage)
			{
				//ステージ1
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);

			}
			else if (menuCount_ == SSSMI_Stage2_TowerStage)
			{
				//ステージ2
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", SL_Stage2_Area1);

			}
		}
		if (easeMenuPosX_[5].num_X == easeMenuPosX_[5].end)
		{
			if (input_->TriggerKey(DIK_Q))
			{
				easeTitlePosX_.Standby(true);
				for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(true);
				outStageSerect_ = true;
			}
		}
	}
	else
	{
		//イージング
		easeTitlePosX_.ease_out_expo();
		for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();

		//座標セット
		spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
		spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
		spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
		spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
		spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });

		if (easeMenuPosX_[5].num_X == easeMenuPosX_[5].start)
		{
			sceneManager_->ChangeScene("TITLE");
		}
	}
	rot_.y += 0.5f;

	objStage_->SetRotation(rot_);

	imguiManager_->Begin();
	imguiManager_->End();

	spriteMenu_->Update();
	spriteTutorial_->Update();
	spriteStage1_->Update();
	spriteStage2_->Update();
	spriteDone_->Update();
	spriteBack_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage_->Update();
}

void StageSelectScene::Draw()
{

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	
	objStage_->Draw();

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	spCommon_->PreDraw();
	//前景スプライト
	//スプライト描画	
	spriteMenu_->Draw();
	spriteTutorial_->Draw();
	spriteStage1_->Draw();
	spriteStage2_->Draw();
	spriteDone_->Draw();
	spriteBack_->Draw();
}

void StageSelectScene::Finalize()
{
	//音声
	audio_->Finalize();
	//sprite
	delete spriteMenu_;
	delete spriteTutorial_;
	delete spriteStage1_;
	delete spriteStage2_;
	delete spriteDone_;
	delete spriteBack_;
	//ステージ
	delete objStage_;

	delete modelStageTutorial_;
	delete modelStage1_;
	delete modelStage2_;
	
	//ライト
	delete lightGroup_;
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);
	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData_->objects) {

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}
		
		//地形
		// モデルを指定して3Dオブジェクトを生成
		TouchableObject* newObject = TouchableObject::Create(model, false);
		// 座標
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		// 回転角
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, objectData.rot);
		newObject->SetRotation(rot);

		// 座標
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, objectData.scale);
		newObject->SetScale(scale);

		newObject->SetCamera(camera_);


		// 配列に登録
		objects_.push_back(newObject);
		

	}

}
