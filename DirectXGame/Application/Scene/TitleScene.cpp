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
	camera_->SetEye({ 0.0f, 1.0f, -50.0f });
	// 注視点座標
	camera_->SetTarget({ 0.0f,0.0f,50.0f });

	//レベルデータ読み込み
	LoadLVData("scene/title");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(TITitle, "texture/title2.png");
	spriteTitle_->Initialize(spCommon_, TITitle);

	spCommon_->LoadTexture(TIMenu, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, TIMenu);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });
	spriteMenu_->SetColor(otherMenuColor);

	spCommon_->LoadTexture(TIMenuTutorial, "texture/titlemenut.png");
	spriteMenuTutorial_->Initialize(spCommon_, TIMenuTutorial);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].start,150.0f });

	spCommon_->LoadTexture(TIMenuStageSerect, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, TIMenuStageSerect);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].start,300.0f });

	spCommon_->LoadTexture(TIMenuDone, "texture/titlemenud.png");
	spriteMenuDone_->Initialize(spCommon_, TIMenuDone);
	spriteMenuDone_->SetPosition({ easeMenuPosX[3].start,550.0f });
	spriteMenuDone_->SetColor(otherMenuColor);

	spCommon_->LoadTexture(TIBackTitle, "texture/back.png");
	spriteBack_->Initialize(spCommon_, TIBackTitle);
	spriteBack_->SetPosition({ easeMenuPosX[4].start,50.0f });
	spriteBack_->SetColor(backTitleColor);


	//FBX
	//objF = ObjectFbx::Create();
	//modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	//objF->SetModelFBX(modelF);
	//objF->SetCamera(camera_);
	//objF->PlayAnimation();//更新で呼ぶと止まるから注意

	//パーティクル
	particle1_ = Particle::LoadFromParticleTexture("particle1.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

}

void TitleScene::Update()
{
	if (MenuCount <= Tutorial)MenuCount = Tutorial;
	else if (MenuCount >= StageSelect)MenuCount = StageSelect;
	if (isMenu)
	{
		//イージング
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 5; i++)easeMenuPosX[i].ease_out_expo();

		//座標セット
		spriteTitle_->SetPosition({ easeTitlePosX.num_X,0.0f });
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteMenuDone_->SetPosition({ easeMenuPosX[3].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[4].num_X,50.0f });

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))MenuCount--;
		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))MenuCount++;

		
		//色を変えるスピード
		float speedColor = 0.02f;

		DirectX::XMFLOAT4 selectMenuColor={ 0.1f + selectColor.x,0.1f + selectColor.y,0.1f + selectColor.z,1.0f };
		
		if (isColorReverse_)selectColor.x -= speedColor;
		else selectColor.x += speedColor;

		const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.9f,0.0f };//{max,min}

		if (selectColor.x >= maxAndMinSpeedColor.x)
		{
			isColorReverse_ = true;
		}
		if (selectColor.x <= maxAndMinSpeedColor.y)
		{
			isColorReverse_ = false;
		}

		if (MenuCount == Tutorial)
		{
			spriteMenuTutorial_->SetColor(selectMenuColor);
			spriteMenuStageSelect_->SetColor(otherMenuColor);
		}
		else if (MenuCount == StageSelect)
		{
			spriteMenuTutorial_->SetColor(otherMenuColor);
			spriteMenuStageSelect_->SetColor(selectMenuColor);
		}

		if (input_->TriggerKey(DIK_SPACE))
		{
			if (MenuCount == Tutorial)
			{
				//チュートリアルステージ
				sceneManager_->ChangeScene("GAMEPLAY", 100);
			}
			else if (MenuCount == StageSelect)
			{
				//ステージ選択
				sceneManager_->ChangeScene("STAGESELECT", 0);

			}
		}
		if (easeMenuPosX[4].num_X == easeMenuPosX[4].end)
		{
			if (input_->TriggerKey(DIK_Q))
			{
				easeTitlePosX.Standby(true);
				for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(true);
				isBack = true;
				isMenu = false;
			}
		}

	}
	else if (isBack)
	{
		//イージング
		easeTitlePosX.ease_out_expo();
		for (int i = 0; i < 5; i++)easeMenuPosX[i].ease_out_expo();

		//座標セット
		spriteTitle_->SetPosition({ easeTitlePosX.num_X,0.0f });
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteMenuDone_->SetPosition({ easeMenuPosX[3].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[4].num_X,50.0f });

		if (easeMenuPosX[4].num_X == easeMenuPosX[4].start)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				easeTitlePosX.Standby(false);
				for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(false);
				isMenu = true;
				isBack = false;
			}
		}
	}
	else
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			easeTitlePosX.Standby(false);
			for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(false);
			isMenu = true;

		}

	}

	spriteTitle_->Update();
	spriteMenu_->Update();
	spriteMenuTutorial_->Update();
	spriteMenuStageSelect_->Update();
	spriteMenuDone_->Update();
	spriteBack_->Update();

	for (Object3d*& player : objPlayers_)
	{
		pm1_->ActiveX(particle1_, player->GetPosition(), { 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f }, { 0.0f,0.001f,0.0f }, 3, { 1.0f, 0.0f });

		player->Update();
	}

	for (Object3d*& ground : objGrounds_)
	{
		DirectX::XMFLOAT3 move = ground->GetPosition();
		DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };

		move.x += speed.x;
		ground->SetPosition(move);
		if (ground->GetPosition().x <= returnPos)ground->SetPosition(startGroundPos);

		ground->Update();
	}

	for (Object3d*& skydome : objSkydomes_)
	{
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);

		skydome->Update();
	}
		

	camera_->Update();
	lightGroup_->Update();
	pm1_->Update();

	//objF->Update();
	imguiManager_->Begin();
	camera_->DebugCamera(true);
	imguiManager_->End();
}

void TitleScene::Draw()
{
	//背景スプライト描画前処理


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& ground : objGrounds_)ground->Draw();
	for (Object3d*& skydome : objSkydomes_)skydome->Draw();
	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//objF->Draw();
	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	pm1_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	spCommon_->PreDraw();
	//前景スプライト
	//スプライト描画
	spriteTitle_->Draw();
	spriteMenu_->Draw();
	spriteMenuTutorial_->Draw();
	spriteMenuStageSelect_->Draw();
	spriteMenuDone_->Draw();
	spriteBack_->Draw();

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
	delete spriteBack_;


	//レベルデータ用オブジェクト
	for (Object3d*& player : objPlayers_)delete player;
	for (Object3d*& ground : objGrounds_)delete ground;
	for (Object3d*& skydome : objSkydomes_)delete skydome;

	delete modelPlayer_;
	delete modelSkydome;
	delete modelGround;

	//ライト
	delete lightGroup_;
	//パーティクル
	delete particle1_;
	delete pm1_;
	//FBX
	//delete objF;
	//delete modelF;
}

void TitleScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile(stagePath);

	// モデル読み込み
	modelPlayer_ = Model::LoadFromOBJ("player", true);
	modelSkydome = Model::LoadFromOBJ("skydomet");
	modelGround = Model::LoadFromOBJ("ground");

	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("skydomet", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		if (objectData.objectType.find("PLAYER") == 0)
		{
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
			objPlayers_.push_back(newObject);
		}
		else if (objectData.objectType.find("PLANE") == 0)
		{
			// モデルを指定して3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			//オブジェクトにモデル紐付ける
			newObject->SetModel(model);

			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);
			startGroundPos = newObject->GetPosition();

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
			objGrounds_.push_back(newObject);
		}
		else if (objectData.objectType.find("SKYDOME") == 0)
		{
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
			objSkydomes_.push_back(newObject);
		}

	}

}

