#include "GameOverScene.h"
#include "FbxLoader.h"
#include "TouchableObject.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;
using namespace IwasiEngine;

/*

*	GameOverScene.cpp

*	ゲームオーバーシーン

*/
GameOverScene::GameOverScene(const int stagenum) :stageNum_(stagenum) {}

void GameOverScene::Initialize()
{
	//インスタンス取得
	dxCommon_ = DirectXCommon::GetInstance();		//DirectX基盤
	spCommon_ = SpriteCommon::GetInstance();		//スプライト基盤
	input_ = Input::GetInstance();					//入力情報
	audio_ = Audio::GetInstance();					//音声
	sceneManager_ = SceneManager::GetInstance();	//シーンマネージャー
	imguiManager_ = ImGuiManager::GetInstance();	//ImGuiマネージャー

	//イージングのロード
	for (int i = 0; i < GOMEN_Num; i++)LoadEasingData("gameover/menuposx.csv", easeMenuPosX_[i], i);
	for (int i = 0; i < GOMEN_Num; i++)LoadEasingData("gameover/menuendposx.csv", easeMenuEndPosX_[i], i);
	LoadEasingData("gameover/cursorposx.csv", easeCursorPosX_);
	LoadEasingData("gameover/continueposx.csv", easeContinuePosX_);
	LoadEasingData("gameover/continueposy.csv", easeContinuePosY_);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/eyecontinue.csv", easeEyeContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/targetcontinue.csv", easeTargetContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playerrotatecontinue.csv", easePlayerRotateContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playermovecontinue.csv", easePlayerMoveContinue_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/eyequitstageselect.csv", easeEyeQuitStageSelect_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/targetquitstageselect.csv", easeTargetQuitStageSelect_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playerrotatequitstageselect.csv", easePlayerRotateQuitStageSelect_[i], i);
	for (int i = 0; i < XYZ_Num; i++)LoadEasingData("gameover/playermovequitstageselect.csv", easePlayerMoveQuitStageSelect_[i], i);
	LoadEasingData("gameover/fadeinout.csv", easeFadeInOut_);


	//カメラ初期化
	camera_ = std::make_unique<Camera>();
	//オーディオ
	audio_->Initialize();

	//カメラ
	// 視点座標
	camera_->SetEye({ easeEyeContinue_[XYZ_X].start, easeEyeContinue_[XYZ_Y].start, easeEyeContinue_[XYZ_Z].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetContinue_[XYZ_X].start, easeTargetContinue_[XYZ_Y].start, easeTargetContinue_[XYZ_Z].start });


	//レベルデータ読み込み
	if (stageNum_ == SL_Default)LoadLVData("scene/gameovert");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/gameover1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/gameover2");
	else LoadLVData("scene/gameovert");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());

	//ゲームオーバースプライト
	spCommon_->LoadTexture(GOSTI_MenuTex, "texture/gameover2.png");
	spriteGameOver_->Initialize(spCommon_, GOSTI_MenuTex);
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].start,menuPosY_[GOMEN_Menu] });

	//コンティニュー表示スプライト
	spCommon_->LoadTexture(GOSTI_ContinueTex, "texture/continue.png");
	spriteContinue_->Initialize(spCommon_, GOSTI_ContinueTex);
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].start,menuPosY_[GOMEN_Continue] });

	//ステージセレクト表示スプライト
	spCommon_->LoadTexture(GOSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, GOSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].start,menuPosY_[GOMEN_StageSelect] });

	//タイトル表示スプライト
	spCommon_->LoadTexture(GOSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, GOSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].start,menuPosY_[GOMEN_Title] });

	//決定スプライト
	spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, GOSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].start,menuPosY_[GOMEN_SelectSpace] });

	//フェードインアウトスプライト
	spCommon_->LoadTexture(GOSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, GOSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });

	//ロードスプライト
	spCommon_->LoadTexture(GOSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, GOSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//現在ステージスプライト
	const int32_t remainderNum = stageNum_ % 10;//余りによってスプライトを変える
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage1.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage4) spCommon_->LoadTexture(GOSTI_StageInfoNowTex, "texture/stagef.png");
	spriteStageInfoNow_->Initialize(spCommon_, GOSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//カーソルスプライト
	spCommon_->LoadTexture(GOSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, GOSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[GOMEN_Continue] });

	//メニュー操作スプライト
	spCommon_->LoadTexture(GOSTI_MenuUITex, "texture/menuui.png");
	spriteMenuUI_->Initialize(spCommon_, GOSTI_MenuUITex);
	spriteMenuUI_->SetPosition({ easeMenuPosX_[GOMEN_UI].start,menuPosY_[GOMEN_UI] });

	//ステージ名スプライト
	LoadStageNameSprite();

	//パーティクル
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");		//炎
	particleFall_ = Particle::LoadFromParticleTexture("wind.png");			//落下演出のパーティクル
	particleSmoke_ = Particle::LoadFromParticleTexture("particle9.png");	//煙
	particleGoal_ = Particle::LoadFromParticleTexture("particle1.png");		//ゴール用
	//ジェット用
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFall_.get());
	pmFire_->SetCamera(camera_.get());
	//煙用
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetBlendMode(ParticleManager::BP_ALPHA);
	pmSmoke_->SetParticleModel(particleSmoke_.get());
	pmSmoke_->SetCamera(camera_.get());
	//ゴールオブジェクト用
	pmGoal_ = ParticleManager::Create();
	pmGoal_->SetParticleModel(particleGoal_.get());
	pmGoal_->SetCamera(camera_.get());

	//イージングスタンバイ
	easeFadeInOut_.Standby(false);
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].Standby(false);
	easeCursorPosX_.Standby(false);

}

void GameOverScene::Update()
{
	//メニュー上限
	if (menuCount_ <= GOSMI_Continue)menuCount_ = GOSMI_Continue;
	else if (menuCount_ >= GOSMI_Title)menuCount_ = GOSMI_Title;


	if (isContinue_)UpdateIsContinue();						//コンティニュー
	else if (isQuitStageSelect_)UpdateIsQuitStageSelect();	//ステージセレクト遷移
	else if (isQuitTitle_)UpdateIsQuitTitle();				//タイトル遷移
	else UpdateIsGameOver();								//メニュー画面

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
			{ 0.0f,0.0f,0.0f,1.0f }
		};

		if (completeRotate_)//イージングによる回転が終わったら
		{
			//ダッシュエフェクトに切り替え
			pmFire_->ActiveX(fire);
		}
		//更新
		player->Update();
	}
	//天球(背景)
	for (std::unique_ptr<Object3d>& stage : objStages_)
	{
		//天球回転用
		DirectX::XMFLOAT3 rotStage = stage->GetRotation();
		//天球(背景)は常時回転する
		const float rotSpeed = -0.2f;
		rotStage.y += rotSpeed;
		stage->SetRotation(rotStage);
		//更新
		stage->Update();
	}
	//ゴール
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Update();//更新

	//スプライト更新
	spriteGameOver_->Update();					 //ゲームオーバースプライト
	spriteContinue_->Update();					 //コンティニュー表示スプライト
	spriteStageSelect_->Update();				 //ステージセレクト表示スプライト
	spriteTitle_->Update();						 //タイトル表示スプライト
	spriteDone_->Update();						 //決定スプライト
	spriteFadeInOut_->Update();					 //フェードインアウトスプライト
	spriteLoad_->Update();						 //ロードスプライト
	spriteStageInfoNow_->Update();				 //現在ステージスプライト
	spriteCursor_->Update();					 //カーソルスプライト
	spriteStageName_->Update();					 //ステージ名スプライト
	spriteMenuUI_->Update();					 //メニュー操作方法スプライト

	//更新
	camera_->Update();			//カメラ
	lightGroup_->Update();		//ライト
	//パーティクルマネージャー更新
	pmFire_->Update();			//炎			
	pmSmoke_->Update();			//煙
	pmGoal_->Update();
	//ImGui
	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(false);//デバッグカメラ
#endif // DEBUG
	imguiManager_->End();
}

void GameOverScene::UpdateIsGameOver()
{
	//ゲームオーバーカラー
	DirectX::XMFLOAT4 gameOverColor;
	const DirectX::XMFLOAT4 isLightBackGroundGameOverColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };//明るい背景
	const DirectX::XMFLOAT4 isDarkBackGroundGameOverColor = { selectColor_.x + 0.1f,selectColor_.y + 0.1f,1.0f,1.0f };//暗い背景

	//ステージ位置(背景)によって色を変える
	if (stageNum_ == SL_Default)gameOverColor = isLightBackGroundGameOverColor;
	else if (stageNum_ <= SL_Stage1_StageID)gameOverColor = isLightBackGroundGameOverColor;
	else if (stageNum_ <= SL_Stage2_StageID)gameOverColor = isDarkBackGroundGameOverColor;
	else gameOverColor = isLightBackGroundGameOverColor;

	//選択してる色
	DirectX::XMFLOAT4 selectMenuColor;
	const DirectX::XMFLOAT4 isLightBackGroundSelectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 isDarkBackGroundSelectMenuColor = { 1.0f,selectColor_.y + 0.1f,selectColor_.z + 0.1f,1.0f };

	if (stageNum_ == SL_Default)selectMenuColor = isLightBackGroundSelectMenuColor;
	else if (stageNum_ <= SL_Stage1_StageID)selectMenuColor = isLightBackGroundSelectMenuColor;
	else if (stageNum_ <= SL_Stage2_StageID)selectMenuColor = isDarkBackGroundSelectMenuColor;
	else selectMenuColor = isLightBackGroundSelectMenuColor;

	//選択されていない色
	DirectX::XMFLOAT4 otherMenuColor;
	const DirectX::XMFLOAT4 isLightBackGroundOtherMenuColor = { 0.0f,0.0f,0.0f,0.7f };//明るい背景
	const DirectX::XMFLOAT4 isDarkBackGroundOtherMenuColor = { 1.0f,1.0f,1.0f,0.7f };//暗い背景

	//ステージ位置(背景)によって色を変える
	if (stageNum_ == SL_Default)otherMenuColor = isLightBackGroundOtherMenuColor;
	else if (stageNum_ <= SL_Stage1_StageID)otherMenuColor = isLightBackGroundOtherMenuColor;
	else if (stageNum_ <= SL_Stage2_StageID)otherMenuColor = isDarkBackGroundOtherMenuColor;
	else otherMenuColor = isLightBackGroundOtherMenuColor;

	//決定指示スプライトのカラー
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };
	//カラー更新
	UpdateChangeColor();

	//イージング
	for (int i = 0; i < GOMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	easeFadeInOut_.ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();

	//座標,カラーセット
	spriteGameOver_->SetPosition({ easeMenuPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[GOMEN_SelectSpace].num_X,menuPosY_[GOMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteFadeInOut_->SetColor({ deepRed_.x,deepRed_.y, deepRed_.z, easeFadeInOut_.num_X });//透明度だけ変える

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//落ちてるときのパーティクル
		FallParticle(player);

		//常時プレイヤーの回転をイージングにセットする
		for (int32_t i = 0; i < XYZ_Num; i++)
		{
			EaseRotateSetUp(player->GetRotation(), easePlayerRotateContinue_[i], i);
			EaseRotateSetUp(player->GetRotation(), easePlayerRotateQuitStageSelect_[i], i);
		}
	}
	//メニュー操作
	if (input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_S))menuCount_++;

	//色を変える(選択しているメニューは強調)
	if (menuCount_ == GOSMI_Continue)//コンティニュー選択時
	{
		spriteContinue_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[GOMEN_Continue] });
	}
	else if (menuCount_ == GOSMI_StageSelect)//ステージセレクト遷移を選択時
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[GOMEN_StageSelect] });
	}
	else if (menuCount_ == GOSMI_Title)//タイトル遷移を選択時
	{
		spriteContinue_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[GOMEN_Title] });
	}
	//決定時のスプライトのイージングが終わったら操作を受け付ける
	if (spriteDone_->GetPosition().x == easeMenuPosX_[GOMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			//決定スプライトからスキップキー表示へ切り替え
			spCommon_->LoadTexture(GOSTI_MenuDoneTex, "texture/skip.png");
			//イージングをスタンバイし次の処理へ
			if (menuCount_ == GOSMI_Continue)//コンティニュー
			{
				for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeContinue_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetContinue_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerRotateContinue_[i].Standby(false);
				easeContinuePosX_.Standby(false);
				easeContinuePosY_.Standby(false);
				easeCursorPosX_.Standby(true);
				//コンティニュー
				isContinue_ = true;
				isGameover_ = false;

			}
			else if (menuCount_ == GOSMI_StageSelect)//ステージセレクトへ戻る
			{
				for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeQuitStageSelect_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetQuitStageSelect_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerRotateQuitStageSelect_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				//ステージセレクトへ
				isQuitStageSelect_ = true;
				isGameover_ = false;
			}
			else if (menuCount_ == GOSMI_Title)//タイトルへ戻る
			{
				for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				//タイトルへ
				isQuitTitle_ = true;
				isGameover_ = false;

			}
		}
	}
	//カラーセット
	spriteGameOver_->SetColor(gameOverColor);
	spriteDone_->SetColor(doneColor);
}

void GameOverScene::UpdateIsContinue()
{
	//イージング
	for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeContinue_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetContinue_[i].ease_in_out_expo();
	easeContinuePosX_.ease_in_out_expo();
	easeContinuePosY_.ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteGameOver_->SetPosition({ easeMenuEndPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeContinuePosX_.num_X,easeContinuePosY_.num_X });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//カメラもセット
	camera_->SetEye({ easeEyeContinue_[XYZ_X].num_X, easeEyeContinue_[XYZ_Y].num_X, easeEyeContinue_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetContinue_[XYZ_X].num_X, easeTargetContinue_[XYZ_Y].num_X, easeTargetContinue_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{

		//自機の回転
		if (!completeRotate_)
		{
			//回転開始
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateContinue_[i].ease_in_expo();
			player->SetRotation({ easePlayerRotateContinue_[XYZ_X].num_X,easePlayerRotateContinue_[XYZ_Y].num_X,
				easePlayerRotateContinue_[XYZ_Z].num_X });

			//自機の回転が終わったら
			if (player->GetRotation().x == easePlayerRotateContinue_[XYZ_X].end)
			{
				//自機の座標イージングと自機のダッシュモデルをスタンバイ
				player->SetModel(modelPlayerContinue_.get());
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveContinue_[i].Standby(false);
				completeRotate_ = true;
			}
		}
		else//自機の回転のイージングが終わったら自機の座標のイージングを始める
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerMoveContinue_[i].ease_in_back();
			player->SetPosition({ easePlayerMoveContinue_[XYZ_X].num_X,easePlayerMoveContinue_[XYZ_Y].num_X,
				easePlayerMoveContinue_[XYZ_Z].num_X });
		}

		if (spriteTitle_->GetPosition().x == easeMenuEndPosX_[GOMEN_Title].end)FadeIn(white_);//白くする
		//スペースを押すとスキップ
		else SkipDirectionOnSpace(white_);

		//完全に白くなったら
		if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
		{
			sceneManager_->ChangeScene("GAMEPLAY", stageNum_);
		}
	}
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
	}
}

void GameOverScene::UpdateIsQuitStageSelect()
{
	//イージング
	for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeQuitStageSelect_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetQuitStageSelect_[i].ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteGameOver_->SetPosition({ easeMenuEndPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuEndPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//カメラもセット
	camera_->SetEye({ easeEyeQuitStageSelect_[XYZ_X].num_X, easeEyeQuitStageSelect_[XYZ_Y].num_X, easeEyeQuitStageSelect_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetQuitStageSelect_[XYZ_X].num_X, easeTargetQuitStageSelect_[XYZ_Y].num_X, easeTargetQuitStageSelect_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{

		//自機の回転
		if (!completeRotate_)
		{
			//回転開始
			for (int i = 0; i < XYZ_Num; i++)easePlayerRotateQuitStageSelect_[i].ease_in_expo();
			player->SetRotation({ easePlayerRotateQuitStageSelect_[XYZ_X].num_X,easePlayerRotateQuitStageSelect_[XYZ_Y].num_X ,easePlayerRotateQuitStageSelect_[XYZ_Z].num_X });
			//自機の回転が終わったら
			if (player->GetRotation().x == easePlayerRotateQuitStageSelect_[XYZ_X].end)
			{
				//自機の座標イージングと自機のダッシュモデルをスタンバイ
				player->SetModel(modelPlayerContinue_.get());
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveQuitStageSelect_[i].Standby(false);
				completeRotate_ = true;
			}
		}
		else//自機の回転のイージングが終わったら自機の座標のイージングを始める
		{
			for (int i = 0; i < XYZ_Num; i++)easePlayerMoveQuitStageSelect_[i].ease_in_quint();
			player->SetPosition({ easePlayerMoveQuitStageSelect_[XYZ_X].num_X,easePlayerMoveQuitStageSelect_[XYZ_Y].num_X,easePlayerMoveQuitStageSelect_[XYZ_Z].num_X });
		}

	}
	//メニューのイージングが終わったら遷移演出
	if (spriteTitle_->GetPosition().x == easeMenuEndPosX_[GOMEN_Title].end)FadeIn(black_);//黒くする
	//スペースを押すとスキップ
	else SkipDirectionOnSpace(black_);
	
	//完全に黒くなったらステージセレクトへ
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//ステージごとにステージセレクトの背景が違う
		if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
		else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
		else if (stageNum_ <= SL_StageTutorial_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);

	}
}

void GameOverScene::UpdateIsQuitTitle()
{
	//イージング
	for (int i = 0; i < GOMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteGameOver_->SetPosition({ easeMenuEndPosX_[GOMEN_Menu].num_X,menuPosY_[GOMEN_Menu] });
	spriteContinue_->SetPosition({ easeMenuEndPosX_[GOMEN_Continue].num_X,menuPosY_[GOMEN_Continue] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[GOMEN_StageSelect].num_X,menuPosY_[GOMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[GOMEN_Title].num_X,menuPosY_[GOMEN_Title] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[GOMEN_UI].num_X,menuPosY_[GOMEN_UI] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//自機は落っこちる
		DirectX::XMFLOAT3 move = player->GetPosition();
		const DirectX::XMFLOAT3 speed = { 0.0f,-1.0f,0.0f };

		move.y += speed.y;
		player->SetPosition(move);
		//落ちてるときのパーティクル
		FallParticle(player);
		//メニューのイージングが終わったら遷移演出
		if (spriteGameOver_->GetPosition().x == easeMenuEndPosX_[GOMEN_Menu].end)FadeIn(black_);//黒くする
		//スペースを押すとスキップ
		else SkipDirectionOnSpace(black_);

		//完全に黒くなったら
		if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
		{
			sceneManager_->ChangeScene("TITLE", stageNum_);//
		}
	}
}

void GameOverScene::Draw()
{
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();					//自機
	for (std::unique_ptr<Object3d>& stage : objStages_)stage->Draw();						//背景(天球)
	if (!isQuitStageSelect_)for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();	//ゴール

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	pmSmoke_->Draw();	//パーティクル(煙)
	pmFire_->Draw();	//パーティクル(炎)
	pmGoal_->Draw();	//パーティクル(ゴール)
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//前景スプライト
	spCommon_->PreDraw();
	//スプライト描画	
	spriteGameOver_->Draw();			 //ゲームオーバースプライト
	spriteContinue_->Draw();			 //コンティニュー表示スプライト
	spriteStageSelect_->Draw();			 //ステージセレクト表示スプライト
	spriteTitle_->Draw();				 //タイトル表示スプライト
	spriteDone_->Draw();				 //決定スプライト
	spriteFadeInOut_->Draw();			 //フェードインアウトスプライト
	spriteLoad_->Draw();				 //ロードスプライト
	spriteStageInfoNow_->Draw();		 //現在ステージスプライト
	spriteCursor_->Draw();				 //カーソルスプライト
	spriteStageName_->Draw();			 //ステージ名スプライト
	spriteMenuUI_->Draw();				//メニュー操作方法スプライト
}

void GameOverScene::Finalize()
{
	//音声
	audio_->Finalize();

}

void GameOverScene::LoadLVData([[maybe_unused]] const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);

	// モデルを読み込んで登録
	ModelMapping(modelPlayer_, "playerhit");			  //自機モデル
	ModelMapping(modelPlayerContinue_, "playerdash");	  //自機モデル（コンティニュー）
	ModelMapping(modelGoal_, "sphere");					  //ゴールモデル
	ModelMapping(modelStageTutorial_, "skydomet");		  //チュートリアルステージモデル(天球)
	ModelMapping(modelStage1_, "skydome");				  //ステージ1モデル(天球)
	ModelMapping(modelStage2_, "skydome2");				  //ステージ2モデル(天球)

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
		else if (objectData.objectType.find("SKYDOME") == LDTOF_TRUE)
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
			objStages_.push_back(std::move(newObject));
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

void GameOverScene::FadeIn(const DirectX::XMFLOAT3& color)
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
		const XMFLOAT3 negapozi = { 1.0f - color.x,1.0f - color.y,1.0f - color.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOut_.num_X });//透明度だけ変える
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ネガポジの応用
		if (isContinue_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
		}
	}
}

void GameOverScene::UpdateChangeColor()
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

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.9f,0.0f };//{max,min}

	if (selectColor_.x >= maxAndMinSpeedColor.x)//Max値に到達したら反転
	{
		isColorReverse_ = true;
	}
	if (selectColor_.x <= maxAndMinSpeedColor.y)//Min値に到達したら反転を戻す
	{
		isColorReverse_ = false;
	}
}

void GameOverScene::EaseRotateSetUp(const DirectX::XMFLOAT3& rotation, Easing& easing, const int32_t num)
{
	//イージングの回転を設定
	XMFLOAT3 rot = rotation;
	if (num == XYZ_X)easing.SetEasing(rot.x, easing.end, easing.maxtime);
	if (num == XYZ_Y)easing.SetEasing(rot.y, easing.end, easing.maxtime);
	if (num == XYZ_Z)easing.SetEasing(rot.z, easing.end, easing.maxtime);
}

void GameOverScene::FallParticle(const std::unique_ptr<Object3d>& player)
{
	const DirectX::XMFLOAT2 dashOffsetXY = { -2.0f,-1.5f };//ポジションオフセット
	//煙プリセット
	const ParticleManager::Preset smoke =
	{
		particleSmoke_.get(),
		{player->GetPosition().x + dashOffsetXY.x,player->GetPosition().y + dashOffsetXY.y,player->GetPosition().z},
		{ 3.0f ,4.0f,0.0f },
		{ 0.15f,2.3f,0.2f },
		{ 0.0f,0.001f,0.0f },
		2,
		{ 1.0f, 5.0f },
		{0.5f,0.5f,0.5f,1.0f },
		{ 0.0f,0.0f,0.0f,0.0f }
	};
	//風プリセット
	const ParticleManager::Preset wind =
	{
		particleFall_.get(),
		player->GetPosition(),
		{ 200.0f ,-100.0f,5.0f },
		{ 0.0f,3.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		8,
		{ 1.0f, 0.0f },
		{1.0f,1.0f,1.0,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};
	//煙パーティクル
	pmSmoke_->ActiveY(smoke);
	//風っぽいパーティクル
	pmFire_->ActiveY(wind);

}

void GameOverScene::LoadStageNameSprite()
{
	//ステージごとにステージ名を変更
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-1.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-1.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-2.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-3.png");
	else if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(GOSTI_StageNameTex, "texture/stagename/2-4.png");
	//アンカーポイントなどをセッティング
	spriteStageName_->Initialize(spCommon_, GOSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化
	spriteStageName_->Update();
}