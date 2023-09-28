#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
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

StageSelectScene::StageSelectScene()
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

	UINT MenuTex = 00;
	spCommon_->LoadTexture(MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });

	UINT MenuTutorialTex = 01;
	spCommon_->LoadTexture(MenuTutorialTex, "texture/titlemenut.png");
	spriteTutorial_->Initialize(spCommon_, MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX[1].start,150.0f });

	UINT Menustage1Tex = 02;
	spCommon_->LoadTexture(Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX[2].start,300.0f });

	UINT Menustage2Tex = 03;
	spCommon_->LoadTexture(Menustage2Tex, "texture/stagetower.png");
	spriteStage2_->Initialize(spCommon_, Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX[3].start,450.0f });

	UINT MenuDoneTex = 04;
	spCommon_->LoadTexture(MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX[4].start,580.0f });

	UINT BackTex = 05;
	spCommon_->LoadTexture(BackTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, BackTex);
	spriteBack_->SetPosition({ easeMenuPosX[5].start,50.0f });
	spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });

	modelStageTutorial = Model::LoadFromOBJ("skydomet");
	modelStage1 = Model::LoadFromOBJ("skydome");
	modelStage2 = Model::LoadFromOBJ("skydome2");
	
	objStage = Object3d::Create();
	objStage->SetModel(modelStageTutorial);
	objStage->SetCamera(camera_);
	objStage->SetScale({ 7.0f,7.0f,7.0f });

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

	easeTitlePosX.Standby(false);
	for (int i = 0; i < 6; i++)easeMenuPosX[i].Standby(false);
}

void StageSelectScene::Update()
{
	if (MenuCount <= 0)MenuCount = 0;
	else if (MenuCount >= 2)MenuCount = 2;
	if (!outStageSerect)
	{
		//イージング
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 6; i++)easeMenuPosX[i].ease_out_expo();

		//座標セット
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteStage1_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteStage2_->SetPosition({ easeMenuPosX[3].num_X,450.0f });
		spriteDone_->SetPosition({ easeMenuPosX[4].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[5].num_X,50.0f });

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))MenuCount--;
		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))MenuCount++;

		if (isColorReverse_)speedColor -= 0.02f;
		else speedColor += 0.02f;

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
			objStage->SetModel(modelStageTutorial);
			spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteTutorial_->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
			spriteStage1_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
		}
		else if (MenuCount == 1)
		{
			objStage->SetModel(modelStage1);
			spriteMenu_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteTutorial_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteStage1_->SetColor({ 0.1f + speedColor,0.1f,0.1f,1.0f });
			spriteStage2_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteDone_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
			spriteBack_->SetColor({ 0.0f,0.0f,0.1f,1.0f });
		}
		else if (MenuCount == 2)
		{
			objStage->SetModel(modelStage2);
			spriteMenu_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteTutorial_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteStage1_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteStage2_->SetColor({ 1.0f,speedColor + 0.1f,speedColor + 0.1f,1.0f });
			spriteDone_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			spriteBack_->SetColor({ 0.5f,0.5f,1.0f,1.0f });
		}

		if (input_->TriggerKey(DIK_SPACE))
		{
			if (MenuCount == 0)
			{
				//チュートリアルステージ
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 1);
			}
			else if (MenuCount == 1)
			{
				//ステージ1
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 3);

			}
			else if (MenuCount == 2)
			{
				//ステージ2
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 4);

			}
		}
		if (easeMenuPosX[5].num_X == easeMenuPosX[5].end)
		{
			if (input_->TriggerKey(DIK_Q))
			{
				easeTitlePosX.Standby(true);
				for (int i = 0; i < 6; i++)easeMenuPosX[i].Standby(true);
				outStageSerect = true;
			}
		}
	}
	else
	{
		//イージング
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 6; i++)easeMenuPosX[i].ease_out_expo();

		//座標セット
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteStage1_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteStage2_->SetPosition({ easeMenuPosX[3].num_X,450.0f });
		spriteDone_->SetPosition({ easeMenuPosX[4].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[5].num_X,50.0f });

		if (easeMenuPosX[5].num_X == easeMenuPosX[5].start)
		{
			sceneManager_->ChangeScene("TITLE");
		}
	}
	rot.y += 0.5f;

	objStage->SetRotation(rot);

	spriteMenu_->Update();
	spriteTutorial_->Update();
	spriteStage1_->Update();
	spriteStage2_->Update();
	spriteDone_->Update();
	spriteBack_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage->Update();
}

void StageSelectScene::Draw()
{

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	
	objStage->Draw();

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
	delete objStage;

	delete modelStageTutorial;
	delete modelStage1;
	delete modelStage2;
	
	//ライト
	delete lightGroup_;
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile(stagePath);
	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData->objects) {

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
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
		objects.push_back(newObject);
		

	}

}
