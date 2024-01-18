#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "TouchableObject.h"
#include "EnumList.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;
using namespace IwasiEngine;
/*

*	StageSelectScene.cpp

*	ステージセレクトシーン

*/


StageSelectScene::StageSelectScene(const int count) :menuCount_(count)
{
}

void StageSelectScene::Initialize()
{
	//インスタンス取得
	dxCommon_ = DirectXCommon::GetInstance();		//DirectX基盤
	spCommon_ = SpriteCommon::GetInstance();		//スプライト基盤
	input_ = Input::GetInstance();					//入力情報
	audio_ = Audio::GetInstance();					//音声
	sceneManager_ = SceneManager::GetInstance();	//シーンマネージャー
	imguiManager_ = ImGuiManager::GetInstance();	//ImGuiマネージャー

	//カメラ初期化
	camera_ = std::make_unique<Camera>();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ easeEyeStageSelect_[XYZ_X].start, easeEyeStageSelect_[XYZ_Y].start, easeEyeStageSelect_[XYZ_Z].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetStageSelect_[XYZ_X].start, easeTargetStageSelect_[XYZ_Y].start, easeTargetStageSelect_[XYZ_Z].start });

	//レベルデータ読み込み
	LoadLVData("scene/stageselect");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);

	//メニュー画面スプライト
	spCommon_->LoadTexture(SSSTI_MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, SSSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[SSMEN_Menu].start,menuPosY_[SSMEN_Menu] });

	//チュートリアルステージスプライト
	spCommon_->LoadTexture(SSSTI_MenuTutorialTex, "texture/stagetraining.png");
	spriteTutorial_->Initialize(spCommon_, SSSTI_MenuTutorialTex);
	spriteTutorial_->SetPosition({ easeMenuPosX_[SSMEN_Tutorial].start,easeStartStagePosY_[SSSMI_StageTutorial_Tutorial].start });

	//ステージ1スプライト
	spCommon_->LoadTexture(SSSTI_Menustage1Tex, "texture/stagesky.png");
	spriteStage1_->Initialize(spCommon_, SSSTI_Menustage1Tex);
	spriteStage1_->SetPosition({ easeMenuPosX_[SSMEN_Stage1_Sky].start,easeStartStagePosY_[SSSMI_Stage1_SkyStage].start });

	//ステージ2スプライト
	spCommon_->LoadTexture(SSSTI_Menustage2Tex, "texture/stageboss.png");
	spriteStage2_->Initialize(spCommon_, SSSTI_Menustage2Tex);
	spriteStage2_->SetPosition({ easeMenuPosX_[SSMEN_Stage2_Space].start,easeStartStagePosY_[SSSMI_Stage2_SpaceStage].start });

	//決定表示スプライト
	spCommon_->LoadTexture(SSSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, SSSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[SSMEN_SelectSpace].start,menuPosY_[SSMEN_SelectSpace] });

	//タイトルに戻るスプライト
	spCommon_->LoadTexture(SSSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, SSSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].start,menuPosY_[SSMEN_Quit] });
	const XMFLOAT4 backColor = { 0.0f,0.0f,0.1f,1.0f };
	spriteBack_->SetColor(backColor);

	//フェードインアウトスプライト
	spCommon_->LoadTexture(SSSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SSSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({
		black_.x,black_.y,black_.z,easeFadeInOut_.start });

	//ロードスプライト
	spCommon_->LoadTexture(SSSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, SSSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.end });//透明化

	//現在ステージスプライト
	spCommon_->LoadTexture(SSSTI_StageInfoNowTex, "texture/stage1.png");
	spriteStageInfoNow_->Initialize(spCommon_, SSSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//カーソルスプライト	
	spCommon_->LoadTexture(SSSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, SSSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[SSMEN_Tutorial] });

	//ステージ名スプライト
	spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/1-1.png");//ここはあくまで置くだけ
	spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化
	
	//メニュー操作スプライト
	spCommon_->LoadTexture(SSSTI_MenuUITex, "texture/menuui.png");
	spriteMenuUI_->Initialize(spCommon_, SSSTI_MenuUITex);
	spriteMenuUI_->SetPosition({ easeMenuPosX_[SSMEN_UI].start,menuPosY_[SSMEN_UI] });

	//背景モデル
	// モデルを読み込んで登録
	ModelMapping(modelStageTutorial_, "skydomet");			//チュートリアルステージモデル(天球)
	ModelMapping(modelStage1_, "skydome");					//ステージ1モデル(天球)
	ModelMapping(modelStage2_, "skydome2");					//ステージ2モデル(天球)
	

	objStage_ = Object3d::Create();
	objStage_->SetModel(modelStageTutorial_.get());
	objStage_->SetCamera(camera_.get());
	const XMFLOAT3 stageScale = { 7.0f,7.0f,7.0f };//ステージ天球の大きさ
	objStage_->SetScale(stageScale);

	//パーティクル
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	particleGoal_ = Particle::LoadFromParticleTexture("particle1.png");

	//ブースト用
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());
	pmFire_->SetCamera(camera_.get());
	//ゴールオブジェクト用
	pmGoal_ = ParticleManager::Create();
	pmGoal_->SetParticleModel(particleGoal_.get());
	pmGoal_->SetCamera(camera_.get());


	//イージングスタンバイ
	for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageSelect_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageSelect_[i].Standby(false);
	easeFadeInOut_.Standby(false);
	easeCursorPosX_.Standby(false);
}

void StageSelectScene::Update()
{
	if (menuCount_ <= SSSMI_StageTutorial_Tutorial)menuCount_ = SSSMI_StageTutorial_Tutorial;
	else if (menuCount_ >= SSSMI_Stage2_SpaceStage)menuCount_ = SSSMI_Stage2_SpaceStage;

	if (isStageSelect_)UpdateIsStageSelect();		//ステージセレクトメニュー時
	else if (isDone_)UpdateIsDone();				//ステージ決定時
	else if (isStart_) UpdateIsGameStart();			//ゲーム開始時
	else UpdateIsQuitTitle();						//タイトル遷移時

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		const XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//オフセット
		//炎プリセット
		const ParticleManager::Preset fire =
		{
			particleFire_.get(),
			{player->GetPosition().x + dashOffsetXY.x,player->GetPosition().y + dashOffsetXY.y,player->GetPosition().z},
			{ 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			2,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{0.0f,0.0f,0.0f,1.0f}
		};
		//パーティクル
		pmFire_->ActiveX(fire);
		//丸影
		SetUpCircleShadow(player->GetPosition());
		//更新
		player->Update();
	}
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Update();	//地面は更新だけ
	for (std::unique_ptr<Object3d>& goal : objGoals_)
	{
		//パーティクルプリセット
		const ParticleManager::Preset goalEffect =
		{
			particleGoal_.get(),
			goal->GetPosition(),
			{ 20.0f,20.0f,20.0f } ,
			{ 0.1f,4.0f,0.1f },
			{ 0.0f,0.001f,0.0f },
			1,
			{3.0f, 0.0f },
			{MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),1.0f},
			{MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),MyMath::RandomMTFloat(0.0f,1.0f),1.0f}
		};
		//ゴールの位置を知らせるパーティクル
		pmGoal_->ActiveY(goalEffect);

		//ゴールは常時回っている
		DirectX::XMFLOAT3 rot = goal->GetRotation();
		const float rotSpeedY = 1.0f;
		rot.y += rotSpeedY;
		goal->SetRotation(rot);
		//更新
		goal->Update();
	}

	//天球も常時回転
	const float rotSpeed = 0.5f;
	rot_.y += rotSpeed;
	objStage_->SetRotation(rot_);

	//スプライト更新
	spriteMenu_->Update();				//メニュー画面スプライト
	spriteTutorial_->Update();			//チュートリアルステージスプライト
	spriteStage1_->Update();			//ステージ1スプライト
	spriteStage2_->Update();			//ステージ2スプライト
	spriteDone_->Update();				//決定表示スプライト
	spriteBack_->Update();				//タイトルに戻るスプライト
	spriteFadeInOut_->Update();			//フェードインアウトスプライト
	spriteLoad_->Update();				//ロードスプライト
	spriteStageInfoNow_->Update();		//現在ステージスプライト
	spriteCursor_->Update();			//カーソルスプライト
	spriteStageName_->Update();			//ステージ名スプライト
	spriteMenuUI_->Update();			//メニュー操作方法スプライト

	//更新
	camera_->Update();					//カメラ
	lightGroup_->Update();				//ライト
	objStage_->Update();				//背景オブジェクト
	pmFire_->Update();					//パーティクルマネージャー(ジェット)
	pmGoal_->Update();					//パーティクルマネージャー(ゴールオブジェクト)

	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(false);
#endif // DEBUG

	imguiManager_->End();
}

void StageSelectScene::UpdateIsStageSelect()
{
	//イージング
	for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageSelect_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageSelect_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();

	//座標セット
	spriteMenu_->SetPosition({ easeMenuPosX_[SSMEN_Menu].num_X,menuPosY_[SSMEN_Menu] });
	spriteTutorial_->SetPosition({ easeMenuPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
	spriteStage1_->SetPosition({ easeMenuPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
	spriteStage2_->SetPosition({ easeMenuPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });
	spriteDone_->SetPosition({ easeMenuPosX_[SSMEN_SelectSpace].num_X,menuPosY_[SSMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuPosX_[SSMEN_UI].num_X,menuPosY_[SSMEN_UI] });
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });

	//カラーセット
	spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });	//透明度だけ変える

	//カメラもセット
	camera_->SetEye({ easeEyeStageSelect_[XYZ_X].num_X, easeEyeStageSelect_[XYZ_Y].num_X, easeEyeStageSelect_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageSelect_[XYZ_X].num_X, easeTargetStageSelect_[XYZ_Y].num_X, easeTargetStageSelect_[XYZ_Z].num_X });

	//メニュー選択
	if (input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_S))menuCount_++;

	//選択中のメニューカラー
	const DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,0.7f };
	//選択中のメニューカラー(暗い背景時)
	const DirectX::XMFLOAT4 selectMenuColorDark = { 1.0f,selectColor_.y + 0.1f,selectColor_.z + 0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColorDark = { 1.0f,1.0f,1.0f,0.7f };
	//決定指示スプライトのカラー
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };
	//タイトルへ戻る指示スプライトのカラー
	const DirectX::XMFLOAT4 quitColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	const DirectX::XMFLOAT4 quitColorDark = { selectColor_.x + 0.1f,selectColor_.y + 0.1f,1.0f,1.0f };
	//色替え
	UpdateChangeColor();

	//色を変える(選択しているメニューは強調)
	if (menuCount_ == SSSMI_StageTutorial_Tutorial)//チュートリアルを選択時
	{
		objStage_->SetModel(modelStageTutorial_.get());
		spriteMenu_->SetColor(selectMenuColor);
		spriteTutorial_->SetColor(selectMenuColor);
		spriteStage1_->SetColor(otherMenuColor);
		spriteStage2_->SetColor(otherMenuColor);
		spriteBack_->SetColor(quitColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Tutorial] });
	}
	else if (menuCount_ == SSSMI_Stage1_SkyStage)//空ステージを選択時
	{
		objStage_->SetModel(modelStage1_.get());
		spriteMenu_->SetColor(selectMenuColor);
		spriteTutorial_->SetColor(otherMenuColor);
		spriteStage1_->SetColor(selectMenuColor);
		spriteStage2_->SetColor(otherMenuColor);
		spriteBack_->SetColor(quitColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Stage1_Sky] });
	}
	else if (menuCount_ == SSSMI_Stage2_SpaceStage)//宇宙ステージを選択時
	{
		objStage_->SetModel(modelStage2_.get());
		spriteMenu_->SetColor(selectMenuColorDark);
		spriteTutorial_->SetColor(otherMenuColorDark);
		spriteStage1_->SetColor(otherMenuColorDark);
		spriteStage2_->SetColor(selectMenuColorDark);
		spriteBack_->SetColor(quitColorDark);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SSMEN_Stage2_Space] });
	}
	//共通カラー
	spriteDone_->SetColor(doneColor);
	if (spriteDone_->GetPosition().x == easeMenuPosX_[SSMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			//イージングスタンバイ
			for (int i = 0; i < SSMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeEyeDoneMenu_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetDoneMenu_[i].Standby(false);

			for (int i = 0; i < XYZ_Num; i++)easeStartStagePosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeStartStagePosY_[i].Standby(false);
			easeCursorPosX_.Standby(true);

			//ここでステージ名のスプライトをロードし、座標、アンカーポイントをセットする
			if (menuCount_ == SSSMI_StageTutorial_Tutorial)//チュートリアルを選択時
			{
				spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/1-1.png");
				spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
				spriteStageName_->SetPosition(stageNamePos_);
				spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
				spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化
			}
			else if (menuCount_ == SSSMI_Stage1_SkyStage)//空ステージを選択時
			{
				spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/2-1.png");
				spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
				spriteStageName_->SetPosition(stageNamePos_);
				spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
				spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

			}
			else if (menuCount_ == SSSMI_Stage2_SpaceStage)//宇宙ステージを選択時
			{
				//現在ステージスプライト
				spCommon_->LoadTexture(SSSTI_StageInfoNowTex, "texture/stagef.png");
				spriteStageInfoNow_->Initialize(spCommon_, SSSTI_StageInfoNowTex);
				spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
				spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化
				
				spCommon_->LoadTexture(SSSTI_StageNameTex, "texture/stagename/2-4.png");
				spriteStageName_->Initialize(spCommon_, SSSTI_StageNameTex);
				spriteStageName_->SetPosition(stageNamePos_);
				spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
				spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化
			}

			isDone_ = true;
			isStageSelect_ = false;
		}
		//メニューのイージングが終わったら操作を受け付ける
		if (easeMenuPosX_[SSMEN_Quit].num_X == easeMenuPosX_[SSMEN_Quit].end)
		{
			if (input_->TriggerKey(DIK_ESCAPE))//タイトルへ戻る
			{
				for (int i = 0; i < XYZ_Num; i++)easePlayerQuitMove_[i].Standby(false);
				for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].Standby(true);
				easeCursorPosX_.Standby(true);

				outStageSelect_ = true;
				isStageSelect_ = false;
			}
		}
	}
}

void StageSelectScene::UpdateIsDone()
{
	//イージング
	for (int i = 0; i < SSMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeDoneMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetDoneMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeStartStagePosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeStartStagePosY_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//座標セット
	spriteMenu_->SetPosition({ easeMenuEndPosX_[SSMEN_Menu].num_X,menuPosY_[SSMEN_Menu] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SSMEN_SelectSpace].num_X,menuPosY_[SSMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[SSMEN_UI].num_X,menuPosY_[SSMEN_UI] });
	spriteBack_->SetPosition({ easeMenuEndPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//ステージごとに座標が違う
	if (menuCount_ == SSSMI_StageTutorial_Tutorial)
	{
		spriteTutorial_->SetPosition({ easeStartStagePosX_[SSSMI_StageTutorial_Tutorial].num_X,easeStartStagePosY_[SSSMI_StageTutorial_Tutorial].num_X });
		spriteStage1_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
		spriteStage2_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });
	}
	else if (menuCount_ == SSSMI_Stage1_SkyStage)
	{
		spriteTutorial_->SetPosition({ easeMenuEndPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
		spriteStage1_->SetPosition({ easeStartStagePosX_[SSSMI_Stage1_SkyStage].num_X,easeStartStagePosY_[SSSMI_Stage1_SkyStage].num_X });
		spriteStage2_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });

	}
	else if (menuCount_ == SSSMI_Stage2_SpaceStage)
	{
		spriteTutorial_->SetPosition({ easeMenuEndPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
		spriteStage1_->SetPosition({ easeMenuEndPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
		spriteStage2_->SetPosition({ easeStartStagePosX_[SSSMI_Stage2_SpaceStage].num_X,easeStartStagePosY_[SSSMI_Stage2_SpaceStage].num_X });
	}

	//カメラもセット
	camera_->SetEye({ easeEyeDoneMenu_[XYZ_X].num_X, easeEyeDoneMenu_[XYZ_Y].num_X, easeEyeDoneMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetDoneMenu_[XYZ_X].num_X, easeTargetDoneMenu_[XYZ_Y].num_X, easeTargetDoneMenu_[XYZ_Z].num_X });

	//イージングが終わったら
	if (camera_->GetEye().x == easeEyeDoneMenu_[XYZ_X].end)
	{
		for (int i = 0; i < XYZ_Num; i++)easePlayerStartMove_[i].Standby(false);
		for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
		for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
		isStart_ = true;
		isDone_ = false;
	}
	//スペースを押すとスキップ
	SkipDirectionOnSpace(white_);

	//完全に白くなったら
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//チュートリアルステージ
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);//ステージ1
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_AreaBoss);//ステージ2はまだ未完成

	}
}

void StageSelectScene::UpdateIsGameStart()
{
	//プレイヤーのX値がここまで来たらフェードアウト
	const float gameStartPosX_ = 200.0f;
	//イージング
	for (int i = 0; i < XYZ_Num; i++)easePlayerStartMove_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_quint();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_quint();

	//カメラセット
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });

	//プレイヤー座標もセット
	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		player->SetPosition({ easePlayerStartMove_[XYZ_X].num_X,easePlayerStartMove_[XYZ_Y].num_X ,easePlayerStartMove_[XYZ_Z].num_X });
		if (player->GetPosition().x >= gameStartPosX_)FadeIn(white_);//ゲームプレイ遷移時は白くする
		else SkipDirectionOnSpace(white_);//スペースを押すとスキップ
	}

	//完全に白くなったら
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//チュートリアルステージ
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_Area1);//ステージ1
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("GAMEPLAY", SL_Stage1_AreaBoss);//ステージ2はまだ未完成

	}
}

void StageSelectScene::UpdateIsQuitTitle()
{
	//イージング
	for (int i = 0; i < XYZ_Num; i++)easePlayerQuitMove_[i].ease_in_expo();
	for (int i = 0; i < SSMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//座標セット
	spriteMenu_->SetPosition({ easeMenuPosX_[SSMEN_Menu].num_X,menuPosY_[SSMEN_Menu] });
	spriteTutorial_->SetPosition({ easeMenuPosX_[SSMEN_Tutorial].num_X,menuPosY_[SSMEN_Tutorial] });
	spriteStage1_->SetPosition({ easeMenuPosX_[SSMEN_Stage1_Sky].num_X,menuPosY_[SSMEN_Stage1_Sky] });
	spriteStage2_->SetPosition({ easeMenuPosX_[SSMEN_Stage2_Space].num_X,menuPosY_[SSMEN_Stage2_Space] });
	spriteDone_->SetPosition({ easeMenuPosX_[SSMEN_SelectSpace].num_X,menuPosY_[SSMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuPosX_[SSMEN_UI].num_X,menuPosY_[SSMEN_UI] });
	spriteBack_->SetPosition({ easeMenuPosX_[SSMEN_Quit].num_X,menuPosY_[SSMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//メニュー標記のイージングが終わったらフェードアウト
	if (spriteMenu_->GetPosition().x == easeMenuPosX_[SSMEN_Menu].start) FadeIn(black_);//黒くする
	else SkipDirectionOnSpace(black_);//スペースを押すとスキップ

	//プレイヤー座標もセット
	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		player->SetPosition({ easePlayerQuitMove_[XYZ_X].num_X,easePlayerQuitMove_[XYZ_Y].num_X ,easePlayerQuitMove_[XYZ_Z].num_X });
	}
	//完全に黒くなったら
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//タイトルへ遷移(背景によって遷移時のタイトルシーンも変わる)
		if (menuCount_ == SSSMI_StageTutorial_Tutorial)sceneManager_->ChangeScene("TITLE", SL_StageTutorial_StageID);
		else if (menuCount_ == SSSMI_Stage1_SkyStage)sceneManager_->ChangeScene("TITLE", SL_Stage1_StageID);
		else if (menuCount_ == SSSMI_Stage2_SpaceStage)sceneManager_->ChangeScene("TITLE", SL_Stage2_StageID);
	}
	
}

void StageSelectScene::Draw()
{

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();	//プレイヤー
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Draw();	//地面
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();			//ゴール
	objStage_->Draw();														//天球(背景)

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	pmFire_->Draw();
	pmGoal_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();


	spCommon_->PreDraw();
	//前景スプライト
	//スプライト描画	
	spriteMenu_->Draw();					//メニュー画面スプライト
	spriteTutorial_->Draw();				//チュートリアルステージスプライト
	spriteStage1_->Draw();					//ステージ1スプライト
	spriteStage2_->Draw();					//ステージ2スプライト
	spriteDone_->Draw();					//決定表示スプライト
	spriteBack_->Draw();					//タイトルに戻るスプライト
	spriteFadeInOut_->Draw();				//フェードインアウトスプライト
	spriteLoad_->Draw();					//ロードスプライト
	spriteStageInfoNow_->Draw();			//現在ステージスプライト
	spriteCursor_->Draw();					//カーソルスプライト
	spriteStageName_->Draw();				//ステージ名スプライト
	spriteMenuUI_->Draw();					//メニュー操作方法スプライト

}

void StageSelectScene::Finalize()
{
	//音声
	audio_->Finalize();
}

void StageSelectScene::FadeIn(const DirectX::XMFLOAT3& color)
{
	if (!isFadeIn_)
	{
		//ここでスタンバイ
		easeFadeInOut_.Standby(true);
		isFadeIn_ = true;
	}
	else
	{
		//ここでフェードインアウト
		const DirectX::XMFLOAT3 negapozi = { 1.0f - color.x,1.0f - color.y,1.0f - color.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOut_.num_X });//透明度だけ変える
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ネガポジの応用
		if (isStart_ || isDone_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
		}
	}
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);
	
	// モデルを読み込んで登録
	ModelMapping(modelPlayer_, "playerdash", true);		//自機モデル
	ModelMapping(modelGround_, "ground");				//床モデル
	ModelMapping(modelGoal_, "sphere");					//ゴールモデル

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}

		if (objectData.objectType.find("PLAYER") == LDTOF_TRUE)
		{
			// モデルを指定して3Dオブジェクトを生成
			std::unique_ptr<Object3d> newObject = Object3d::Create();
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//カメラセット
			newObject->SetCamera(camera_.get());
			// 配列に登録
			objPlayers_.push_back(std::move(newObject));
		}
		else if (objectData.objectType.find("PLANE") == LDTOF_TRUE)
		{
			// モデルを指定して3Dオブジェクトを生成
			std::unique_ptr<Object3d> newObject = Object3d::Create();
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//カメラセット
			newObject->SetCamera(camera_.get());
			// 配列に登録
			objGrounds_.push_back(std::move(newObject));
		}
		else if (objectData.objectType.find("GOAL") == LDTOF_TRUE)
		{
			// モデルを指定して3Dオブジェクトを生成
			std::unique_ptr<Object3d> newObject = Object3d::Create();
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//カメラセット
			newObject->SetCamera(camera_.get());
			// 配列に登録
			objGoals_.push_back(std::move(newObject));
		}

	}

}

void StageSelectScene::UpdateChangeColor()
{
	//色を変えるスピード
	const float speedColor = 0.02f;

	if (isColorReverse_)
	{
		//反転させる
		selectColor_.x -= speedColor;
		selectColor_.y -= speedColor;
		selectColor_.z -= speedColor;
	}

	else
	{
		//色は自動的に増減
		selectColor_.x += speedColor;
		selectColor_.y += speedColor;
		selectColor_.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.7f,0.0f };//{max,min}

	if (selectColor_.x >= maxAndMinSpeedColor.x)//Max値に到達したら反転
	{
		isColorReverse_ = true;
	}
	if (selectColor_.x <= maxAndMinSpeedColor.y)//Min値に到達したら反転を戻す
	{
		isColorReverse_ = false;
	}
}

void StageSelectScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
{
	const DirectX::XMVECTOR dir = { 0.0f,1.0f,0.0f,0.0f };			//投影方向
	const DirectX::XMFLOAT3 casterPosOffset = { -0.5f,0.0f,0.0f };	//キャスター座標のオフセット
	//キャスター座標
	const DirectX::XMFLOAT3 casterPos =
	{
		pos.x + casterPosOffset.x,
		pos.y + casterPosOffset.y,
		pos.z + casterPosOffset.z
	};
	const DirectX::XMFLOAT3 atten = { 0.5f,0.6f,0.0f };				//距離減衰係数
	const DirectX::XMFLOAT2 factorAngle = { 0.2f,0.5f };			//角度減衰係数

	//シャドウのセット
	lightGroup_->SetCircleShadowDir(LightGroup::LN_0, dir);
	lightGroup_->SetCircleShadowCasterPos(LightGroup::LN_0, casterPos);
	lightGroup_->SetCircleShadowAtten(LightGroup::LN_0, atten);
	lightGroup_->SetCircleShadowFactorAngleCos(LightGroup::LN_0, factorAngle);
}
