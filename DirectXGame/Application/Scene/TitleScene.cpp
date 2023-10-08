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


TitleScene::TitleScene(int stagenum) : stageNum_(stagenum) {}

void TitleScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ easeEyeMenu_[0].start, easeEyeMenu_[1].start, easeEyeMenu_[2].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetMenu_[0].start, easeTargetMenu_[1].start, easeTargetMenu_[2].start });

	//レベルデータ読み込み
	if (stageNum_ == SL_Default)LoadLVData("scene/titlet");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/title1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/title2");
	else LoadLVData("scene/titlet");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(TSTI_TitleTex, "texture/title2.png");
	spriteTitle_->Initialize(spCommon_, TSTI_TitleTex);
	spriteTitle_->SetColor(backTitleColor_);

	spCommon_->LoadTexture(TSTI_TitleDoneTex, "texture/space.png");
	spriteTitleDone_->Initialize(spCommon_, TSTI_TitleDoneTex);
	spriteTitleDone_->SetPosition({ 0.0f,550.0f });

	spCommon_->LoadTexture(TSTI_MenuTex, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, TSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[0].start,0.0f });
	spriteMenu_->SetColor(otherMenuColor_);

	spCommon_->LoadTexture(TSTI_MenuTutorialTex, "texture/titlemenut.png");
	spriteMenuTutorial_->Initialize(spCommon_, TSTI_MenuTutorialTex);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].start,150.0f });

	spCommon_->LoadTexture(TSTI_MenuStageSerectTex, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, TSTI_MenuStageSerectTex);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].start,300.0f });

	spCommon_->LoadTexture(TSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteMenuDone_->Initialize(spCommon_, TSTI_MenuDoneTex);
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].start,550.0f });
	spriteMenuDone_->SetColor(otherMenuColor_);

	spCommon_->LoadTexture(TSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, TSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[4].start,50.0f });
	spriteBack_->SetColor(backTitleColor_);


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
	if (menuCount_ <= TSMI_Tutorial)menuCount_ = TSMI_Tutorial;
	else if (menuCount_ >= TSMI_StageSelect)menuCount_ = TSMI_StageSelect;

	DirectX::XMFLOAT4 TitleDoneColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	//SetColorより前に呼び出す
	UpdateChangeColor();

	if (isStartGame_)UpdateIsStartGame();
	else if (isMenu_)UpdateIsMenu();
	else if (isBack_)UpdateIsBack();
	else
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			for (int i = 0; i < 2; i++)easeTitlePosX_[i].Standby(false);
			for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(false);
			for (int i = 0; i < 3; i++)easeEyeMenu_[i].Standby(false);
			for (int i = 0; i < 3; i++)easeTargetMenu_[i].Standby(false);
			isMenu_ = true;

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
		//X値がここまで来たらループ
		const float returnPos_ = -120.0f;

		move.x += speed.x;
		ground->SetPosition(move);
		if (ground->GetPosition().x <= returnPos_)ground->SetPosition(startPos_);

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
	for (Object3d*& goal : objGoals_)goal->Update();

	camera_->Update();
	lightGroup_->Update();
	pm1_->Update();

	//objF->Update();

	imguiManager_->Begin();

	camera_->DebugCamera(false);

	imguiManager_->End();
}

void TitleScene::UpdateIsStartGame()
{
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeGameStart_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetGameStart_[i].ease_out_expo();

	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[4].num_X,50.0f });

	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[0].num_X, easeEyeGameStart_[1].num_X, easeEyeGameStart_[2].num_X });
	camera_->SetTarget({ easeTargetGameStart_[0].num_X, easeTargetGameStart_[1].num_X, easeTargetGameStart_[2].num_X });

	for (Object3d*& goal : objGoals_)
	{

		DirectX::XMFLOAT3 move = goal->GetPosition();
		DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };
		//X値がここまで来たらゲームスタート
		const float gameStartPos_ = 8.0f;

		move.x += speed.x;
		goal->SetPosition(move);
		if (goal->GetPosition().x <= gameStartPos_)
		{

			//チュートリアルステージへ
			sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);

		};


	}

}

void TitleScene::UpdateIsBack()
{
	//イージング
	for (int i = 0; i < 2; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetMenu_[i].ease_out_expo();
	//座標セット
	spriteTitle_->SetPosition({ easeTitlePosX_[0].num_X,0.0f });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[1].num_X,550.0f });
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[4].num_X,50.0f });

	//カメラもセット
	camera_->SetEye({ easeEyeMenu_[0].num_X, easeEyeMenu_[1].num_X, easeEyeMenu_[2].num_X });
	camera_->SetTarget({ easeTargetMenu_[0].num_X, easeTargetMenu_[1].num_X, easeTargetMenu_[2].num_X });

	if (easeMenuPosX_[4].num_X == easeMenuPosX_[4].start)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			for (int i = 0; i < 2; i++)easeTitlePosX_[i].Standby(false);
			for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(false);
			for (int i = 0; i < 3; i++)easeEyeMenu_[i].Standby(false);
			for (int i = 0; i < 3; i++)easeTargetMenu_[i].Standby(false);
			isMenu_ = true;
			isBack_ = false;
		}
	}
}

void TitleScene::UpdateIsMenu()
{
	DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };

	//イージング
	for (int i = 0; i < 2; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetMenu_[i].ease_out_expo();

	//座標セット
	spriteTitle_->SetPosition({ easeTitlePosX_[0].num_X,0.0f });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[1].num_X,550.0f });
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[3].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[4].num_X,50.0f });

	//カメラもセット
	camera_->SetEye({ easeEyeMenu_[0].num_X, easeEyeMenu_[1].num_X, easeEyeMenu_[2].num_X });
	camera_->SetTarget({ easeTargetMenu_[0].num_X, easeTargetMenu_[1].num_X, easeTargetMenu_[2].num_X });

	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	if (menuCount_ == TSMI_Tutorial)
	{
		spriteMenuTutorial_->SetColor(selectMenuColor);
		spriteMenuStageSelect_->SetColor(otherMenuColor_);
	}
	else if (menuCount_ == TSMI_StageSelect)
	{
		spriteMenuTutorial_->SetColor(otherMenuColor_);
		spriteMenuStageSelect_->SetColor(selectMenuColor);
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (menuCount_ == TSMI_Tutorial)
		{
			for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < 3; i++)easeEyeGameStart_[i].Standby(false);
			for (int i = 0; i < 3; i++)easeTargetGameStart_[i].Standby(false);
			isStartGame_ = true;
		}
		else if (menuCount_ == TSMI_StageSelect)
		{
			//ステージ選択
			if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
			else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
			else sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);//チュートリアルに飛ばすと本末転倒
		}
	}
	if (easeMenuPosX_[4].num_X == easeMenuPosX_[4].end)
	{
		if (input_->TriggerKey(DIK_Q))
		{
			for (int i = 0; i < 2; i++)easeTitlePosX_[i].Standby(true);
			for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < 3; i++)easeEyeMenu_[i].Standby(true);
			for (int i = 0; i < 3; i++)easeTargetMenu_[i].Standby(true);
			isBack_ = true;
			isMenu_ = false;
		}
	}

}

void TitleScene::Draw()
{
	//背景スプライト描画前処理


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& ground : objGrounds_)ground->Draw();
	for (Object3d*& skydome : objSkydomes_)skydome->Draw();
	for (Object3d*& goal : objGoals_)goal->Draw();
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
	for (Object3d*& goal : objGoals_)delete goal;

	delete modelPlayer_;
	delete modelSkydome_;
	delete modelSkydomeStage1_;
	delete modelSkydomeStage2_;
	delete modelGround_;
	delete modelGoal_;

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
	modelSkydome_ = Model::LoadFromOBJ("skydomet");
	modelSkydomeStage1_ = Model::LoadFromOBJ("skydome");
	modelSkydomeStage2_ = Model::LoadFromOBJ("skydome2");
	modelGround_ = Model::LoadFromOBJ("ground");
	modelGoal_ = Model::LoadFromOBJ("sphere");

	models_.insert(std::make_pair("player", modelPlayer_));
	models_.insert(std::make_pair("skydomet", modelSkydome_));
	models_.insert(std::make_pair("skydome", modelSkydomeStage1_));
	models_.insert(std::make_pair("skydome2", modelSkydomeStage2_));
	models_.insert(std::make_pair("ground", modelGround_));
	models_.insert(std::make_pair("sphere", modelGoal_));

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
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
			startPos_ = newObject->GetPosition();

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
		else if (objectData.objectType.find("GOAL") == 0)
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
			objGoals_.push_back(newObject);
		}

	}

}

void TitleScene::UpdateChangeColor()
{
	//色を変えるスピード
	float speedColor = 0.02f;

	if (isColorReverse_)
	{
		selectColor_.x -= speedColor;
		selectColor_.y -= speedColor;
		selectColor_.z -= speedColor;
	}

	else
	{
		selectColor_.x += speedColor;
		selectColor_.y += speedColor;
		selectColor_.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.9f,0.0f };//{max,min}

	if (selectColor_.x >= maxAndMinSpeedColor.x)
	{
		isColorReverse_ = true;
	}
	if (selectColor_.x <= maxAndMinSpeedColor.y)
	{
		isColorReverse_ = false;
	}

}
