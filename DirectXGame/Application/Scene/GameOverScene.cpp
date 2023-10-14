#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "EnumList.h"

using namespace DirectX;

/*

*	GameOverScene.cpp

*	ゲームオーバーシーン

*/

//静的メンバ変数の実体
DirectXCommon* GameOverScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();
Audio* GameOverScene::audio_ = Audio::GetInstance();
SceneManager* GameOverScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameOverScene::imguiManager_ = ImGuiManager::GetInstance();

GameOverScene::GameOverScene(int stagenum) :stageNum_(stagenum) {}

void GameOverScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();

	//カメラ初期化
	camera_ = new Camera();
	//オーディオ
	audio_->Initialize();

	//カメラ
	// 視点座標
	camera_->SetEye({ easeEyeContinue_[0].start, easeEyeContinue_[1].start, easeEyeContinue_[2].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetContinue_[0].start, easeTargetContinue_[1].start, easeTargetContinue_[2].start });


	//レベルデータ読み込み
	if (stageNum_ == SL_Default)LoadLVData("scene/gameovert");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/gameover1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/gameover2");
	else LoadLVData("scene/gameovert");


	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(GOSTI_MenuTex, "texture/gameover2.png");
	spriteGameOver_->Initialize(spCommon_, GOSTI_MenuTex);
	spriteGameOver_->SetPosition({ easeMenuPosX_[0].start,0.0f });

	spCommon_->LoadTexture(GOSTI_ContinueTex, "texture/continue.png");
	spriteContinue_->Initialize(spCommon_, GOSTI_ContinueTex);
	spriteContinue_->SetPosition({ easeMenuPosX_[1].start,150.0f });

	spCommon_->LoadTexture(GOSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, GOSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].start,300.0f });

	spCommon_->LoadTexture(GOSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, GOSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[3].start,450.0f });

	spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, GOSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[4].start,580.0f });

	spCommon_->LoadTexture(GOSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, GOSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ 1.0f,1.0f, 1.0f, easeFadeInOut_.start });

	//パーティクル
	particle1_ = Particle::LoadFromParticleTexture("particle1.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	easeFadeInOut_.Standby(false);
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(false);
}

void GameOverScene::Update()
{
	if (menuCount_ <= GOSMI_Continue)menuCount_ = GOSMI_Continue;
	else if (menuCount_ >= GOSMI_Title)menuCount_ = GOSMI_Title;

	//選択中のメニューカラー
	DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,1.0f };
	//決定指示スプライトのカラー
	DirectX::XMFLOAT4 doneColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	UpdateChangeColor();
	//イージング
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();

	//座標,カラーセット
	spriteGameOver_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteContinue_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteTitle_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteFadeInOut_->SetColor({ 0.2f,0.0f, 0.0f, easeFadeInOut_.num_X });//透明度だけ変える

	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	if (menuCount_ == GOSMI_Continue)
	{
		spriteContinue_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == GOSMI_StageSelect)
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == GOSMI_Title)
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (menuCount_ == GOSMI_Continue)
		{
			//コンティニュー
			sceneManager_->ChangeScene("GAMEPLAY", stageNum_);
		}
		else if (menuCount_ == GOSMI_StageSelect)
		{
			//ステージセレクトへ
			if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
			else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
			else if (stageNum_ <= SL_StageTutorial_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);
		}
		else if (menuCount_ == GOSMI_Title)
		{
			//タイトルへ
			sceneManager_->ChangeScene("TITLE", stageNum_);

		}
	}

	spriteGameOver_->SetColor(doneColor);
	spriteDone_->SetColor(doneColor);

	for (Object3d*& player : objPlayers_)
	{
		pm1_->ActiveY(particle1_, player->GetPosition(), { 0.0f ,2.0f,0.0f },
			{ 0.3f,3.0f,0.3f }, { 0.0f,0.001f,0.0f }, 3, { 1.0f, 0.0f });

		player->Update();
	}
	for (Object3d*& stage : objStages_)
	{
		//天球回転用
		DirectX::XMFLOAT3 rotStage = stage->GetRotation();

		const float rotSpeed = -0.2f;
		rotStage.y += rotSpeed;
		stage->SetRotation(rotStage);

		stage->Update();
	}
	for (Object3d*& goal : objGoals_)goal->Update();



	spriteGameOver_->Update();
	spriteContinue_->Update();
	spriteStageSelect_->Update();
	spriteTitle_->Update();
	spriteDone_->Update();
	spriteFadeInOut_->Update();

	camera_->Update();
	lightGroup_->Update();
	pm1_->Update();

	imguiManager_->Begin();
#ifdef _DEBUG
	camera_->DebugCamera(false);
#endif // DEBUG
	imguiManager_->End();
}

void GameOverScene::Draw()
{
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& stage : objStages_)stage->Draw();
	for (Object3d*& goal : objGoals_)goal->Draw();

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

	//前景スプライト
	spCommon_->PreDraw();
	//スプライト描画	
	spriteGameOver_->Draw();
	spriteContinue_->Draw();
	spriteStageSelect_->Draw();
	spriteTitle_->Draw();
	spriteDone_->Draw();
	spriteFadeInOut_->Draw();
}

void GameOverScene::Finalize()
{
	//音声
	audio_->Finalize();
	//sprite
	delete spriteGameOver_;
	delete spriteContinue_;
	delete spriteStageSelect_;
	delete spriteTitle_;
	delete spriteDone_;
	delete spriteFadeInOut_;
	//モデル
	//レベルデータ用オブジェクト
	for (Object3d*& player : objPlayers_)delete player;
	for (Object3d*& stage : objStages_)delete stage;
	for (Object3d*& goal : objGoals_)delete goal;

	delete modelPlayer_;
	delete modelGoal_;
	delete modelStageTutorial_;
	delete modelStage1_;
	delete modelStage2_;

	//カメラ
	delete camera_;
	//ライト
	delete lightGroup_;
	//パーティクル
	delete particle1_;
	delete pm1_;
}

void GameOverScene::LoadLVData([[maybe_unused]] const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);

	// モデル読み込み
	modelPlayer_ = Model::LoadFromOBJ("player", true);
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelStageTutorial_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");

	models_.insert(std::make_pair("player", modelPlayer_));
	models_.insert(std::make_pair("sphere", modelGoal_));
	models_.insert(std::make_pair("skydomet", modelStageTutorial_));
	models_.insert(std::make_pair("skydome", modelStage1_));
	models_.insert(std::make_pair("skydome2", modelStage2_));

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
			objStages_.push_back(newObject);
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

void GameOverScene::FadeOut(DirectX::XMFLOAT3 rgb)
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

void GameOverScene::UpdateChangeColor()
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
