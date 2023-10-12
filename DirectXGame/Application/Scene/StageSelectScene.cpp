#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include "StageList.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

//ステージセレクトシーン

//静的メンバ変数の実体
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
	camera_->SetEye({ easeEyeStageSelect_[0].start, easeEyeStageSelect_[1].start, easeEyeStageSelect_[2].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetStageSelect_[0].start, easeTargetStageSelect_[1].start, easeTargetStageSelect_[2].start });

	//レベルデータ読み込み
	LoadLVData("scene/stageselect");

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

	spCommon_->LoadTexture(SSSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SSSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ 0.0f,0.0f, 0.0f, easeFadeInOut_.start });

	modelStageTutorial_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");

	objStage_ = Object3d::Create();
	objStage_->SetModel(modelStageTutorial_);
	objStage_->SetCamera(camera_);
	objStage_->SetScale({ 7.0f,7.0f,7.0f });

	particle1_ = Particle::LoadFromParticleTexture("particle1.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	//イージングStandby
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(false);
	for (int i = 0; i < 3; i++)easeEyeStageSelect_[i].Standby(false);
	for (int i = 0; i < 3; i++)easeTargetStageSelect_[i].Standby(false);
	easeFadeInOut_.Standby(false);

}

void StageSelectScene::Update()
{
	if (menuCount_ <= SSSMI_StageTutorial_Tutorial)menuCount_ = SSSMI_StageTutorial_Tutorial;
	else if (menuCount_ >= SSSMI_Stage2_TowerStage)menuCount_ = SSSMI_Stage2_TowerStage;

	if (isStageSelect_)UpdateIsStageSelect();
	else if (isDone_)UpdateIsDone();
	else if (isStart_) UpdateIsGameStart();
	else UpdateIsQuitTitle();

	for (Object3d*& player : objPlayers_)
	{
		pm1_->ActiveX(particle1_, player->GetPosition(), { 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f }, { 0.0f,0.001f,0.0f }, 3, { 1.0f, 0.0f });

		player->Update();
	}
	for (Object3d*& ground : objGrounds_)ground->Update();
	for (Object3d*& goal : objGoals_)goal->Update();


	rot_.y += 0.5f;

	objStage_->SetRotation(rot_);

	spriteMenu_->Update();
	spriteTutorial_->Update();
	spriteStage1_->Update();
	spriteStage2_->Update();
	spriteDone_->Update();
	spriteBack_->Update();
	spriteFadeInOut_->Update();

	pm1_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage_->Update();

	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(false);
#endif // DEBUG

	imguiManager_->End();
}

void StageSelectScene::UpdateIsStageSelect()
{
	//イージング
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeStageSelect_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetStageSelect_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();

	//座標セット
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });

	//カラーセット
	spriteFadeInOut_->SetColor({ 0.0f,0.0f, 0.0f, easeFadeInOut_.num_X });//透明度だけ変える

	//カメラもセット
	camera_->SetEye({ easeEyeStageSelect_[0].num_X, easeEyeStageSelect_[1].num_X, easeEyeStageSelect_[2].num_X });
	camera_->SetTarget({ easeTargetStageSelect_[0].num_X, easeTargetStageSelect_[1].num_X, easeTargetStageSelect_[2].num_X });

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
		for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(true);
		for (int i = 0; i < 3; i++)easeEyeDoneMenu_[i].Standby(false);
		for (int i = 0; i < 3; i++)easeTargetDoneMenu_[i].Standby(false);
		isDone_ = true;
		isStageSelect_ = false;
	}
	if (easeMenuPosX_[5].num_X == easeMenuPosX_[5].end)
	{
		if (input_->TriggerKey(DIK_Q))
		{
			for (int i = 0; i < 3; i++)easePlayerQuitMove_[i].Standby(false);
			for (int i = 0; i < 6; i++)easeMenuPosX_[i].Standby(true);
			outStageSelect_ = true;
			isStageSelect_ = false;
		}
	}
}

void StageSelectScene::UpdateIsDone()
{
	//イージング
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeDoneMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetDoneMenu_[i].ease_out_expo();

	//座標セット
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });

	//カメラもセット
	camera_->SetEye({ easeEyeDoneMenu_[0].num_X, easeEyeDoneMenu_[1].num_X, easeEyeDoneMenu_[2].num_X });
	camera_->SetTarget({ easeTargetDoneMenu_[0].num_X, easeTargetDoneMenu_[1].num_X, easeTargetDoneMenu_[2].num_X });

	//イージングが終わったら
	if (camera_->GetEye().x == easeEyeDoneMenu_[0].end)
	{
		for (int i = 0; i < 3; i++)easePlayerStartMove_[i].Standby(false);
		for (int i = 0; i < 3; i++)easeEyeGameStart_[i].Standby(false);
		for (int i = 0; i < 3; i++)easeTargetGameStart_[i].Standby(false);
		isStart_ = true;
		isDone_ = false;
	}
}

void StageSelectScene::UpdateIsGameStart()
{
	//プレイヤーのX値がここまで来たらフェードアウト
	const float gameStartPosX_ = 200.0f;
	//イージング
	for (int i = 0; i < 3; i++)easePlayerStartMove_[i].ease_in_out_expo();
	for (int i = 0; i < 3; i++)easeEyeGameStart_[i].ease_in_quint();
	for (int i = 0; i < 3; i++)easeTargetGameStart_[i].ease_in_quint();

	//カメラセット
	camera_->SetEye({ easeEyeGameStart_[0].num_X, easeEyeGameStart_[1].num_X, easeEyeGameStart_[2].num_X });
	camera_->SetTarget({ easeTargetGameStart_[0].num_X, easeTargetGameStart_[1].num_X, easeTargetGameStart_[2].num_X });

	//プレイヤー座標もセット
	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerStartMove_[0].num_X,easePlayerStartMove_[1].num_X ,easePlayerStartMove_[2].num_X });;
		if (player->GetPosition().x >= gameStartPosX_)FadeOut({ 1.0f,1.0f,1.0f });//ゲームプレイ遷移時は白くする
	}
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//チュートリアルステージ
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);//ステージ1
		else if (menuCount_ == SSSMI_Stage2_TowerStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_AreaBoss);//ステージ2はまだ未完成

	}
}

void StageSelectScene::UpdateIsQuitTitle()
{
	//イージング
	for (int i = 0; i < 3; i++)easePlayerQuitMove_[i].ease_in_expo();
	for (int i = 0; i < 6; i++)easeMenuPosX_[i].ease_out_expo();

	//座標セット
	spriteMenu_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteTutorial_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStage1_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteStage2_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteBack_->SetPosition({ easeMenuPosX_[5].num_X,50.0f });
	//メニュー標記のイージングが終わったらフェードアウト
	if (spriteMenu_->GetPosition().x == easeMenuPosX_[0].start) FadeOut({ 0.0f,0.0f,0.0f });//黒くする

	//プレイヤー座標もセット
	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerQuitMove_[0].num_X,easePlayerQuitMove_[1].num_X ,easePlayerQuitMove_[2].num_X });
	}
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("TITLE", SL_StageTutorial_StageID);
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("TITLE", SL_Stage1_StageID);
		else if (menuCount_ == SSSMI_Stage2_TowerStage)sceneManager_->ChangeScene("TITLE", SL_Stage2_StageID);
	}
}

void StageSelectScene::Draw()
{

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& ground : objGrounds_)ground->Draw();
	for (Object3d*& goal : objGoals_)goal->Draw();
	objStage_->Draw();

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

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
	spriteMenu_->Draw();
	spriteTutorial_->Draw();
	spriteStage1_->Draw();
	spriteStage2_->Draw();
	spriteDone_->Draw();
	spriteBack_->Draw();
	spriteFadeInOut_->Draw();
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
	delete spriteFadeInOut_;

	//ステージ
	delete objStage_;
	//レベルデータ用オブジェクト
	for (Object3d*& player : objPlayers_)delete player;
	for (Object3d*& ground : objGrounds_)delete ground;
	for (Object3d*& goal : objGoals_)delete goal;

	delete modelStageTutorial_;
	delete modelStage1_;
	delete modelStage2_;
	delete modelPlayer_;
	delete modelGround_;
	delete modelGoal_;
	//パーティクル
	delete particle1_;
	delete pm1_;

	//ライト
	delete lightGroup_;
}

void StageSelectScene::FadeOut(DirectX::XMFLOAT3 rgb)
{
	if (!isFadeOut_)
	{
		easeFadeInOut_.Standby(true);
		isFadeOut_ = true;
	}
	else
	{
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//透明度だけ変える

	}
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);

	// モデル読み込み
	modelPlayer_ = Model::LoadFromOBJ("player", true);
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelGround_ = Model::LoadFromOBJ("ground");

	models_.insert(std::make_pair("player", modelPlayer_));
	models_.insert(std::make_pair("sphere", modelGoal_));
	models_.insert(std::make_pair("ground", modelGround_));

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData_->objects) {
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
