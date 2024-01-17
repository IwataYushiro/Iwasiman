#include "TitleScene.h"
#include "FbxLoader.h"

#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace IwasiEngine;
/*

*	TitleScene.cpp

*	タイトルシーン

*/

//静的メンバ変数の実体
DirectXCommon* TitleScene::dxCommon_ = DirectXCommon::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
Audio* TitleScene::audio_ = Audio::GetInstance();
SceneManager* TitleScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* TitleScene::imguiManager_ = ImGuiManager::GetInstance();

TitleScene::TitleScene(const int stagenum) : stageNum_(stagenum) {}

void TitleScene::Initialize()
{
	//インスタンス取得
	spCommon_ = SpriteCommon::GetInstance();//スプライト基盤

	//カメラ初期化
	camera_ = std::make_unique<Camera>();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ easeEyeMenu_[XYZ_X].start, easeEyeMenu_[XYZ_Y].start, easeEyeMenu_[XYZ_Z].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].start, easeTargetMenu_[XYZ_Y].start, easeTargetMenu_[XYZ_Z].start });

	//レベルデータ読み込み
	if (stageNum_ == SL_Default)LoadLVData("scene/titlet");				//チュートリアルから戻ってきた場合
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/title1");	//空ステージから戻ってきた場合
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/title2");	//宇宙ステージから戻ってきた場合
	else LoadLVData("scene/titlet");									//初期のレベル

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);//0番シャドウだけを動かす

	//スプライト
	//タイトル画面スプライト
	spCommon_->LoadTexture(TSTI_TitleTex, "texture/title3.png");
	spriteTitle_->Initialize(spCommon_, TSTI_TitleTex);
	spriteTitle_->SetColor(backTitleColor_);
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].start,startTitlePosY_[TS_Title] });

	//タイトル画面スプライト(タイトルの後ろ)
	spCommon_->LoadTexture(TSTI_TitleBackTex, "texture/titleback.png");
	spriteTitleBack_->Initialize(spCommon_, TSTI_TitleBackTex);
	spriteTitleBack_->SetPosition({ easeTitlePosX_[TS_Title].start,startTitlePosY_[TS_Title] });

	//タイトル決定表示スプライト
	spCommon_->LoadTexture(TSTI_TitleDoneTex, "texture/space3.png");
	spriteTitleDone_->Initialize(spCommon_, TSTI_TitleDoneTex);
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].start,startTitlePosY_[TS_Done] });

	//タイトルメニュー画面スプライト
	spCommon_->LoadTexture(TSTI_MenuTex, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, TSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].start,menuPosY_[TMEN_Menu] });

	//チュートリアル表示スプライト
	spCommon_->LoadTexture(TSTI_MenuTutorialTex, "texture/titlemenug.png");
	spriteMenuTutorial_->Initialize(spCommon_, TSTI_MenuTutorialTex);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].start,easeStartStagePosY_.start });

	//ステージセレクト表示スプライト
	spCommon_->LoadTexture(TSTI_MenuStageSerectTex, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, TSTI_MenuStageSerectTex);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].start,menuPosY_[TMEN_StageSelect] });

	//タイトルメニュー決定表示スプライト
	spCommon_->LoadTexture(TSTI_MenuDoneTex, "texture/space.png");
	spriteMenuDone_->Initialize(spCommon_, TSTI_MenuDoneTex);
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].start,menuPosY_[TMEN_SelectSpace] });

	//タイトルメニュー→タイトル移行のスプライト
	spCommon_->LoadTexture(TSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, TSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].start,menuPosY_[TMEN_Quit] });
	spriteBack_->SetColor(backTitleColor_);

	//フェードインアウトスプライト
	spCommon_->LoadTexture(TSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, TSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.start });

	//ロードスプライト
	spCommon_->LoadTexture(TSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, TSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.end });//透明化

	//現在ステージスプライト
	spCommon_->LoadTexture(TSTI_StageInfoNowTex, "texture/stage1.png");
	spriteStageInfoNow_->Initialize(spCommon_, TSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//カーソルスプライト
	spCommon_->LoadTexture(TSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, TSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[TMEN_Tutorial] });

	//ステージ名スプライト
	spCommon_->LoadTexture(TSTI_StageNameTex, "texture/stagename/1-1.png");
	spriteStageName_->Initialize(spCommon_, TSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//メニュー操作スプライト
	spCommon_->LoadTexture(TSTI_MenuUITex, "texture/menuui.png");
	spriteMenuUI_->Initialize(spCommon_, TSTI_MenuUITex);
	spriteMenuUI_->SetPosition({ easeMenuPosX_[TMEN_UI].start,menuPosY_[TMEN_UI] });

	//パーティクル
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	particleGoal_ = Particle::LoadFromParticleTexture("particle1.png");
	//ブースト用
	pmFire_ = ParticleManager::Create();
	pmFire_->SetBlendMode(ParticleManager::BP_ADD);
	pmFire_->SetParticleModel(particleFire_.get());
	pmFire_->SetCamera(camera_.get());
	//ゴールオブジェクト用
	pmGoal_ = ParticleManager::Create();
	pmGoal_->SetParticleModel(particleGoal_.get());
	pmGoal_->SetCamera(camera_.get());

	//イージングスタンバイ
	easeFadeInOut_.Standby(false);
}

void TitleScene::Update()
{

	//メニュー上下限
	if (menuCount_ <= TSMI_Tutorial)menuCount_ = TSMI_Tutorial;
	else if (menuCount_ >= TSMI_StageSelect)menuCount_ = TSMI_StageSelect;

	//タイトルカラー
	DirectX::XMFLOAT4 titleColor;
	const DirectX::XMFLOAT4 isLightBackGroundTitleColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };//明るい背景
	const DirectX::XMFLOAT4 isDarkBackGroundTitleColor = { selectColor_.x + 0.1f,selectColor_.y + 0.1f,1.0f,1.0f };//暗い背景

	//戻ってきたステージによって文字の色を変える
	if (stageNum_ == SL_Default)titleColor = isLightBackGroundTitleColor;
	else if (stageNum_ <= SL_Stage1_StageID)titleColor = isLightBackGroundTitleColor;
	else if (stageNum_ <= SL_Stage2_StageID)titleColor = isDarkBackGroundTitleColor;
	else titleColor = isLightBackGroundTitleColor;

	//決定指示スプライトのカラー
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };

	//SetColorより前に呼び出す
	UpdateChangeColor();

	if (isStartGame_)UpdateIsStartGame();			//ゲームスタート時
	else if (isStageSelect_)UpdateIsStageSelect();	//ステージセレクト遷移時
	else if (isMenu_)UpdateIsMenu();				//メニュー時
	else if (isBack_)UpdateIsBack();				//タイトルへ戻った時
	else											//タイトル画面時
	{
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });//透明度だけ変える

		if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.end)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].Standby(false);
				for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].Standby(false);
				easeCursorPosX_.Standby(false);
				isMenu_ = true;

			}
		}
	}
	//カラーセット
	spriteTitle_->SetColor(titleColor);
	spriteMenu_->SetColor(titleColor);
	spriteTitleDone_->SetColor(doneColor);
	spriteMenuDone_->SetColor(doneColor);

	//スプライト更新
	spriteTitle_->Update();					//タイトル画面スプライト
	spriteTitleDone_->Update();				//タイトル決定表示スプライト
	spriteMenu_->Update();					//タイトルメニュー画面スプライト
	spriteMenuTutorial_->Update();			//チュートリアル表示スプライト
	spriteMenuStageSelect_->Update();		//ステージセレクト表示スプライト
	spriteMenuDone_->Update();				//タイトルメニュー決定表示スプライト
	spriteBack_->Update();					//タイトルメニュー→タイトル移行のスプライト
	spriteFadeInOut_->Update();				//フェードインアウトスプライト
	spriteLoad_->Update();					//ロードスプライト
	spriteStageInfoNow_->Update();			//現在ステージスプライト
	spriteCursor_->Update();				//カーソルスプライト
	spriteTitleBack_->Update();				//タイトル画面スプライト(タイトルの後ろ)
	spriteStageName_->Update();				//ステージ名スプライト
	spriteMenuUI_->Update();				//メニュー操作方法スプライト

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		const DirectX::XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//ポジションオフセット

		//煙プリセット
		const ParticleManager::Preset smoke =
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
		//パーティクル
		pmFire_->ActiveX(smoke);

		//丸影
		SetUpCircleShadow(player->GetPosition());
		//更新
		player->Update();
	}

	for (std::unique_ptr<Object3d>& ground : objGrounds_)
	{
		DirectX::XMFLOAT3 move = ground->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };
		//X値がここまで来たらループ
		const float returnPos_ = -120.0f;
		//常時進む
		move.x += speed.x;
		ground->SetPosition(move);
		if (ground->GetPosition().x <= returnPos_)ground->SetPosition(startPos_);
		//更新
		ground->Update();
	}

	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)
	{
		//天球回転用
		DirectX::XMFLOAT3 rotSkydome = skydome->GetRotation();
		//常時回る
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);
		//更新
		skydome->Update();
	}
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Update();
	//更新
	camera_->Update();		//カメラ
	lightGroup_->Update();	//ライト
	pmFire_->Update();			//パーティクルマネージャー(ジェット)
	pmGoal_->Update();			//パーティクルマネージャー(ゴールオブジェクト)

	//objF->Update();

	//ImGui
	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(true);//デバッグカメラ
#endif // _DEBUG

	imguiManager_->End();
}

void TitleScene::UpdateIsStartGame()
{
	//イージング
	for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_out_expo();
	easeStartStagePosX_.ease_out_expo();
	easeStartStagePosY_.ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//座標セット
	spriteMenu_->SetPosition({ easeMenuEndPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeStartStagePosX_.num_X,easeStartStagePosY_.num_X });
	spriteMenuStageSelect_->SetPosition({ easeMenuEndPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuEndPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[TMEN_UI].num_X,menuPosY_[TMEN_UI] });
	spriteBack_->SetPosition({ easeMenuEndPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);

	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& goal : objGoals_)
	{
		//ここでゴールオブジェクトが動く
		DirectX::XMFLOAT3 move = goal->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.5f,0.0f,0.0f };
		//X値がここまで来たらイージング
		const float gameStartPos_ = 60.0f;
		//進む
		move.x += speed.x;
		goal->SetPosition(move);

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
		//ゴールオブジェクトの位置が一定の位置に到着したら遷移演出
		if (goal->GetPosition().x <= gameStartPos_)FadeIn(white_);//ゲームプレイ遷移時は白くする
		else SkipDirectionOnSpace(white_);//スペースを押すとスキップ

	}
	//完全に白くなったら
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);//チュートリアルステージへ
}

void TitleScene::UpdateIsStageSelect()
{
	//イージング
	for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easePlayerMove_[i].ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteMenu_->SetPosition({ easeMenuEndPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuEndPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial] });
	spriteMenuStageSelect_->SetPosition({ easeMenuEndPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuEndPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuEndPosX_[TMEN_UI].num_X,menuPosY_[TMEN_UI] });
	spriteBack_->SetPosition({ easeMenuEndPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);

	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });
	//じきの座標セット
	for (std::unique_ptr<Object3d>& player : objPlayers_) player->SetPosition({ easePlayerMove_[XYZ_X].num_X,easePlayerMove_[XYZ_Y].num_X,easePlayerMove_[XYZ_Z].num_X });
	//メニューのイージングが終わったら遷移演出
	if (spriteMenu_->GetPosition().x == easeMenuEndPosX_[TMEN_Menu].end)FadeIn(black_);//黒くする	
	else SkipDirectionOnSpace(black_);//スペースを押すとスキップ
	//完全に黒くなったら
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//ステージ選択
		if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
		else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
		else sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);//チュートリアルに飛ばすと本末転倒
	}


}

void TitleScene::UpdateIsBack()
{
	//イージング
	for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleBack_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].num_X,startTitlePosY_[TS_Done] });
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial] });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuPosX_[TMEN_UI].num_X,menuPosY_[TMEN_UI] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//カメラもセット
	camera_->SetEye({ easeEyeMenu_[XYZ_X].num_X, easeEyeMenu_[XYZ_Y].num_X, easeEyeMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].num_X, easeTargetMenu_[XYZ_Y].num_X, easeTargetMenu_[XYZ_Z].num_X });

	//戻る用のスプライトのイージングが終わったら操作を受け付ける
	if (spriteBack_->GetPosition().x == easeMenuPosX_[TMEN_Quit].start)
	{
		if (input_->TriggerKey(DIK_SPACE))//メニューへ
		{
			for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].Standby(false);
			for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].Standby(false);
			easeCursorPosX_.Standby(false);
			isMenu_ = true;
			isBack_ = false;
		}
	}
}

void TitleScene::UpdateIsMenu()
{
	//選択してる色
	DirectX::XMFLOAT4 selectMenuColor;
	const DirectX::XMFLOAT4 isLightBackGroundSelectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 isDarkBackGroundSelectMenuColor = { 1.0f,selectColor_.y + 0.1f,selectColor_.z + 0.1f,1.0f };
	//ステージ位置(背景)によって色を変える
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

	//イージング
	for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleBack_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].num_X,startTitlePosY_[TS_Done] });
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial] });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteMenuUI_->SetPosition({ easeMenuPosX_[TMEN_UI].num_X,menuPosY_[TMEN_UI] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });

	//カメラもセット
	camera_->SetEye({ easeEyeMenu_[XYZ_X].num_X, easeEyeMenu_[XYZ_Y].num_X, easeEyeMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].num_X, easeTargetMenu_[XYZ_Y].num_X, easeTargetMenu_[XYZ_Z].num_X });

	//メニュー選択
	if (input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_S))menuCount_++;

	//色を変える(選択しているメニューは強調)
	if (menuCount_ == TSMI_Tutorial)//チュートリアル遷移を選択時
	{
		spriteMenuTutorial_->SetColor(selectMenuColor);
		spriteMenuStageSelect_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[TMEN_Tutorial] });
	}
	else if (menuCount_ == TSMI_StageSelect)//ステージセレクト遷移を選択時
	{
		spriteMenuTutorial_->SetColor(otherMenuColor);
		spriteMenuStageSelect_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[TMEN_StageSelect] });
	}

	//メニューのイージングが終わったら操作を受け付ける
	if (spriteMenu_->GetPosition().x == easeMenuPosX_[TMEN_Menu].end)
	{
		if (input_->TriggerKey(DIK_SPACE))//決定
		{
			if (menuCount_ == TSMI_Tutorial)//チュートリアルを選んだ場合チュートリアルへ遷移
			{
				//イージングのスタンバイ
				for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
				easeStartStagePosX_.Standby(false);
				easeStartStagePosY_.Standby(false);
				easeCursorPosX_.Standby(true);
				isStartGame_ = true;
			}
			else if (menuCount_ == TSMI_StageSelect)//ステージセレクトを選んだらステージセレクトへ遷移
			{
				//イージングのスタンバイ
				for (int i = 0; i < TMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerMove_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isStageSelect_ = true;

			}
		}
		if (input_->TriggerKey(DIK_ESCAPE))//ESCキーでタイトルに戻る
		{
			//イージングのスタンバイ
			for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].Standby(true);
			for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].Standby(true);
			easeCursorPosX_.Standby(true);
			isBack_ = true;
			isMenu_ = false;
		}
	}

}

void TitleScene::FadeIn(const DirectX::XMFLOAT3& color)
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
		if (isStartGame_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
		}
	}
}

void TitleScene::Draw()
{
	//背景スプライト描画前処理

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();				//プレイヤー
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Draw();				//地面
	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)skydome->Draw();				//天球(背景)
	if (!isStageSelect_)for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();	//ゴール
	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//objF->Draw();
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
	spriteTitleBack_->Draw();				//タイトル画面スプライト(タイトルの後ろ)
	spriteTitle_->Draw();					//タイトル画面スプライト
	spriteTitleDone_->Draw();				//タイトル決定表示スプライト
	spriteMenu_->Draw();					//タイトルメニュー画面スプライト
	spriteMenuTutorial_->Draw();			//チュートリアル表示スプライト
	spriteMenuStageSelect_->Draw();			//ステージセレクト表示スプライト
	spriteMenuDone_->Draw();				//タイトルメニュー決定表示スプライト
	spriteBack_->Draw();					//タイトルメニュー→タイトル移行のスプライト
	spriteFadeInOut_->Draw();				//フェードインアウトスプライト
	spriteLoad_->Draw();					//ロードスプライト
	spriteStageInfoNow_->Draw();			//現在ステージスプライト
	spriteCursor_->Draw();					//カーソルスプライト
	spriteStageName_->Draw();				//ステージ名スプライト
	spriteMenuUI_->Draw();					//メニュー操作方法スプライト
}

void TitleScene::Finalize()
{
	//音声
	audio_->Finalize();
}

void TitleScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);

	// モデルを読み込んで登録
	ModelMapping(modelPlayer_, "playerdash", true);		//自機モデル
	ModelMapping(modelSkydome_, "skydomet");			//チュートリアルステージモデル(天球)
	ModelMapping(modelSkydomeStage1_, "skydome");		//ステージ1モデル(天球)
	ModelMapping(modelSkydomeStage2_, "skydome2");		//ステージ2モデル(天球)
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
			startPos_ = newObject->GetPosition();

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
			objSkydomes_.push_back(std::move(newObject));
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

void TitleScene::UpdateChangeColor()
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

void TitleScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
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
