#include "StageClearScene.h"
#include "FbxLoader.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace IwasiEngine;
using namespace MyMath;
/*

*	StageClearScene.cpp

*	ステージクリアシーン

*/

//静的メンバ変数の実体
DirectXCommon* StageClearScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageClearScene::input_ = Input::GetInstance();
Audio* StageClearScene::audio_ = Audio::GetInstance();
SceneManager* StageClearScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageClearScene::imguiManager_ = ImGuiManager::GetInstance();

StageClearScene::StageClearScene(const int stagenum) :stageNum_(stagenum) {}

void StageClearScene::Initialize()
{
	//インスタンス取得
	spCommon_ = SpriteCommon::GetInstance();

	//カメラ
	camera_ = std::make_unique<Camera>();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].start, easeEyeStageClear_[XYZ_Y].start, easeEyeStageClear_[XYZ_Z].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].start, easeTargetStageClear_[XYZ_Y].start, easeTargetStageClear_[XYZ_Z].start });

	//レベルデータ読み込み
	if (stageNum_ == SL_Default)LoadLVData("scene/stagecleart");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/stageclear1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/stageclear2");
	else LoadLVData("scene/stagecleart");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);

	//スプライト
	//ステージクリア時のスプライト
	if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_Stage2_AreaBoss)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(SCSTI_MenuTex, "texture/areaclear.png");
	else spCommon_->LoadTexture(SCSTI_MenuTex, "texture/stageclear2.png");
	spriteStageClear_->Initialize(spCommon_, SCSTI_MenuTex);
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].start,menuPosY_[SCMEN_Menu] });

	//次のステージ表示のスプライト
	spCommon_->LoadTexture(SCSTI_NextStageTex, "texture/nextstage.png");
	spriteNextStage_->Initialize(spCommon_, SCSTI_NextStageTex);
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].start,menuPosY_[SCMEN_NextStage] });

	//ステージセレクト表示のスプライト
	spCommon_->LoadTexture(SCSTI_StageSelectTex, "texture/backstageselect.png");
	spriteStageSelect_->Initialize(spCommon_, SCSTI_StageSelectTex);
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].start,menuPosY_[SCMEN_StageSelect] });

	//タイトル表示のスプライト
	spCommon_->LoadTexture(SCSTI_TitleTex, "texture/backtitle.png");
	spriteTitle_->Initialize(spCommon_, SCSTI_TitleTex);
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].start,menuPosY_[SCMEN_Title] });

	//決定表示のスプライト
	spCommon_->LoadTexture(SCSTI_MenuDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, SCSTI_MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].start,menuPosY_[SCMEN_SelectSpace] });

	//フェードインアウトのスプライト
	spCommon_->LoadTexture(SCSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, SCSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });

	//ロードスプライト
	spCommon_->LoadTexture(SCSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, SCSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//現在ステージスプライト
	const int remainderNum = stageNum_ % 10;//余りによってスプライトを変える
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(SCSTI_StageInfoNowTex, "texture/stagef.png");
	spriteStageInfoNow_->Initialize(spCommon_, SCSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//カーソルスプライト
	spCommon_->LoadTexture(SCSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, SCSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,menuPosY_[SCMEN_NextStage] });

	//ステージ名スプライト
	LoadStageNameSprite();

	//パーティクル
	//クリア演出用
	particleClear_ = Particle::LoadFromParticleTexture("particle1.png");
	pmClear_ = ParticleManager::Create();
	pmClear_->SetParticleModel(particleClear_.get());
	pmClear_->SetCamera(camera_.get());
	//自機の移動演出用
	particleSmoke_ = Particle::LoadFromParticleTexture("particle8.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());
	pmSmoke_->SetCamera(camera_.get());

	//各ステージの最終面は次のステージへ行く表記が無い
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

	//イージングスタンバイ
	easeFadeInOut_.Standby(false);
	for (int i = 0; i < SCMEN_Num; i++)easeMenuPosX_[i].Standby(false);
	easeCursorPosX_.Standby(false);
}

void StageClearScene::Update()
{
	if (isNextStage_)UpdateIsNextStage();				//次のステージへ遷移する時
	else if (isStageSelect_)UpdateIsStageSelect();		//ステージセレクト遷移時
	else if (isQuitTitle_)UpdateIsQuitTitle();			//タイトルへ遷移する時
	else UpdateIsMenu();								//メニュー画面時

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		const XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//オフセット
		//煙プリセット
		const ParticleManager::Preset smoke =
		{
			particleSmoke_.get(),
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
		pmSmoke_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);
		
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
	//天球(背景)
	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)
	{
		//天球回転用
		DirectX::XMFLOAT3 rotSkydome = skydome->GetRotation();
		//天球(背景)は常時回転する
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);
		//更新
		skydome->Update();
	}
	//ゴール
	for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Update();//更新


	//スプライト更新
	spriteStageClear_->Update();		   //ステージクリア時のスプライト
	spriteNextStage_->Update();			   //次のステージ表示のスプライト
	spriteStageSelect_->Update();		   //ステージセレクト表示のスプライト
	spriteTitle_->Update();				   //タイトル表示のスプライト
	spriteDone_->Update();				   //決定表示のスプライト
	spriteFadeInOut_->Update();			   //フェードインアウトのスプライト
	spriteLoad_->Update();				   //ロードスプライト
	spriteStageInfoNow_->Update();		   //現在ステージスプライト
	spriteCursor_->Update();			   //カーソルスプライト
	spriteStageName_->Update();			   //ステージ名スプライト

	//更新
	camera_->Update();		   //カメラ
	lightGroup_->Update();	   //ライト
	pmClear_->Update();		   //クリア時のパーティクル
	pmSmoke_->Update();		   //煙のパーティクル
	//ImGui
	imguiManager_->Begin();
	imguiManager_->End();

}

void StageClearScene::UpdateIsNextStage()
{
	//イージング
	for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//座標セット
	spriteStageClear_->SetPosition({ easeMenuEndPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuEndPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//カメラもセット
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].num_X, easeEyeStageClear_[XYZ_Y].num_X, easeEyeStageClear_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].num_X, easeTargetStageClear_[XYZ_Y].num_X, easeTargetStageClear_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//煙プリセット
		const ParticleManager::Preset clear =
		{
			particleClear_.get(),
			{player->GetPosition().x + 70.0f,player->GetPosition().y + 30.0f ,player->GetPosition().z},
			{ 0.0f ,60.0f,75.0f },
			{  RandomMTFloat(-15.0f,-8.0f),0.0f,0.0f},
			{ 0.0f,0.001f,0.0f },
			3,
			{ RandomMTFloat(4.0f,6.0f), 0.0f },
			{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f},
			{ RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f }
		};
		//パーティクル
		pmClear_->ActiveX(clear.particle, clear.startPos, clear.pos, clear.vel,
			clear.acc, clear.num, clear.scale, clear.startColor, clear.endColor);

	}

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
		//ゴールオブジェクトの位置が一定の位置に到着したら遷移演出
		if (goal->GetPosition().x <= gameStartPos_)
		{
			FadeOut(white_);//ゲームプレイ遷移時は白くする
			//完全に白くなったら
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				if (stageNum_ == SL_Stage1_AreaBoss) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
				else if (stageNum_ == SL_StageTutorial_Final) sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else sceneManager_->ChangeScene("GAMEPLAY", ++stageNum_);
			}
		};


	}
}

void StageClearScene::UpdateIsStageSelect()
{
	//イージング
	for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easePlayerMoveStageSelect_[i].ease_in_out_expo();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteStageClear_->SetPosition({ easeMenuEndPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuEndPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);
	//カメラもセット
	camera_->SetEye({ easeEyeStageClear_[XYZ_X].num_X, easeEyeStageClear_[XYZ_Y].num_X, easeEyeStageClear_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetStageClear_[XYZ_X].num_X, easeTargetStageClear_[XYZ_Y].num_X, easeTargetStageClear_[XYZ_Z].num_X });

	for (std::unique_ptr<Object3d>& player : objPlayers_)
	{
		//自機の座標をセット
		player->SetPosition({ easePlayerMoveStageSelect_[XYZ_X].num_X,easePlayerMoveStageSelect_[XYZ_Y].num_X,easePlayerMoveStageSelect_[XYZ_Z].num_X });
		//メニューのイージングが終わったら遷移演出
		if (spriteStageClear_->GetPosition().x == easeMenuEndPosX_[SCMEN_Menu].end)
		{
			FadeOut(black_);//黒くする
			//完全に黒くなったら
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//ステージ選択
				if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
				else sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);//ここはチュートリアルに飛ばしてもいい
			}
		}
	}
}

void StageClearScene::UpdateIsQuitTitle()
{
	//イージング
	for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].ease_in_out_quint();
	easeCursorPosX_.ease_out_expo();
	//座標セット
	spriteStageClear_->SetPosition({ easeMenuEndPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuEndPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuEndPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuEndPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuEndPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);

	//メニューのイージングが終わったら遷移演出
	if (spriteStageClear_->GetPosition().x == easeMenuEndPosX_[SCMEN_Menu].end)FadeOut(black_);//黒くする
	//完全に黒くなったら
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
		if (menuCount_ <= SCSMI_NextStage)menuCount_ = SCSMI_NextStage;
		else if (menuCount_ >= SCSMI_Title)menuCount_ = SCSMI_Title;
	}
	else//最終面時は次のステージの表記が無い
	{
		if (menuCount_ <= SCSMI_StageSelect)menuCount_ = SCSMI_StageSelect;
		else if (menuCount_ >= SCSMI_Title)menuCount_ = SCSMI_Title;
	}

	//選択中のメニューカラー
	const DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,0.7f };
	//ステージクリアのカラー
	const DirectX::XMFLOAT4 clearColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	//決定指示スプライトのカラー
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.6f + selectColor_.x };
	//カラー更新
	UpdateChangeColor();

	//イージング
	easeMenuPosX_[SCMEN_Menu].ease_out_expo();
	for (int i = SCMEN_NextStage; i < SCMEN_Num; i++)easeMenuPosX_[i].ease_in_expo();
	easeFadeInOut_.ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();
	//座標セット
	spriteStageClear_->SetPosition({ easeMenuPosX_[SCMEN_Menu].num_X,menuPosY_[SCMEN_Menu] });
	spriteNextStage_->SetPosition({ easeMenuPosX_[SCMEN_NextStage].num_X,menuPosY_[SCMEN_NextStage] });
	spriteStageSelect_->SetPosition({ easeMenuPosX_[SCMEN_StageSelect].num_X,menuPosY_[SCMEN_StageSelect] });
	spriteTitle_->SetPosition({ easeMenuPosX_[SCMEN_Title].num_X,menuPosY_[SCMEN_Title] });
	spriteDone_->SetPosition({ easeMenuPosX_[SCMEN_SelectSpace].num_X,menuPosY_[SCMEN_SelectSpace] });

	spriteFadeInOut_->SetColor({ white_.x,white_.y, white_.z, easeFadeInOut_.num_X });//透明度だけ変える

	//メニュー操作
	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	//色を変える(選択しているメニューは強調)
	if (menuCount_ == SCSMI_NextStage)//次のステージを選択時
	{
		spriteNextStage_->SetColor(selectMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SCMEN_NextStage] });
	}
	else if (menuCount_ == SCSMI_StageSelect)//ステージセレクト遷移を選択時
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(selectMenuColor);
		spriteTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SCMEN_StageSelect] });
	}
	else if (menuCount_ == SCSMI_Title)//タイトル遷移時
	{
		spriteNextStage_->SetColor(otherMenuColor);
		spriteStageSelect_->SetColor(otherMenuColor);
		spriteTitle_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,menuPosY_[SCMEN_Title] });
	}
	//決定時のスプライトのイージングが終わったら操作を受け付ける
	if (spriteDone_->GetPosition().x == easeMenuPosX_[SCMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			//イージングをスタンバイし次の処理へ
			if (menuCount_ == SCSMI_NextStage)//次のステージへ
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isNextStage_ = true;
			}
			else if (menuCount_ == SCSMI_StageSelect)//ステージセレクトへ戻る
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeEyeStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetStageClear_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerMoveStageSelect_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isStageSelect_ = true;
			}
			else if (menuCount_ == SCSMI_Title)//タイトルへ戻る
			{
				for (int i = 0; i < SCMEN_Num; i++)easeMenuEndPosX_[i].Standby(false);
				easeCursorPosX_.Standby(true);
				isQuitTitle_ = true;
			}


		}
	}
	
	//クリア演出プリセット
	const ParticleManager::Preset clear =
	{
		particleClear_.get(),
		{0.0f,0.0f,0.0f},//使わない
		{ 5.0f ,2.0f,0.0f },
		{ 3.0f,6.0f,0.3f },
		{ 0.0f,0.001f,0.0f },
		3,
		{ RandomMTFloat(4.0f,6.0f), 0.0f },
		{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f},
		{ RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f }
	};
	
	const DirectX::XMFLOAT3 clearStartPosLeft = { -30.0f,-30.0f,-5.0f };//左側
	const DirectX::XMFLOAT3 clearStartPosRight = { 30.0f,-30.0f,-5.0f };//右側

	//左側に
	pmClear_->ActiveY(clear.particle, clearStartPosLeft, clear.pos, clear.vel,
		clear.acc, clear.num, clear.scale, clear.startColor, clear.endColor);

	//右側に
	pmClear_->ActiveY(clear.particle, clearStartPosRight, clear.pos, clear.vel,
		clear.acc, clear.num, clear.scale, clear.startColor, clear.endColor);
	//カラーセット
	spriteStageClear_->SetColor(clearColor);
	spriteDone_->SetColor(doneColor);
}

void StageClearScene::Draw()
{
	//背景スプライト描画前処理
	spCommon_->PreDraw();

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	for (std::unique_ptr<Object3d>& player : objPlayers_)player->Draw();				  //自機
	for (std::unique_ptr<Object3d>& ground : objGrounds_)ground->Draw();				  //天球
	for (std::unique_ptr<Object3d>& skydome : objSkydomes_)skydome->Draw();				  //床
	if (!isStageSelect_)for (std::unique_ptr<Object3d>& goal : objGoals_)goal->Draw();	  //ゴール

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());


	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());
	pmClear_->Draw();	//クリアエフェクト
	pmSmoke_->Draw();	//煙エフェクト
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//前景スプライト
	spCommon_->PreDraw();
	//スプライト描画	
	spriteStageClear_->Draw();					   //ステージクリア時のスプライト
	if (!isFinalStage_)spriteNextStage_->Draw();   //次のステージ表示のスプライト(最終面の場合スキップ)
	spriteStageSelect_->Draw();					   //ステージセレクト表示のスプライト
	spriteTitle_->Draw();						   //タイトル表示のスプライト
	spriteDone_->Draw();						   //決定表示のスプライト
	spriteFadeInOut_->Draw();					   //フェードインアウトのスプライト
	spriteLoad_->Draw();						   //ロードスプライト
	spriteStageInfoNow_->Draw();				   //現在ステージスプライト
	spriteCursor_->Draw();						   //カーソルスプライト
	spriteStageName_->Draw();					   //ステージ名スプライト
}

void StageClearScene::FadeOut(const DirectX::XMFLOAT3& rgb)
{
	if (!isFadeOut_)
	{
		//ここでスタンバイ
		easeFadeInOut_.Standby(true);
		isFadeOut_ = true;
	}
	else
	{
		//ここでフェードインアウト
		const DirectX::XMFLOAT3 negapozi = { 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//透明度だけ変える
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ネガポジの応用
		if (isNextStage_)
		{
			spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
			spriteStageName_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
		}
	}
}

void StageClearScene::Finalize()
{
	//音声
	audio_->Finalize();
}

void StageClearScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);

	// モデル読み込み
	modelPlayer_ = Model::LoadFromOBJ("playerdash", true);		 //自機モデル
	modelSkydome_ = Model::LoadFromOBJ("skydomet");				 //チュートリアルステージモデル(天球)
	modelSkydomeStage1_ = Model::LoadFromOBJ("skydome");		 //ステージ1モデル(天球)
	modelSkydomeStage2_ = Model::LoadFromOBJ("skydome2");		 //ステージ2モデル(天球)
	modelGround_ = Model::LoadFromOBJ("ground");				 //床のモデル
	modelGoal_ = Model::LoadFromOBJ("sphere");					 //ゴールモデル
	//マップに登録する
	models_.insert(std::make_pair("playerdash", modelPlayer_.get()));
	models_.insert(std::make_pair("skydomet", modelSkydome_.get()));
	models_.insert(std::make_pair("skydome", modelSkydomeStage1_.get()));
	models_.insert(std::make_pair("skydome2", modelSkydomeStage2_.get()));
	models_.insert(std::make_pair("ground", modelGround_.get()));
	models_.insert(std::make_pair("sphere", modelGoal_.get()));

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

			// 座標
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

			// 座標
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

			// 座標
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

			// 座標
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

void StageClearScene::UpdateChangeColor()
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

void StageClearScene::LoadStageNameSprite()
{
	//ステージごとにステージ名を変更
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/2-2.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/2-3.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(SCSTI_StageNameTex, "texture/stagename/2-4.png");
	//アンカーポイントなどをセッティング
	spriteStageName_->Initialize(spCommon_, SCSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化
	spriteStageName_->Update();
}

void StageClearScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
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
