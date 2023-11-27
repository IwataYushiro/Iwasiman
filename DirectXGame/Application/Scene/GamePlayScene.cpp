#include "GamePlayScene.h"
#include "LevelLoaderJson.h"

#include "CollisionManager.h"
#include "MeshCollider.h"
#include "TouchableObject.h"
#include "CollisionAttribute.h"
#include "EnumList.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

/*

*	GamyPlayScene.cpp

*	ゲームプレイシーン

*/

//静的メンバ変数の実体
DirectXCommon* GamePlayScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
Audio* GamePlayScene::audio_ = Audio::GetInstance();
SceneManager* GamePlayScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GamePlayScene::imguiManager_ = ImGuiManager::GetInstance();

GamePlayScene::GamePlayScene(int stagenum) :stageNum_(stagenum) {}

void GamePlayScene::Initialize()
{

	spCommon_ = SpriteCommon::GetInstance();
	colManager_ = CollisionManager::GetInstance();
	//工業地帯
	enemyFactory_ = std::make_unique<EnemyFactory>();
	gimmickFactory_ = std::make_unique<GimmickFactory>();


	// 描画初期化処理　ここから
#pragma region 描画初期化処理

	//カメラ生成
	camera_ = std::make_unique<Camera>();

	//音声データ
	sound_ = audio_->SoundLoadWave("Resources/TestMusic.wav");

	//音声再生呼び出し例
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	camera_->Reset();
	//弾リセット
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Reset();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Reset();


	//モデル読み込み
	LoadModel();
	//レベルデータ読み込み
	if (stageNum_ == SL_Stage1_Area1)LoadLVData("test");
	else if (stageNum_ == SL_Stage1_Area2)LoadLVData("stage2");
	else if (stageNum_ == SL_Stage1_Area3)LoadLVData("stage3_2");
	else if (stageNum_ == SL_Stage1_AreaBoss)LoadLVData("stageboss1");

	else if (stageNum_ == SL_StageTutorial_Area1)LoadLVData("tutorial");
	else if (stageNum_ == SL_StageTutorial_Area2)LoadLVData("tutorial2");
	else if (stageNum_ == SL_StageTutorial_Area3)LoadLVData("tutorial3");
	else if (stageNum_ == SL_StageTutorial_Final)LoadLVData("tutorialf");

	//スプライト
	LoadSprite();

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());

	//パーティクル
	particle1_ = Particle::LoadFromParticleTexture("particle8.png");
	pm_ = ParticleManager::Create();
	pm_->SetBlendMode(ParticleManager::BP_ADD);
	pm_->SetParticleModel(particle1_.get());
	pm_->SetCamera(camera_.get());

	//イージングスタンバイ
	if (stageNum_ >= SL_StageTutorial_Area1)for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].Standby(false);
	easeFadeInOut_.Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easePlayerPositionGameStart_[i].Standby(false);
	easeReadyPosition_[XXY_X1].Standby(false);

}

void GamePlayScene::Update()
{
	//死亡フラグの立った弾を削除
	playerBullets_.remove_if(
		[](std::unique_ptr<PlayerBullet>& pbullet) { return pbullet->IsDead(); });
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& ebullet) { return ebullet->IsDead(); });

	/*players_.remove_if(
		[](std::unique_ptr<Player>& player) {return player->IsDead(); });*/

	enemys_.remove_if(
		[](std::unique_ptr<BaseEnemy>& enemy) {return enemy->IsDead(); });

	//天球は常時ぐるぐる
	for (std::unique_ptr<Object3d>& skydome : skydomes_)
	{
		//天球回転用
		XMFLOAT3 rotSkydome = skydome->GetRotation();
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;

		skydome->SetRotation(rotSkydome);

		skydome->Update();
	}
	
	if (isStart_)			UpdateIsStartGame();			//ゲーム開始時
	else if (isGamePlay_)	UpdateIsPlayGame();				//ゲームプレイ時
	else if (isPause_)		UpdateIsPause();				//ポーズ時
	else if (isHowToPlay_)	UpdateHowToPlay();				//遊び方説明時
	else if (isClear_)		UpdateIsStageClear();			//ステージクリア時
	else if (isGameOver_)	UpdateIsGameOver();				//ゲームオーバー時
	else					UpdateIsQuitGame();				//終了時

	spritePause_->Update();
	spritePauseInfo_->Update();
	spritePauseResume_->Update();
	spritePauseHowToPlay_->Update();
	spritePauseStageSelect_->Update();
	spritePauseTitle_->Update();
	spriteDone_->Update();
	spriteQuitHowtoPlay_->Update();
	spriteReady_->Update();
	spriteGo_->Update();
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();
	spriteCursor_->Update();

	//チュートリアル関係
	UpdateTutorial();
	UpdateTutorialSprite();

	

}

void GamePlayScene::UpdateIsStartGame()
{
	//イージング
	easeFadeInOut_.ease_in_out_quint();

	easeEyeGameStart_[XYZ_X].ease_in_out_quint();
	easeEyeGameStart_[XYZ_Y].ease_in_out_quint();
	easeEyeGameStart_[XYZ_Z].ease_in_sine();
	easeTargetGameStart_[XYZ_X].ease_in_out_quint();
	easeTargetGameStart_[XYZ_Y].ease_in_out_quint();
	easeTargetGameStart_[XYZ_Z].ease_in_sine();


	if (isEndReady_)
	{for (int i = 0; i < XYW_Num; i++)easeGoSizeAndAlpha_[i].ease_out_cubic();
		//サイズ、カラーセット
		spriteGo_->SetSize({ easeGoSizeAndAlpha_[XYW_X].num_X,easeGoSizeAndAlpha_[XYW_Y].num_X });
		spriteGo_->SetColor({ black_.x,black_.y, black_.z,easeGoSizeAndAlpha_[XYW_W].num_X });
		
	}
	else if(isStartReadyPart2_)
	{
		easeReadyPosition_[XXY_X2].ease_in_back();
		//座標セット
		spriteReady_->SetPosition({ easeReadyPosition_[XXY_X2].num_X,easeReadyPosition_[XXY_Y].start });
		//移動が終わったら
		if (spriteReady_->GetPosition().x == easeReadyPosition_[XXY_X2].end)
		{
			for (int i = 0; i < XYW_Num; i++)easeGoSizeAndAlpha_[i].Standby(false);
			isEndReady_ = true;
			isStartReadyPart2_ = false;
		}
	}
	else//スタート時
	{
		easeReadyPosition_[XXY_X1].ease_out_circ();
		//座標セット
		spriteReady_->SetPosition({ easeReadyPosition_[XXY_X1].num_X,easeReadyPosition_[XXY_Y].start });
		//移動が終わったら
		if (spriteReady_->GetPosition().x == easeReadyPosition_[XXY_X1].end)
		{
			easeReadyPosition_[XXY_X2].Standby(false);
			isStartReadyPart2_ = true; 
		}

	}

	for (int i = 0; i < XYZ_Num; i++)easePlayerPositionGameStart_[i].ease_in_out_quint();

	//フェードインアウト
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.num_X });
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });
	
	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });

	for (std::unique_ptr<Player>& player : players_)
	{
		//パーティクル
		const ParticleManager::Preset smoke =
		{
			particle1_.get(),
			player->GetPosition(),
			{ 0.0f ,3.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			2,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{0.0f,0.0f,0.0f,1.0f}
		};
		pm_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		player->SetPosition
		({
			easePlayerPositionGameStart_[XYZ_X].num_X,
			easePlayerPositionGameStart_[XYZ_Y].num_X,
			easePlayerPositionGameStart_[XYZ_Z].num_X
			});

		player->Update(false, false, true);

		if (player->GetPosition().x == easePlayerPositionGameStart_[XYZ_X].end)
		{
			isStart_ = false;
			isGamePlay_ = true;
		}
	}

	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<BaseEnemy>& enemy : enemys_)enemy->Update(true);
	//弾更新
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();

	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();

	for (std::unique_ptr<Goal>& goal : goals_)goal->Update();

	for (std::unique_ptr<Item>& item : items_)item->Update();

	for (std::unique_ptr<Object3d>& object : objects_) object->Update();
	//カメラ
	camera_->Update();
	lightGroup_->Update();
	pm_->Update();
}

void GamePlayScene::UpdateIsPlayGame()
{

	//モデル呼び出し例
	for (std::unique_ptr<Player>& player : players_)
	{

		//チュートリアル基本操作
		if (stageNum_ == SL_StageTutorial_Area1)player->Update(false, false, false);
		//チュートリアル奥側移動→攻撃→応用ステージ
		else if (stageNum_ == SL_StageTutorial_Area2)player->Update(true, false, false);
		//基本状態
		else player->Update();


		lightGroup_->SetPointLightPos(0, player->GetWorldPosition());
		//かめおべら
		if (player->IsDead())
		{

			isGameOver_ = true;
			isGamePlay_ = false;
		}
		//ImGui	
		imguiManager_->Begin();
		imguiManager_->End();
	}
	//弾更新
	for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets_) playerBullet->Update();

	for (std::unique_ptr<BaseEnemy>& enemy : enemys_)
	{
		enemy->Update();
		//ボス撃破
		if (enemy->BossDead())
		{
			isClear_ = true;
			isGamePlay_ = false;
		}
	}
	//弾更新
	for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();

	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();

	for (std::unique_ptr<Goal>& goal : goals_)
	{
		goal->Update();
		//クリア
		if (goal->IsGoal())
		{
			isClear_ = true;
			isGamePlay_ = false;
		}

	}
	for (std::unique_ptr<Item>& item : items_)
	{
		item->Update();
	}
	for (std::unique_ptr<Object3d>& object : objects_) object->Update();

	//imguiManager_->Begin();
	//camera_->DebugCamera();
	//imguiManager_->End();

	//カメラ
	camera_->Update();
	lightGroup_->Update();
	pm_->Update();
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())colManager_->CheckAllCollisions();
	//Pause機能
	if (input_->TriggerKey(DIK_Q))
	{
		for (std::unique_ptr<Player>& player : players_)if (player->IsBreak())return;
		//ここでイージングの準備
		for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(false);
		easeCursorPosX_.Standby(false);
		isBack_ = false;
		isPause_ = true;
		isGamePlay_ = false;
	}
}

void GamePlayScene::UpdateIsPause()
{
	//メニューの上限
	if (menuCount_ <= GPSPMI_Resume)menuCount_ = GPSPMI_Resume;
	else if (menuCount_ >= GPSPMI_Title)menuCount_ = GPSPMI_Title;
	//選択中のメニューカラー
	const DirectX::XMFLOAT4 selectMenuColor = { 0.5f + infoColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 1.0f,1.0f,1.0f,0.5f };
	//ポーズカラー
	const DirectX::XMFLOAT4 pauseColor = { 0.1f ,0.1f,0.5f + infoColor_.z,1.0f };
	//決定指示スプライトのカラー
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.5f + infoColor_.x };
	//カラー更新
	UpdateChangeColor();

	//イージング(ポーズ中に準備してもここがやってくれる)
	FadeOut(black_);
	for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();

	//ポジションセット
	spritePause_->SetPosition({ easePauseMenuPosX_[PMEN_Menu].num_X, pausePosY_[PMEN_Menu] });
	spritePauseResume_->SetPosition({ easePauseMenuPosX_[PMEN_Resume].num_X, pausePosY_[PMEN_Resume] });
	spritePauseHowToPlay_->SetPosition({ easePauseMenuPosX_[PMEN_HowToPlay].num_X, pausePosY_[PMEN_HowToPlay] });
	spritePauseStageSelect_->SetPosition({ easePauseMenuPosX_[PMEN_StageSelect].num_X, pausePosY_[PMEN_StageSelect] });
	spritePauseTitle_->SetPosition({ easePauseMenuPosX_[PMEN_Title].num_X, pausePosY_[PMEN_Title] });
	spriteDone_->SetPosition({ easePauseMenuPosX_[PMEN_SelectSpace].num_X, pausePosY_[PMEN_SelectSpace] });
	spriteTutorialInfoHowToPlay_->SetPosition({ easePauseMenuPosX_[PMEN_TutorialHowToPlayInfo].num_X, pausePosY_[PMEN_TutorialHowToPlayInfo] });

	spriteTutorialHTPMove_->SetPosition({ easeHowToPlayPosX_[HTPEN_Move].num_X,howToPlayPosY_[HTPEN_Move] });
	spriteTutorialHTPDash_->SetPosition({ easeHowToPlayPosX_[HTPEN_Dash].num_X,howToPlayPosY_[HTPEN_Dash] });
	spriteTutorialHTPJump_->SetPosition({ easeHowToPlayPosX_[HTPEN_Jump].num_X,howToPlayPosY_[HTPEN_Jump] });
	spriteTutorialHTPMoveBack_->SetPosition({ easeHowToPlayPosX_[HTPEN_MoveBack].num_X,howToPlayPosY_[HTPEN_MoveBack] });
	spriteTutorialHTPAttack_->SetPosition({ easeHowToPlayPosX_[HTPEN_Attack].num_X,howToPlayPosY_[HTPEN_Attack] });
	spriteQuitHowtoPlay_->SetPosition({ easeHowToPlayPosX_[HTPEN_Quit].num_X,howToPlayPosY_[HTPEN_Quit] });

	//メニュー操作
	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S))menuCount_++;

	if (menuCount_ == GPSPMI_Resume)
	{
		spritePauseResume_->SetColor(selectMenuColor);
		spritePauseHowToPlay_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Resume] });
	}
	else if (menuCount_ == GPSPMI_HowToPlay)
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHowToPlay_->SetColor(selectMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_HowToPlay] });
	}
	else if (menuCount_ == GPSPMI_StageSelect)
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHowToPlay_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(selectMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_StageSelect] });
	}
	else if (menuCount_ == GPSPMI_Title)
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHowToPlay_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Title] });
	}
	
	//デフォルトカラー
	spritePause_->SetColor(pauseColor);
	spriteDone_->SetColor(doneColor);

	if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			if (menuCount_ == GPSPMI_Resume)
			{
				//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;

			}
			else if (menuCount_ == GPSPMI_HowToPlay)
			{
				if (stageNum_ < SL_StageTutorial_Area1)//チュートリアルステージだと何も起こらない
				{
					//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
					if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
					{
						for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
						for (int i = 0; i < HTPEN_Num; i++)easeHowToPlayPosX_[i].Standby(false);
						easeCursorPosX_.Standby(true);
					}

					isHowToPlay_ = true;
					isPause_ = false;
				}
			}
			else if (menuCount_ == GPSPMI_StageSelect)
			{
				//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;
			}
			else if (menuCount_ == GPSPMI_Title)
			{
				//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;
			}


		}
	}

	if (isBack_)
	{
		if (menuCount_ == GPSPMI_Resume) FadeIn(black_);
		else if (menuCount_ == GPSPMI_StageSelect) FadeIn(black_);
		else if (menuCount_ == GPSPMI_Title) FadeIn(black_);

	}
	//到達したらPause解除
	if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].start)
	{
		if (isBack_)
		{
			if (menuCount_ == GPSPMI_Resume)
			{
				isGamePlay_ = true;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isBack_ = false;
				isPause_ = false;
			}
			else if (menuCount_ == GPSPMI_StageSelect)
			{
				isQuit_ = true;
				isBack_ = false;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isPause_ = false;
			}
			else if (menuCount_ == GPSPMI_Title)
			{
				isQuit_ = true;
				isBack_ = false;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isPause_ = false;
			}

		}

	}

}

void GamePlayScene::UpdateHowToPlay()
{
	//イージング
	for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < HTPEN_Num; i++)easeHowToPlayPosX_[i].ease_out_expo();
	easeCursorPosX_.ease_out_expo();

	//ポジションセット
	spritePause_->SetPosition({ easePauseMenuPosX_[PMEN_Menu].num_X, pausePosY_[PMEN_Menu] });
	spritePauseResume_->SetPosition({ easePauseMenuPosX_[PMEN_Resume].num_X, pausePosY_[PMEN_Resume] });
	spritePauseHowToPlay_->SetPosition({ easePauseMenuPosX_[PMEN_HowToPlay].num_X, pausePosY_[PMEN_HowToPlay] });
	spritePauseStageSelect_->SetPosition({ easePauseMenuPosX_[PMEN_StageSelect].num_X, pausePosY_[PMEN_StageSelect] });
	spritePauseTitle_->SetPosition({ easePauseMenuPosX_[PMEN_Title].num_X, pausePosY_[PMEN_Title] });
	spriteDone_->SetPosition({ easePauseMenuPosX_[PMEN_SelectSpace].num_X, pausePosY_[PMEN_SelectSpace] });
	spriteCursor_->SetPositionX(easeCursorPosX_.num_X);

	spriteTutorialHTPMove_->SetPosition({ easeHowToPlayPosX_[HTPEN_Move].num_X,howToPlayPosY_[HTPEN_Move] });
	spriteTutorialHTPDash_->SetPosition({ easeHowToPlayPosX_[HTPEN_Dash].num_X,howToPlayPosY_[HTPEN_Dash] });
	spriteTutorialHTPJump_->SetPosition({ easeHowToPlayPosX_[HTPEN_Jump].num_X,howToPlayPosY_[HTPEN_Jump] });
	spriteTutorialHTPMoveBack_->SetPosition({ easeHowToPlayPosX_[HTPEN_MoveBack].num_X,howToPlayPosY_[HTPEN_MoveBack] });
	spriteTutorialHTPAttack_->SetPosition({ easeHowToPlayPosX_[HTPEN_Attack].num_X,howToPlayPosY_[HTPEN_Attack] });
	spriteQuitHowtoPlay_->SetPosition({ easeHowToPlayPosX_[HTPEN_Quit].num_X,howToPlayPosY_[HTPEN_Quit] });
	//到達するまで遊び方説明画面解除不可
	if (spriteDone_->GetPosition().x == easePauseMenuPosX_[HTPEN_Quit].start)
	{
		//到達した後スペースで戻る
		if (input_->TriggerKey(DIK_SPACE))
		{
			for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(false);
			for (int i = 0; i < HTPEN_Num; i++)easeHowToPlayPosX_[i].Standby(true);
			easeCursorPosX_.Standby(false);
			isBackPause_ = true;
		}

	}
	if (isBackPause_)
	{
		for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].ease_in_out_quint();
		for (int i = 0; i < HTPEN_Num; i++)easeHowToPlayPosX_[i].ease_in_out_quint();
		easeCursorPosX_.ease_in_out_quint();
		if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
		{
			isPause_ = true;
			isBackPause_ = false;
			isHowToPlay_ = false;
		}
	}
}

void GamePlayScene::UpdateIsStageClear()
{
	for (std::unique_ptr<Player>& player : players_)player->Update();

	FadeOut(white_);
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		sceneManager_->ChangeScene("STAGECLEAR", stageNum_);
		
	}
}

void GamePlayScene::UpdateIsGameOver()
{
	FadeOut(deepRed_);
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		sceneManager_->ChangeScene("GAMEOVER", stageNum_);
		
	}
}

void GamePlayScene::UpdateIsQuitGame()
{
	FadeOut(black_);
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//ポーズからステージセレクト、タイトルを選んだとき
		if (menuCount_ == GPSPMI_StageSelect)
		{
			//ステージ選択
			if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
			else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
			else sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);
		}
		else if (menuCount_ == GPSPMI_Title) sceneManager_->ChangeScene("TITLE", stageNum_);
	}


}

void GamePlayScene::UpdateTutorial()
{
	if (stageNum_ == SL_StageTutorial_Area1)
	{
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			nullptr, nullptr, spriteTutorialInfo1_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area2)
	{
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), nullptr, spriteTutorialInfo2_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area3)
	{
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo3_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo4_.get());
	}
}

void GamePlayScene::FadeOut(DirectX::XMFLOAT3 rgb)
{

	if (isPause_)//ポーズ時の場合
	{
		if (!isFadeOutPause_)
		{
			easeFadeInOutPause_.Standby(true);
			isFadeOutPause_ = true;
		}
		else
		{
			easeFadeInOutPause_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOutPause_.num_X });//透明度だけ変える

		}
	}
	else//シーン遷移時の場合
	{
		if (!isFadeOutScene_)
		{
			easeFadeInOut_.Standby(true);
			isFadeOutScene_ = true;
		}
		else
		{
			const XMFLOAT3 negapozi = { 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z };
			easeFadeInOut_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//透明度だけ変える
			if(isQuit_)spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ネガポジの応用
		}
	}
}

void GamePlayScene::FadeIn(DirectX::XMFLOAT3 rgb)
{

	if (isPause_)//ポーズ時の場合にしか使わない
	{
		if (!isFadeInPause_)
		{
			easeFadeInOutPause_.Standby(false);
			isFadeInPause_ = true;
		}
		else
		{
			easeFadeInOutPause_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOutPause_.num_X });//透明度だけ変える

		}
	}
}

void GamePlayScene::Draw()
{
	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	for (std::unique_ptr<Player>& player : players_)player->Draw();
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Draw();
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->Draw();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();
	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_) gimmick->Draw();
	for (std::unique_ptr<Goal>& goal : goals_)goal->Draw();
	for (std::unique_ptr<Item>& item : items_)item->Draw();
	for (auto& skydome : skydomes_)skydome->Draw();
	for (auto& object : objects_)object->Draw();

	//モデル描画後処理
	Object3d::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	pm_->Draw();
	for (std::unique_ptr<Player>& player : players_)player->DrawParticle();
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->DrawParticle();
	for (std::unique_ptr<Item>& item : items_)item->DrawParticle();
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//スプライト描画前処理
	spCommon_->PreDraw();
	//前景スプライト
	if (isPause_)//ポーズ時
	{
		//フェードインアウト
		spriteFadeInOut_->Draw();

		spritePause_->Draw();
		spritePauseResume_->Draw();
		spritePauseHowToPlay_->Draw();
		spritePauseStageSelect_->Draw();
		spritePauseTitle_->Draw();
		spriteDone_->Draw();
		spriteCursor_->Draw();
		if (stageNum_ >= SL_StageTutorial_Area1)//チュートリアルステージ以外は書かない
		{
			if (menuCount_ == GPSPMI_HowToPlay)spriteTutorialInfoHowToPlay_->Draw();
		}

	}
	else if (isHowToPlay_)//遊び方説明時
	{
		//フェードインアウト
		spriteFadeInOut_->Draw();

		spritePause_->Draw();
		spritePauseResume_->Draw();
		spritePauseHowToPlay_->Draw();
		spritePauseStageSelect_->Draw();
		spritePauseTitle_->Draw();
		spriteDone_->Draw();
		spriteCursor_->Draw();

		spriteTutorialHTPMove_->Draw();
		spriteTutorialHTPDash_->Draw();
		spriteTutorialHTPJump_->Draw();
		spriteTutorialHTPMoveBack_->Draw();
		spriteTutorialHTPAttack_->Draw();
		spriteQuitHowtoPlay_->Draw();
	}
	else
	{
for (std::unique_ptr<Item>& item : items_){item->DrawSprite();}
		

		for (std::unique_ptr<Player>& player : players_) { player->DrawSprite(); 
		
		if (!player->IsBreak())
		{
			if (isGamePlay_)spritePauseInfo_->Draw();
			
			if (stageNum_ == SL_StageTutorial_Area1)
			{
				DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
					nullptr, nullptr, spriteTutorialInfo1_.get());
			}
			else if (stageNum_ == SL_StageTutorial_Area2)
			{
				DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
					spriteTutorialHTPMoveBack_.get(), nullptr, spriteTutorialInfo2_.get());
			}
			else if (stageNum_ == SL_StageTutorial_Area3)
			{
				DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
					spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo3_.get());
			}
			else if (stageNum_ == SL_StageTutorial_Final)
			{
				DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
					spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo4_.get());
			}
		}
}
		//フェードインアウトとロードと現ステージ
		spriteFadeInOut_->Draw();
		spriteLoad_->Draw();
		spriteStageInfoNow_->Draw();
		//レディーゴー
		spriteReady_->Draw();
		spriteGo_->Draw();
	}


}

void GamePlayScene::Finalize()
{
	//終了処理
	audio_->Finalize();
	//解放
	//各種音声
	audio_->SoundUnLoad(&sound_);

	objects_.clear();
	skydomes_.clear();

	models_.clear();

}

void GamePlayScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData_ = LevelLoader::LoadFile(stagePath);
	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData_->objects) {

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}
		//プレイヤー
		if (objectData.objectType.find("PLAYER") == LDTOF_TRUE)
		{
			//プレイヤー初期化
			std::unique_ptr<Player> newplayer;
			//イージング用のポジション
			DirectX::XMFLOAT3 endEasePlayerPosition;
			//開始時ポジションを決めるオフセット
			const DirectX::XMFLOAT3 offsetPlayerPosition = { -100.0f,100.0f,60.0f };
			//イージングの時間
			const float maxTime = 4.0f;

			//生成
			newplayer = Player::Create(model, modelPlayerBullet_.get(), this);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newplayer->SetPosition(pos);
			endEasePlayerPosition = newplayer->GetPosition();//最終イージングポジションセット
			startEasePlayerPosition_ = endEasePlayerPosition + offsetPlayerPosition;
			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newplayer->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newplayer->SetScale(scale);
			//ここで開始時プレイヤー演出セット
			easePlayerPositionGameStart_[XYZ_X].SetEasing(startEasePlayerPosition_.x, endEasePlayerPosition.x, maxTime);
			easePlayerPositionGameStart_[XYZ_Y].SetEasing(startEasePlayerPosition_.y, endEasePlayerPosition.y, maxTime);
			easePlayerPositionGameStart_[XYZ_Z].SetEasing(startEasePlayerPosition_.z, endEasePlayerPosition.z, maxTime);

			newplayer->SetCamera(camera_.get());
			newplayer->Update();
			//リストに登録
			players_.push_back(std::move(newplayer));
		}
		//敵
		else if (objectData.objectType.find("ENEMY") == LDTOF_TRUE)
		{
			//敵初期化
			std::unique_ptr<BaseEnemy> newenemy;
			std::unique_ptr<Player>& player = players_.front();

			newenemy = enemyFactory_->CreateEnemy(objectData.objectPattern,
				model, modelEnemyBullet_.get(), player.get(), this);

			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newenemy->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newenemy->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newenemy->SetScale(scale);

			newenemy->SetCamera(camera_.get());
			newenemy->Update();
			//リストに登録
			enemys_.push_back(std::move(newenemy));

		}
		//仕掛け
		else if (objectData.objectType.find("GIMMICK") == LDTOF_TRUE)
		{
			//敵初期化
			std::unique_ptr<BaseGimmick> newGimmick;
			std::unique_ptr<Player>& player = players_.front();

			newGimmick = gimmickFactory_->CreateGimmick(objectData.objectPattern, model, player.get());

			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newGimmick->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newGimmick->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newGimmick->SetScale(scale);

			newGimmick->SetCamera(camera_.get());
			newGimmick->Update();
			//リストに登録
			gimmicks_.push_back(std::move(newGimmick));
		}
		//ゴール
		else if (objectData.objectType.find("GOAL") == LDTOF_TRUE)
		{
			//ゴール初期化
			std::unique_ptr<Goal> newgoal;
			newgoal = Goal::Create(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newgoal->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newgoal->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newgoal->SetScale(scale);

			newgoal->SetCamera(camera_.get());
			newgoal->Update();
			//リストに登録
			goals_.push_back(std::move(newgoal));
		}
		//アイテム
		else if (objectData.objectType.find("ITEM") == LDTOF_TRUE)
		{
			//アイテム初期化
			std::unique_ptr<Item> newitem;
			std::unique_ptr<Player>& player = players_.front();
			//ジャンプ
			if (objectData.objectPattern.find("JUMP") == LDTOF_TRUE)
				newitem = Item::Create(modelItemJump_.get(), player.get(), SUBCOLLISION_ATTR_ITEM_JUMP);
			//回復アイテム
			else if (objectData.objectPattern.find("HEAL") == LDTOF_TRUE)
				newitem = Item::Create(modelItemHeal_.get(), player.get(), SUBCOLLISION_ATTR_ITEM_HEAL);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newitem->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newitem->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newitem->SetScale(scale);

			newitem->SetCamera(camera_.get());
			newitem->Update();
			//リストに登録
			items_.push_back(std::move(newitem));
		}
		//当たり判定がないオブジェクト
		else if (objectData.objectType.find("NONE") == LDTOF_TRUE)
		{
			// モデルを指定して3Dオブジェクトを生成
			std::unique_ptr<Object3d> newObject = Object3d::Create();
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

			newObject->SetCamera(camera_.get());


			// 配列に登録
			objects_.push_back(std::move(newObject));
		}
		//天球
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

			newObject->SetCamera(camera_.get());
			// 配列に登録
			skydomes_.push_back(std::move(newObject));
		}
		//地形
		else
		{
			// モデルを指定して3Dオブジェクトを生成
			std::unique_ptr<TouchableObject> newObject = TouchableObject::Create(model, true);
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

			newObject->SetCamera(camera_.get());


			// 配列に登録
			objects_.push_back(std::move(newObject));
		}

	}

}

void GamePlayScene::UpdateChangeColor()
{
	//色を変えるスピード
	const float speedColor = 0.02f;

	if (isColorReverse_)
	{
		infoColor_.x -= speedColor;
		infoColor_.y -= speedColor;
		infoColor_.z -= speedColor;
	}

	else
	{
		infoColor_.x += speedColor;
		infoColor_.y += speedColor;
		infoColor_.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.5f,0.0f };//{max,min}

	if (infoColor_.x >= maxAndMinSpeedColor.x)
	{
		isColorReverse_ = true;
	}
	if (infoColor_.x <= maxAndMinSpeedColor.y)
	{
		isColorReverse_ = false;
	}
}

void GamePlayScene::AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet)
{
	//リストに登録
	playerBullets_.push_back(std::move(playerBullet));
}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	//リストに登録
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GamePlayScene::LoadModel()
{
	// モデル読み込み
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelPlayerBullet_ = Model::LoadFromOBJ("playerbullet");
	modelEnemy1_ = Model::LoadFromOBJ("enemy1");
	modelEnemyBullet_ = Model::LoadFromOBJ("enemybullet");
	modelBoss1_ = Model::LoadFromOBJ("boss1");
	modelBossCore1_ = Model::LoadFromOBJ("core1");
	modelGoal_ = Model::LoadFromOBJ("sphere");
	modelItemJump_ = Model::LoadFromOBJ("itemjump");
	modelItemHeal_ = Model::LoadFromOBJ("itemheal");
	modelSpike_ = Model::LoadFromOBJ("spikeball");
	modelStageT_ = Model::LoadFromOBJ("skydomet");
	modelStage1_ = Model::LoadFromOBJ("skydome");
	modelStage2_ = Model::LoadFromOBJ("skydome2");
	modelGround_ = Model::LoadFromOBJ("ground");
	modelSphere_ = Model::LoadFromOBJ("sphere2");
	modelBox_ = Model::LoadFromOBJ("ground2");

	models_.insert(std::make_pair("player", modelPlayer_.get()));
	models_.insert(std::make_pair("playerbullet", modelPlayerBullet_.get()));
	models_.insert(std::make_pair("enemy1", modelEnemy1_.get()));
	models_.insert(std::make_pair("enemybullet", modelEnemyBullet_.get()));
	models_.insert(std::make_pair("boss1", modelBoss1_.get()));
	models_.insert(std::make_pair("core1", modelBossCore1_.get()));
	models_.insert(std::make_pair("sphere", modelGoal_.get()));
	models_.insert(std::make_pair("Itemjump", modelItemJump_.get()));
	models_.insert(std::make_pair("itemheal", modelItemHeal_.get()));
	models_.insert(std::make_pair("spikeball", modelSpike_.get()));
	models_.insert(std::make_pair("skydomet", modelStageT_.get()));
	models_.insert(std::make_pair("skydome", modelStage1_.get()));
	models_.insert(std::make_pair("skydome2", modelStage2_.get()));
	models_.insert(std::make_pair("ground", modelGround_.get()));
	models_.insert(std::make_pair("sphere2", modelSphere_.get()));
	models_.insert(std::make_pair("ground2", modelBox_.get()));

}

void GamePlayScene::SettingTutorialEase(int num, Sprite* s1, Sprite* s2,
	Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)
{
	switch (num)
	{
	case GPSSTEN_Stundby:
		if (s1 != nullptr)s1->SetPosition({ easeInfoTutorial_[TIEN_Move].start,tutorialInfoPosY_[TIEN_Move] });
		if (s2 != nullptr)s2->SetPosition({ easeInfoTutorial_[TIEN_Dash].start,tutorialInfoPosY_[TIEN_Dash] });
		if (s3 != nullptr)s3->SetPosition({ easeInfoTutorial_[TIEN_Jump].start,tutorialInfoPosY_[TIEN_Jump] });
		if (s4 != nullptr)s4->SetPosition({ easeInfoTutorial_[TIEN_MoveBack].start,tutorialInfoPosY_[TIEN_MoveBack] });
		if (s5 != nullptr)s5->SetPosition({ easeInfoTutorial_[TIEN_Attack].start,tutorialInfoPosY_[TIEN_Attack] });
		if (s6 != nullptr)s6->SetPosition({ easeInfoTutorial_[TIEN_Info].start,tutorialInfoPosY_[TIEN_Info] });
		break;
	case GPSSTEN_Active:
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		if (s1 != nullptr)s1->SetPosition({ easeInfoTutorial_[TIEN_Move].num_X,tutorialInfoPosY_[TIEN_Move] });
		if (s2 != nullptr)s2->SetPosition({ easeInfoTutorial_[TIEN_Dash].num_X,tutorialInfoPosY_[TIEN_Dash] });
		if (s3 != nullptr)s3->SetPosition({ easeInfoTutorial_[TIEN_Jump].num_X,tutorialInfoPosY_[TIEN_Jump] });
		if (s4 != nullptr)s4->SetPosition({ easeInfoTutorial_[TIEN_MoveBack].num_X,tutorialInfoPosY_[TIEN_MoveBack] });
		if (s5 != nullptr)s5->SetPosition({ easeInfoTutorial_[TIEN_Attack].num_X,tutorialInfoPosY_[TIEN_Attack] });
		if (s6 != nullptr)s6->SetPosition({ easeInfoTutorial_[TIEN_Info].num_X,tutorialInfoPosY_[TIEN_Info] });
		break;
	}

}

void GamePlayScene::UpdateTutorialSprite()
{
	if (stageNum_ >= SL_StageTutorial_Area1)//チュートリアルステージだと何も起こらない
	{
		//チュートリアル説明カラー
		const DirectX::XMFLOAT4 infoColorRed = { 0.1f + infoColor_.x,0.0f,0.0f,1.0f };
		const DirectX::XMFLOAT4 infoColorBlue = { 0.0f,0.0f,0.1f + infoColor_.z,1.0f };

		//操作説明文字カラー
		const DirectX::XMFLOAT4 howToPlayColorActiveRed = { 1.0f,0.1f,0.1f,1.0f };
		const DirectX::XMFLOAT4 howToPlayColorActiveBlue = { 0.1f,0.1f,1.0f,1.0f };
		const DirectX::XMFLOAT4 howToPlayColorDefault = { 0.01f,0.01f,0.01f,1.0f };

		UpdateChangeColor();

		spriteTutorialHTPDash_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPMove_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPJump_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPMoveBack_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPAttack_->SetColor(howToPlayColorDefault);

		//ダッシュ
		if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
		{
			if (input_->PushKey(DIK_A))spriteTutorialHTPDash_->SetColor(howToPlayColorActiveBlue);
			if (input_->PushKey(DIK_D))spriteTutorialHTPDash_->SetColor(howToPlayColorActiveRed);
		}
		else
		{
			if (input_->PushKey(DIK_A))spriteTutorialHTPMove_->SetColor(howToPlayColorActiveBlue);
			if (input_->PushKey(DIK_D))spriteTutorialHTPMove_->SetColor(howToPlayColorActiveRed);
		}
		//ジャンプ
		if (input_->PushKey(DIK_SPACE))spriteTutorialHTPJump_->SetColor(howToPlayColorActiveRed);
		//奥側ジャンプ
		if (input_->PushKey(DIK_W))spriteTutorialHTPMoveBack_->SetColor(howToPlayColorActiveRed);
		if (input_->PushKey(DIK_S))spriteTutorialHTPMoveBack_->SetColor(howToPlayColorActiveBlue);
		//攻撃
		if (input_->PushKey(DIK_L))spriteTutorialHTPAttack_->SetColor(howToPlayColorActiveRed);

		spriteTutorialInfo1_->SetColor(infoColorRed);
		spriteTutorialInfo2_->SetColor(infoColorRed);
		spriteTutorialInfo3_->SetColor(infoColorRed);
		spriteTutorialInfo4_->SetColor(infoColorBlue);
	}
	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();
	spriteTutorialInfoHowToPlay_->Update();

	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();
}

void GamePlayScene::DrawTutorialSprite(Sprite* s1, Sprite* s2, Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)
{
	if (s1 != nullptr)s1->Draw();
	if (s2 != nullptr)s2->Draw();
	if (s3 != nullptr)s3->Draw();
	if (s4 != nullptr)s4->Draw();
	if (s5 != nullptr)s5->Draw();
	if (s6 != nullptr)s6->Draw();
}

void GamePlayScene::LoadSprite()
{
	//スプライト
	spCommon_->LoadTexture(GPSTI_PauseInfoTex, "texture/pauseinfo.png");
	spritePauseInfo_->Initialize(spCommon_, GPSTI_PauseInfoTex);

	//ポーズ時
	spCommon_->LoadTexture(GPSTI_PauseTex, "texture/pause.png");
	spritePause_->Initialize(spCommon_, GPSTI_PauseTex);
	spritePause_->SetPosition({ easePauseMenuPosX_[PMEN_Menu].start,pausePosY_[PMEN_Menu] });

	spCommon_->LoadTexture(GPSTI_PauseResumeTex, "texture/resume.png");
	spritePauseResume_->Initialize(spCommon_, GPSTI_PauseResumeTex);
	spritePauseResume_->SetPosition({ easePauseMenuPosX_[PMEN_Resume].start,pausePosY_[PMEN_Resume] });

	spCommon_->LoadTexture(GPSTI_PauseHowToPlayTex, "texture/howtoplay2.png");
	spritePauseHowToPlay_->Initialize(spCommon_, GPSTI_PauseHowToPlayTex);
	spritePauseHowToPlay_->SetPosition({ easePauseMenuPosX_[PMEN_HowToPlay].start,pausePosY_[PMEN_HowToPlay] });

	spCommon_->LoadTexture(GPSTI_PauseStageSelectTex, "texture/backstageselect.png");
	spritePauseStageSelect_->Initialize(spCommon_, GPSTI_PauseStageSelectTex);
	spritePauseStageSelect_->SetPosition({ easePauseMenuPosX_[PMEN_StageSelect].start,pausePosY_[PMEN_StageSelect] });

	spCommon_->LoadTexture(GPSTI_PauseTitleTex, "texture/backtitle.png");
	spritePauseTitle_->Initialize(spCommon_, GPSTI_PauseTitleTex);
	spritePauseTitle_->SetPosition({ easePauseMenuPosX_[PMEN_Title].start,pausePosY_[PMEN_Title] });

	spCommon_->LoadTexture(GPSTI_PauseDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, GPSTI_PauseDoneTex);
	spriteDone_->SetPosition({ easePauseMenuPosX_[PMEN_SelectSpace].start,pausePosY_[PMEN_SelectSpace] });

	spCommon_->LoadTexture(GPSTI_QuitHowToPlayTex, "texture/space.png");
	spriteQuitHowtoPlay_->Initialize(spCommon_, GPSTI_QuitHowToPlayTex);
	spriteQuitHowtoPlay_->SetPosition({ easeHowToPlayPosX_[HTPEN_Quit].start,howToPlayPosY_[HTPEN_Quit] });

	spCommon_->LoadTexture(GPSTI_ReadyTex, "texture/ready2.png");
	spriteReady_->Initialize(spCommon_, GPSTI_ReadyTex);
	spriteReady_->SetPosition({ easeReadyPosition_[XXY_X1].start,easeReadyPosition_[XXY_Y].start });
	spriteReady_->SetColor({ black_.x,black_.y,black_.z,easeFadeInOut_.start });//真っ黒

	spCommon_->LoadTexture(GPSTI_GoTex, "texture/go.png");
	spriteGo_->Initialize(spCommon_, GPSTI_GoTex);
	spriteGo_->SetSize({ easeGoSizeAndAlpha_[XYW_X].start,easeGoSizeAndAlpha_[XYW_Y].start });
	spriteGo_->SetPosition({ goPosition_[XY_X],goPosition_[XY_Y] });

	const XMFLOAT2 centralAnchorPoint = { 0.5f,0.5f };//アンカーポイントは中央
	spriteGo_->SetAnchorPoint(centralAnchorPoint);
	spriteGo_->SetColor({ black_.x,black_.y,black_.z,easeGoSizeAndAlpha_[XYW_W].start });

	spCommon_->LoadTexture(GPSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, GPSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });//真っ白

	spCommon_->LoadTexture(GPSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, GPSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	const int32_t remainderNum = stageNum_ % 10;//余りによってスプライトを変える
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage1.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage4) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stagef.png");

	spriteStageInfoNow_->Initialize(spCommon_, GPSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	spCommon_->LoadTexture(GPSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, GPSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,pausePosY_[PMEN_Resume] });


	spCommon_->LoadTexture(GPSTTI_TutorialInfo1Tex, "texture/info/tinfo1.png");//1
	spriteTutorialInfo1_->Initialize(spCommon_, GPSTTI_TutorialInfo1Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo2Tex, "texture/info/tinfo2.png");//2
	spriteTutorialInfo2_->Initialize(spCommon_, GPSTTI_TutorialInfo2Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo3Tex, "texture/info/tinfo3.png");//3
	spriteTutorialInfo3_->Initialize(spCommon_, GPSTTI_TutorialInfo3Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo4Tex, "texture/info/tinfo4.png");//4
	spriteTutorialInfo4_->Initialize(spCommon_, GPSTTI_TutorialInfo4Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfoHowToPlayTex, "texture/info/tinfohowtoplay.png");//遊び方説明について
	spriteTutorialInfoHowToPlay_->Initialize(spCommon_, GPSTTI_TutorialInfoHowToPlayTex);
	spriteTutorialInfoHowToPlay_->SetPosition({ easePauseMenuPosX_[PMEN_TutorialHowToPlayInfo].start,pausePosY_[PMEN_TutorialHowToPlayInfo]});

	spCommon_->LoadTexture(GPSTTI_HowToPlayMoveTex, "texture/info/moveinfo.png");//1~
	spriteTutorialHTPMove_->Initialize(spCommon_, GPSTTI_HowToPlayMoveTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayDashTex, "texture/info/dashinfo.png");//1~
	spriteTutorialHTPDash_->Initialize(spCommon_, GPSTTI_HowToPlayDashTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayJumpTex, "texture/info/jumpinfo.png");//1~
	spriteTutorialHTPJump_->Initialize(spCommon_, GPSTTI_HowToPlayJumpTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayBackMoveTex, "texture/info/backmoveinfo.png");//2~
	spriteTutorialHTPMoveBack_->Initialize(spCommon_, GPSTTI_HowToPlayBackMoveTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayAttackTex, "texture/info/attackinfo2.png");//3~
	spriteTutorialHTPAttack_->Initialize(spCommon_, GPSTTI_HowToPlayAttackTex);

	if (stageNum_ == SL_StageTutorial_Area1)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			nullptr, nullptr, spriteTutorialInfo1_.get());
	}

	else if (stageNum_ == SL_StageTutorial_Area2)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), nullptr, spriteTutorialInfo2_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area3)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo3_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo4_.get());
	}
	else//チュートリアルステージ以外の場合
	{
		//カラーセット
		const DirectX::XMFLOAT4 howToPlayColor = { 1.0f,0.15f,0.15f,1.0f };

		spriteTutorialHTPDash_->SetColor(howToPlayColor);
		spriteTutorialHTPMove_->SetColor(howToPlayColor);
		spriteTutorialHTPJump_->SetColor(howToPlayColor);
		spriteTutorialHTPMoveBack_->SetColor(howToPlayColor);
		spriteTutorialHTPAttack_->SetColor(howToPlayColor);
	}

	spritePause_->Update();
	spritePauseInfo_->Update();
	spritePauseResume_->Update();
	spritePauseHowToPlay_->Update();
	spritePauseStageSelect_->Update();
	spritePauseTitle_->Update();
	spriteDone_->Update();
	spriteQuitHowtoPlay_->Update();
	spriteReady_->Update();
	spriteGo_->Update();
	spriteFadeInOut_->Update();

	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();
	spriteTutorialInfoHowToPlay_->Update();

	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();


}
