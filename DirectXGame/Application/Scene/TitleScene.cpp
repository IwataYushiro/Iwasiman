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
	//LoadLVData();

	//ライトを生成
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
	objF->PlayAnimation();//更新で呼ぶと止まるから注意

	//パーティクル
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
		//イージング
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 4; i++)easeMenuPosX[i].ease_out_expo();

		//座標セット
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
				//チュートリアルステージ
				camera_->Reset();
				sceneManager_->ChangeScene("GAMEPLAY", 1);
			}
			else if (MenuCount == 1)
			{
				//ステージ選択
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
	//背景スプライト描画前処理

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	/*for (auto& object : objects) {
		object->Draw();
	}*/
	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	objF->Draw();
	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();


	spCommon_->PreDraw();
	//前景スプライト
	//スプライト描画
	spriteTitle_->Draw();
	spriteMenu_->Draw();
	spriteMenuTutorial_->Draw();
	spriteMenuStageSelect_->Draw();
	spriteMenuDone_->Draw();

}

void TitleScene::Finalize()
{
	//音声
	audio_->Finalize();
	//スプライト
	delete spriteTitle_;
	delete spriteMenu_;
	delete spriteMenuTutorial_;
	delete spriteMenuStageSelect_;
	delete spriteMenuDone_;

	//プレイヤー
	delete object3DPlayer_;
	delete modelPlayer_;
	//レベルデータ用オブジェクト
	/*for (Object3d*& object : objects)
	{
		delete object;
	}
	delete modelSkydome;
	delete modelGround;
	delete modelFighter;
	delete modelSphere;*/

	//ライト
	delete lightGroup_;
	//パーティクル
	//delete particle1_;
	//delete pm1_;
	//FBX
	delete objF;
	delete modelF;
}

void TitleScene::LoadLVData()
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile("test");

	// モデル読み込み
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

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// モデルを指定して3Dオブジェクトを生成
		Object3d* newObject = Object3d::Create();
		//オブジェクトにモデル紐付ける
		newObject->SetModel(model);

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

