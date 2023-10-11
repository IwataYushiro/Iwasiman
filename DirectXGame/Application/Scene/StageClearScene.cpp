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


StageClearScene::StageClearScene(int stagenum) :stageNum_(stagenum) {}

void StageClearScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ easeEyeGameStart_[0].start, easeEyeGameStart_[1].start, easeEyeGameStart_[2].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetGameStart_[0].start, easeTargetGameStart_[1].start, easeTargetGameStart_[2].start });

	//レベルデータ読み込み
	if (stageNum_ == SL_Default)LoadLVData("scene/stagecleart");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/stageclear1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/stageclear2");
	else LoadLVData("scene/stagecleart");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	//スプライト
	if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_Stage2_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else spCommon_->LoadTexture(SCSTI_MenuTex, "texture/stageclear2.png");

	spriteStageClear_->Initialize(spCommon_, SCSTI_MenuTex);
	spriteStageClear_->SetPosition({ easeMenuPosX_[0].start,0.0f });

	spCommon_->LoadTexture(SCSTI_ContinueTex, "texture/nextstage.png");
	spriteNextStage_->Initialize(spCommon_, SCSTI_ContinueTex);
	spriteNextStage_->SetPosition({ easeMenuPosX_[1].start,150.0f });

	spCommon_->LoadTexture(SCSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, SCSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].start,300.0f });

	spCommon_->LoadTexture(SCSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, SCSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[3].start,450.0f });

	spCommon_->LoadTexture(SCSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, SCSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[4].start,580.0f });

	spCommon_->LoadTexture(SCSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, TSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ 1.0f,1.0f, 1.0f, easeFadeInOut_.start });

	//パーティクル
	particle1_ = Particle::LoadFromParticleTexture("particle1.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	if (stageNum_ == SL_Stage1_AreaBoss)
	{
		isFinalStage_ = true;
		menuCount_ = SCSMI_StageSelect;
	}
	else if (stageNum_ == SL_Stage2_AreaBoss)
	{
		isFinalStage_ = true;
		menuCount_ = SCSMI_StageSelect;
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		isFinalStage_ = true;
		menuCount_ = SCSMI_StageSelect;
	}

	easeFadeInOut_.Standby(false);
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(false);
}

void StageClearScene::Update()
{
	if (isNextStage_)UpdateIsNextStage();
	else if (isStageSelect_)UpdateIsStageSelect();
	else if (isQuitTitle_)UpdateIsQuitTitle();
	else UpdateIsMenu();

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

	

	spriteStageClear_->Update();
	spriteNextStage_->Update();
	spriteStageSelect_->Update();
	spriteTitle_->Update();
	spriteDone_->Update();
	spriteFadeInOut_->Update();


	camera_->Update();
	lightGroup_->Update();
	pm1_->Update();

	imguiManager_->Begin();
	imguiManager_->End();

}

void StageClearScene::UpdateIsNextStage()
{
	//イージング
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeGameStart_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetGameStart_[i].ease_out_expo();

	//座標セット
	spriteStageClear_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteNextStage_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteTitle_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });

	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[0].num_X, easeEyeGameStart_[1].num_X, easeEyeGameStart_[2].num_X });
	camera_->SetTarget({ easeTargetGameStart_[0].num_X, easeTargetGameStart_[1].num_X, easeTargetGameStart_[2].num_X });

	for (Object3d*& goal : objGoals_)
	{

		DirectX::XMFLOAT3 move = goal->GetPosition();
		DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };
		//X値がここまで来たらイージング
		const float gameStartPos_ = 50.0f;

		move.x += speed.x;
		goal->SetPosition(move);
		if (goal->GetPosition().x <= gameStartPos_)
		{
			FadeOut({ 1.0f,1.0f,1.0f });//ゲームプレイ遷移時は白くする
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				if (stageNum_ == SL_Stage1_AreaBoss) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
				else if (stageNum_ == SL_StageTutorial_Final) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else sceneManager_->ChangeScene("GAMEPLAY", ++stageNum_);
			}
		};


	}
}

void StageClearScene::UpdateIsStageSelect()
{
	//イージング
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeGameStart_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetGameStart_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easePlayerMoveStageSelect_[i].ease_in_out_expo();
	//座標セット
	spriteStageClear_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteNextStage_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteTitle_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });

	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[0].num_X, easeEyeGameStart_[1].num_X, easeEyeGameStart_[2].num_X });
	camera_->SetTarget({ easeTargetGameStart_[0].num_X, easeTargetGameStart_[1].num_X, easeTargetGameStart_[2].num_X });

	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerMoveStageSelect_[0].num_X,easePlayerMoveStageSelect_[1].num_X,easePlayerMoveStageSelect_[2].num_X });

		if (spriteStageClear_->GetPosition().x == easeMenuPosX_[0].start)
		{
			FadeOut({ 0.0f,0.0f,0.0f });//黒くする
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//ステージ選択
				if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
				else sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);//ここはチュートリアルに飛ばしてもいい
			}
		}
	}
}

void StageClearScene::UpdateIsQuitTitle()
{
	//イージング
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	//座標セット
	spriteStageClear_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteNextStage_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteTitle_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });

	if(spriteStageClear_->GetPosition().x == easeMenuPosX_[0].start)FadeOut({ 0.0f,0.0f,0.0f });//黒くする
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//タイトルへ
		sceneManager_->ChangeScene("TITLE", stageNum_);
	}
}

void StageClearScene::UpdateIsMenu()
{
	//メニューの上限をいたステージによって変更
	if (!isFinalStage_)
	{
		if (menuCount_ <= SCSMI_Continue)menuCount_ = SCSMI_Continue;
		else if (menuCount_ >= SCSMI_Title)menuCount_ = SCSMI_Title;
	}
	else
	{
		if (menuCount_ <= SCSMI_StageSelect)menuCount_ = SCSMI_StageSelect;
		else if (menuCount_ >= SCSMI_Title)menuCount_ = SCSMI_Title;
	}

	//選択中のメニューカラー
	DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,1.0f };
	//決定指示スプライトのカラー
	DirectX::XMFLOAT4 doneColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	UpdateChangeColor();

	//イージング
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();

	//座標セット
	spriteStageClear_->SetPosition({ easeMenuPosX_[0].num_X,0.0f });
	spriteNextStage_->SetPosition({ easeMenuPosX_[1].num_X,150.0f });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[2].num_X,300.0f });
	spriteTitle_->SetPosition({ easeMenuPosX_[3].num_X,450.0f });
	spriteDone_->SetPosition({ easeMenuPosX_[4].num_X,550.0f });
	spriteFadeInOut_->SetColor({ 1.0f,1.0f, 1.0f, easeFadeInOut_.num_X });//透明度だけ変える

	//メニュー操作
	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;


	if (menuCount_ == SCSMI_Continue)
	{
		spriteNextStage_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == SCSMI_StageSelect)
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
	}
	else if (menuCount_ == SCSMI_Title)
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
	}

	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{

			if (menuCount_ == SCSMI_Continue)
			{
				for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < 3; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < 3; i++)easeTargetGameStart_[i].Standby(false);
				isNextStage_ = true;
			}
			else if (menuCount_ == SCSMI_StageSelect)
			{
				for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < 3; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < 3; i++)easeTargetGameStart_[i].Standby(false);
				for (int i = 0; i < 3; i++)easePlayerMoveStageSelect_[i].Standby(false);
				isStageSelect_ = true;
			}
			else if (menuCount_ == SCSMI_Title)
			{
				for (int i = 0; i < 5; i++)easeMenuPosX_[i].Standby(true);
				isQuitTitle_ = true;
			}

			
		}
	}

	spriteStageClear_->SetColor(doneColor);
	spriteDone_->SetColor(doneColor);
}

void StageClearScene::Draw()
{
	//背景スプライト描画前処理
	spCommon_->PreDraw();

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (Object3d*& player : objPlayers_)player->Draw();
	for (Object3d*& ground : objGrounds_)ground->Draw();
	for (Object3d*& skydome : objSkydomes_)skydome->Draw();
	if (!isStageSelect_)for (Object3d*& goal : objGoals_)goal->Draw();

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
	spriteStageClear_->Draw();
	if (!isFinalStage_)spriteNextStage_->Draw();
	spriteStageSelect_->Draw();
	spriteTitle_->Draw();
	spriteDone_->Draw();
	spriteFadeInOut_->Draw();

}

void StageClearScene::FadeOut(DirectX::XMFLOAT3 rgb)
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

void StageClearScene::Finalize()
{
	//音声
	audio_->Finalize();
	//スプライト
	delete spriteStageClear_;
	delete spriteNextStage_;
	delete spriteStageSelect_;
	delete spriteTitle_;
	delete spriteDone_;
	delete spriteFadeInOut_;

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


}

void StageClearScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);

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

void StageClearScene::UpdateChangeColor()
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
