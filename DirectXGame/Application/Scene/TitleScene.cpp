#include "TitleScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "StageList.h"

#include <cassert>
#include <sstream>
#include <iomanip>

DirectXCommon* TitleScene::dxCommon_ = DirectXCommon::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
Audio* TitleScene::audio_ = Audio::GetInstance();
SceneManager* TitleScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* TitleScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* TitleScene::camera_ = Camera::GetInstance();


TitleScene::TitleScene(int stagenum) : stageNum(stagenum){}

void TitleScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ easeEye[0].start, easeEye[1].start, easeEye[2].start });
	// 注視点座標
	camera_->SetTarget({ easeTarget[0].start, easeTarget[1].start, easeTarget[2].start });

	//レベルデータ読み込み
	if (stageNum == SL_Default)LoadLVData("scene/titlet");
	else if (stageNum <= SL_Stage1_StageID)LoadLVData("scene/title1");
	else if (stageNum <= SL_Stage2_StageID)LoadLVData("scene/title2");
	else LoadLVData("scene/titlet");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(TSTI_TitleTex, "texture/title2.png");
	spriteTitle_->Initialize(spCommon_, TSTI_TitleTex);
	spriteTitle_->SetColor(backTitleColor);

	spCommon_->LoadTexture(TSTI_TitleDoneTex, "texture/space.png");
	spriteTitleDone_->Initialize(spCommon_, TSTI_TitleDoneTex);
	spriteTitleDone_->SetPosition({ 0.0f,550.0f });

	spCommon_->LoadTexture(TSTI_MenuTex, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, TSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });
	spriteMenu_->SetColor(otherMenuColor);

	spCommon_->LoadTexture(TSTI_MenuTutorialTex, "texture/titlemenut.png");
	spriteMenuTutorial_->Initialize(spCommon_, TSTI_MenuTutorialTex);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].start,150.0f });

	spCommon_->LoadTexture(TSTI_MenuStageSerectTex, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, TSTI_MenuStageSerectTex);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].start,300.0f });

	spCommon_->LoadTexture(TSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteMenuDone_->Initialize(spCommon_, TSTI_MenuDoneTex);
	spriteMenuDone_->SetPosition({ easeMenuPosX[3].start,550.0f });
	spriteMenuDone_->SetColor(otherMenuColor);

	spCommon_->LoadTexture(TSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, TSTI_BackTitleTex);
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
	//メニュー上下限
	if (MenuCount <= TSMI_Tutorial)MenuCount = TSMI_Tutorial;
	else if (MenuCount >= TSMI_StageSelect)MenuCount = TSMI_StageSelect;

	
	DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor.x,0.1f,0.1f,1.0f };
	DirectX::XMFLOAT4 TitleDoneColor = { 0.0f,0.0f,0.1f + selectColor.z,1.0f };
	//SetColorより前に呼び出す
	UpdateChangeColor();

	
	if (isMenu)
	{
		//イージング
		for (int i = 0; i < 2; i++)easeTitlePosX[i].ease_out_expo();
		for (int i = 0; i < 5; i++)easeMenuPosX[i].ease_out_expo();
		for (int i = 0; i < 3; i++)easeEye[i].ease_out_expo();
		for (int i = 0; i < 3; i++)easeTarget[i].ease_out_expo();

		//座標セット
		spriteTitle_->SetPosition({ easeTitlePosX[0].num_X,0.0f});
		spriteTitleDone_->SetPosition({ easeTitlePosX[1].num_X,550.0f });
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteMenuDone_->SetPosition({ easeMenuPosX[3].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[4].num_X,50.0f });

		//カメラもセット
		camera_->SetEye({easeEye[0].num_X, easeEye[1].num_X, easeEye[2].num_X});
		camera_->SetTarget({easeTarget[0].num_X, easeTarget[1].num_X, easeTarget[2].num_X});

		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))MenuCount--;
		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))MenuCount++;

		if (MenuCount == TSMI_Tutorial)
		{
			spriteMenuTutorial_->SetColor(selectMenuColor);
			spriteMenuStageSelect_->SetColor(otherMenuColor);
		}
		else if (MenuCount == TSMI_StageSelect)
		{
			spriteMenuTutorial_->SetColor(otherMenuColor);
			spriteMenuStageSelect_->SetColor(selectMenuColor);
		}

		if (input_->TriggerKey(DIK_SPACE))
		{
			if (MenuCount == TSMI_Tutorial)
			{
				//チュートリアルステージ
				sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);
			}
			else if (MenuCount == TSMI_StageSelect)
			{
				//ステージ選択
				if(stageNum <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT",SSSMI_Stage1_SkyStage );
				else if (stageNum <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
				else sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);//チュートリアルに飛ばすと本末転倒
			}
		}
		if (easeMenuPosX[4].num_X == easeMenuPosX[4].end)
		{
			if (input_->TriggerKey(DIK_Q))
			{
				for (int i = 0; i < 2; i++)easeTitlePosX[i].Standby(true);
				for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(true);
				for (int i = 0; i < 3; i++)easeEye[i].Standby(true);
				for (int i = 0; i < 3; i++)easeTarget[i].Standby(true);
				isBack = true;
				isMenu = false;
			}
		}

	}
	else if (isBack)
	{
		//イージング
		for (int i = 0; i < 2; i++)easeTitlePosX[i].ease_out_expo();
		for (int i = 0; i < 5; i++)easeMenuPosX[i].ease_out_expo();
		for (int i = 0; i < 3; i++)easeEye[i].ease_out_expo();
		for (int i = 0; i < 3; i++)easeTarget[i].ease_out_expo();
		//座標セット
		spriteTitle_->SetPosition({ easeTitlePosX[0].num_X,0.0f });
		spriteTitleDone_->SetPosition({ easeTitlePosX[1].num_X,550.0f });
		spriteMenu_->SetPosition({ easeMenuPosX[0].num_X,0.0f });
		spriteMenuTutorial_->SetPosition({ easeMenuPosX[1].num_X,150.0f });
		spriteMenuStageSelect_->SetPosition({ easeMenuPosX[2].num_X,300.0f });
		spriteMenuDone_->SetPosition({ easeMenuPosX[3].num_X,550.0f });
		spriteBack_->SetPosition({ easeMenuPosX[4].num_X,50.0f });

		//カメラもセット
		camera_->SetEye({ easeEye[0].num_X, easeEye[1].num_X, easeEye[2].num_X });
		camera_->SetTarget({ easeTarget[0].num_X, easeTarget[1].num_X, easeTarget[2].num_X });

		if (easeMenuPosX[4].num_X == easeMenuPosX[4].start)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				for (int i = 0; i < 2; i++)easeTitlePosX[i].Standby(false);
				for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(false);
				for (int i = 0; i < 3; i++)easeEye[i].Standby(false);
				for (int i = 0; i < 3; i++)easeTarget[i].Standby(false);
				isMenu = true;
				isBack = false;
			}
		}
	}
	else
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			for (int i = 0; i < 2; i++)easeTitlePosX[i].Standby(false);
			for (int i = 0; i < 5; i++)easeMenuPosX[i].Standby(false);
			for (int i = 0; i < 3; i++)easeEye[i].Standby(false);
			for (int i = 0; i < 3; i++)easeTarget[i].Standby(false);
			isMenu = true;

		}

	}
	spriteTitleDone_->SetColor(TitleDoneColor);
	spriteMenu_->SetColor(TitleDoneColor);
	spriteMenuDone_->SetColor(TitleDoneColor);



	spriteTitle_->Update();
	spriteTitleDone_->Update();
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
		if (ground->GetPosition().x <= returnPos)ground->SetPosition(startPos);

		ground->Update();
	}

	for (Object3d*& skydome : objSkydomes_)
	{
		//天球回転用
		DirectX::XMFLOAT3 rotSkydome = skydome->GetRotation();

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
	camera_->DebugCamera(false);
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
	spriteTitleDone_->Draw();
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
	delete spriteTitleDone_;
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
	delete modelSkydomeStage1;
	delete modelSkydomeStage2;
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
	modelSkydomeStage1 = Model::LoadFromOBJ("skydome");
	modelSkydomeStage2 = Model::LoadFromOBJ("skydome2");
	modelGround = Model::LoadFromOBJ("ground");

	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("skydomet", modelSkydome));
	models.insert(std::make_pair("skydome", modelSkydomeStage1));
	models.insert(std::make_pair("skydome2", modelSkydomeStage2));
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
			startPos = newObject->GetPosition();

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

void TitleScene::UpdateChangeColor()
{
	//色を変えるスピード
	float speedColor = 0.02f;

	if (isColorReverse_)
	{
		selectColor.x -= speedColor;
		selectColor.y -= speedColor;
		selectColor.z -= speedColor;
	}

	else
	{
		selectColor.x += speedColor;
		selectColor.y += speedColor;
		selectColor.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.9f,0.0f };//{max,min}

	if (selectColor.x >= maxAndMinSpeedColor.x)
	{
		isColorReverse_ = true;
	}
	if (selectColor.x <= maxAndMinSpeedColor.y)
	{
		isColorReverse_ = false;
	}

}
