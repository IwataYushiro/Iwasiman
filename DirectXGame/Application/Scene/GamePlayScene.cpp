#include "GamePlayScene.h"
#include "LevelLoaderJson.h"

#include "CollisionManager.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"
#include "EnumList.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;
using namespace IwasiEngine;
/*

*	GamyPlayScene.cpp

*	ゲームプレイシーン

*/

GamePlayScene::GamePlayScene(int stagenum) :stageNum_(stagenum) {}

void GamePlayScene::Initialize()
{
	//インスタンス取得
	dxCommon_ = DirectXCommon::GetInstance();		//DirectX基盤
	spCommon_ = SpriteCommon::GetInstance();		//スプライト基盤
	input_ = Input::GetInstance();					//入力情報
	audio_ = Audio::GetInstance();					//音声
	sceneManager_ = SceneManager::GetInstance();	//シーンマネージャー
	imguiManager_ = ImGuiManager::GetInstance();	//ImGuiマネージャー
	colManager_ = CollisionManager::GetInstance();	//コリジョンマネージャー

	//ファクトリー生成
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
	if (stageNum_ == SL_Stage1_Area1)LoadLVData("stage1");
	else if (stageNum_ == SL_Stage1_Area2)LoadLVData("stage2");
	else if (stageNum_ == SL_Stage1_Area3)LoadLVData("stage3_3");
	else if (stageNum_ == SL_Stage1_AreaBoss)LoadLVData("stageboss1");

	else if (stageNum_ == SL_StageTutorial_Area1)LoadLVData("tutorial");
	else if (stageNum_ == SL_StageTutorial_Area2)LoadLVData("tutorial2");
	else if (stageNum_ == SL_StageTutorial_Area3)LoadLVData("tutorial3_2");
	else if (stageNum_ == SL_StageTutorial_Final)LoadLVData("tutorialf");

	//スプライト
	LoadSprite();

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);

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
	for (int i = 0; i < XY_Num; i++)easeTutorialListScale_[i].Standby(false);
}

void GamePlayScene::Update()
{
	//死亡フラグの立った弾、敵を削除
	playerBullets_.remove_if(
		[](std::unique_ptr<PlayerBullet>& pbullet) { return pbullet->IsDead(); });
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& ebullet) { return ebullet->IsDead(); });

	/*players_.remove_if(
		[](std::unique_ptr<Player>& player) {return player->IsDead(); });*/

	enemys_.remove_if(
		[](std::unique_ptr<BaseEnemy>& enemy) {return enemy->IsDead(); });

	for (std::unique_ptr<Player>& player : players_)
	{
		//丸影は常時表示
		SetUpCircleShadow(player->GetPosition());
	}
	//天球(背景)
	for (std::unique_ptr<Object3d>& skydome : skydomes_)
	{
		//天球回転用
		XMFLOAT3 rotSkydome = skydome->GetRotation();
		//天球(背景)は常時回転する
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);
		//更新
		skydome->Update();
	}

	if (isStart_)			UpdateIsStartGame();			//ゲーム開始時
	else if (isGamePlay_)	UpdateIsPlayGame();				//ゲームプレイ時
	else if (isPause_)		UpdateIsPause();				//ポーズ時
	else if (isClear_)		UpdateIsStageClear();			//ステージクリア時
	else if (isGameOver_)	UpdateIsGameOver();				//ゲームオーバー時
	else					UpdateIsQuitGame();				//終了時

	//スプライト更新
	spritePause_->Update();
	spritePauseInfo_->Update();
	spritePauseResume_->Update();
	spritePauseHint_->Update();
	spriteHintInfo_->Update();
	spritePauseStageSelect_->Update();
	spritePauseTitle_->Update();
	spritePauseUI_->Update();
	spriteDone_->Update();
	spriteReady_->Update();
	spriteGo_->Update();
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();
	spriteCursor_->Update();
	spriteHowToPlayList_->Update();
	spriteStageName_->Update();

	//チュートリアル関係
	UpdateTutorial();
	UpdateTutorialSprite();

	//更新
	camera_->Update();				//カメラ
	lightGroup_->Update();			//ライト
	pm_->Update();					//パーティクル

}

void GamePlayScene::UpdateIsStartGame()
{
	//イージング
	easeFadeInOut_.ease_in_out_quint();
	//カメライージング
	easeEyeGameStart_[XYZ_X].ease_in_out_quint();
	easeEyeGameStart_[XYZ_Y].ease_in_out_quint();
	easeEyeGameStart_[XYZ_Z].ease_in_sine();
	easeTargetGameStart_[XYZ_X].ease_in_out_quint();
	easeTargetGameStart_[XYZ_Y].ease_in_out_quint();
	easeTargetGameStart_[XYZ_Z].ease_in_sine();
	//操作説明リスト表示イージング
	for (int i = 0; i < XY_Num; i++)easeTutorialListScale_[i].ease_in_out_bounce();
	spriteHowToPlayList_->SetSize({ easeTutorialListScale_[XY_X].num_X,easeTutorialListScale_[XY_Y].num_X });

	if (isEndReady_)//ゴー
	{
		//ゴー
		for (int i = 0; i < XYW_Num; i++)easeGoSizeAndAlpha_[i].ease_out_cubic();
		//サイズ、カラーセット
		spriteGo_->SetSize({ easeGoSizeAndAlpha_[XYW_X].num_X,easeGoSizeAndAlpha_[XYW_Y].num_X });
		spriteGo_->SetColor({ black_.x,black_.y, black_.z,easeGoSizeAndAlpha_[XYW_W].num_X });

	}
	else if (isStartReadyPart2_)//レディーが一定の位置に達したら
	{
		//レディー(一旦戻ってまた左に)
		easeReadyPosition_[XXY_X2].ease_in_back();
		//座標セット
		spriteReady_->SetPosition({ easeReadyPosition_[XXY_X2].num_X,easeReadyPosition_[XXY_Y].start });
		//移動が終わったら
		if (spriteReady_->GetPosition().x == easeReadyPosition_[XXY_X2].end)
		{
			//ゴーを動かす
			for (int i = 0; i < XYW_Num; i++)easeGoSizeAndAlpha_[i].Standby(false);
			isEndReady_ = true;
			isStartReadyPart2_ = false;
		}
	}
	else//スタート時レディー
	{
		//レディー(そのまま左に)
		easeReadyPosition_[XXY_X1].ease_out_circ();
		//座標セット
		spriteReady_->SetPosition({ easeReadyPosition_[XXY_X1].num_X,easeReadyPosition_[XXY_Y].start });
		//移動が終わったら
		if (spriteReady_->GetPosition().x == easeReadyPosition_[XXY_X1].end)
		{
			//もう一回別のイージングでレディーを動かす
			easeReadyPosition_[XXY_X2].Standby(false);
			isStartReadyPart2_ = true;
		}

	}
	//プレイヤー座標のイージング
	for (int i = 0; i < XYZ_Num; i++)easePlayerPositionGameStart_[i].ease_in_out_quint();

	//フェードインアウト
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.num_X });
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });//ステージ開始時に出る

	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });


	for (std::unique_ptr<Player>& player : players_)
	{
		const XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//オフセット
		//煙プリセット
		const ParticleManager::Preset smoke =
		{
			particle1_.get(),
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
		pm_->ActiveX(smoke);

		//座標セット
		player->SetPosition
		({
			easePlayerPositionGameStart_[XYZ_X].num_X,
			easePlayerPositionGameStart_[XYZ_Y].num_X,
			easePlayerPositionGameStart_[XYZ_Z].num_X
			});

		//更新(操作は出来ない)
		player->Update(false, false, true);

		//ステージ開始時のイージングが終わったらゲーム開始
		if (player->GetPosition().x == easePlayerPositionGameStart_[XYZ_X].end)
		{
			isStart_ = false;
			isGamePlay_ = true;
		}
	}

	//更新
	//プレイヤーが死んでないとき
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<BaseEnemy>& enemy : enemys_)enemy->Update(true);					//敵
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();	//敵弾
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();				//ギミック
	//プレイヤーの生死は関係ない
	for (std::unique_ptr<Goal>& goal : goals_)goal->Update();				//ゴール
	for (std::unique_ptr<Item>& item : items_)item->Update();				//アイテム
	for (std::unique_ptr<Object3d>& object : objects_) object->Update();	//地形

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

		//自機が死んだらゲームオーバー(死亡演出スキップでも同様)
		if (player->IsDead() || player->IsSkipDeadDirection())
		{
			isGameOver_ = true;
			isGamePlay_ = false;
		}
		//ImGui	
		imguiManager_->Begin();
		imguiManager_->End();
	}
	//自機弾更新
	for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets_) playerBullet->Update();
	//敵
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_)
	{
		enemy->Update();
		//ボス撃破でステージクリア
		if (enemy->BossDead())
		{
			isClear_ = true;
			isGamePlay_ = false;
		}
	}
	//敵弾更新
	for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();
	//ギミック更新
	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();
	//ゴール
	for (std::unique_ptr<Goal>& goal : goals_)
	{
		//更新
		goal->Update();
		//ゴールに触れたらステージクリア
		if (goal->IsGoal())
		{
			isClear_ = true;
			isGamePlay_ = false;
		}

	}
	//アイテム更新
	for (std::unique_ptr<Item>& item : items_) item->Update();
	//地形更新
	for (std::unique_ptr<Object3d>& object : objects_) object->Update();

	//ImGui
	//imguiManager_->Begin();
	//camera_->DebugCamera();//デバッグカメラ
	//imguiManager_->End();

	//自機が生きているときだけ当たり判定をチェックする
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())colManager_->CheckAllCollisions();
	//Pause機能
	if (input_->TriggerKey(DIK_ESCAPE))
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
	FadeIn(black_);
	for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();

	//ポジションセット
	// 
	//ポーズ関係
	spritePause_->SetPosition({ easePauseMenuPosX_[PMEN_Menu].num_X, pausePosY_[PMEN_Menu] });
	spritePauseResume_->SetPosition({ easePauseMenuPosX_[PMEN_Resume].num_X, pausePosY_[PMEN_Resume] });
	spritePauseHint_->SetPosition({ easePauseMenuPosX_[PMEN_Hint].num_X, pausePosY_[PMEN_Hint] });
	spritePauseStageSelect_->SetPosition({ easePauseMenuPosX_[PMEN_StageSelect].num_X, pausePosY_[PMEN_StageSelect] });
	spritePauseTitle_->SetPosition({ easePauseMenuPosX_[PMEN_Title].num_X, pausePosY_[PMEN_Title] });
	spriteDone_->SetPosition({ easePauseMenuPosX_[PMEN_SelectSpace].num_X, pausePosY_[PMEN_SelectSpace] });
	spriteHintInfo_->SetPosition({ easePauseMenuPosX_[PMEN_HintInfo].num_X, pausePosY_[PMEN_HintInfo] });
	spritePauseUI_->SetPosition({ easePauseMenuPosX_[PMEN_UI].num_X,pausePosY_[PMEN_UI] });

	//メニュー操作
	if (input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_S))menuCount_++;

	//色を変える(選択しているメニューは強調)
	if (menuCount_ == GPSPMI_Resume)//ゲーム再開を選択時
	{
		spritePauseResume_->SetColor(selectMenuColor);
		spritePauseHint_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Resume] });
	}
	else if (menuCount_ == GPSPMI_Hint)//遊び方説明を選択時
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHint_->SetColor(selectMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Hint] });
	}
	else if (menuCount_ == GPSPMI_StageSelect)//ステージセレクト遷移を選択時
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHint_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(selectMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_StageSelect] });
	}
	else if (menuCount_ == GPSPMI_Title)//タイトル遷移を選択時
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHint_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Title] });
	}

	//デフォルトカラー
	spritePause_->SetColor(pauseColor);
	spriteDone_->SetColor(doneColor);

	//決定時のスプライトのイージングが終わったら操作を受け付ける
	if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			if (menuCount_ == GPSPMI_Resume)//ゲーム再開
			{
				//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;

			}
			else if (menuCount_ == GPSPMI_StageSelect)//ステージセレクトへ戻る
			{
				//ここでイージングの準備。しかし終了座標に到達していないと受け付けない
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;
			}
			else if (menuCount_ == GPSPMI_Title)//タイトルへ戻る
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
		//ポーズを解除するとき画面の明るさを戻す
		if (menuCount_ == GPSPMI_Resume) FadeOut(black_);
		else if (menuCount_ == GPSPMI_StageSelect) FadeOut(black_);
		else if (menuCount_ == GPSPMI_Title) FadeOut(black_);

	}
	//決定時のスプライトのイージングが終わったらポーズを解除
	if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].start)
	{
		if (isBack_)
		{
			if (menuCount_ == GPSPMI_Resume)//ゲーム再開時
			{
				isGamePlay_ = true;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isBack_ = false;
				isPause_ = false;
			}
			else if (menuCount_ == GPSPMI_StageSelect)//ステージセレクトへ戻る
			{
				isQuit_ = true;
				isBack_ = false;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isPause_ = false;
			}
			else if (menuCount_ == GPSPMI_Title)//タイトルへ戻る
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

void GamePlayScene::UpdateIsStageClear()
{
	for (std::unique_ptr<Player>& player : players_)player->Update();

	FadeIn(white_);//白くする
	//完全に白くなったらステージクリアへ遷移
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		sceneManager_->ChangeScene("STAGECLEAR", stageNum_);

	}
}

void GamePlayScene::UpdateIsGameOver()
{
	//ボスを撃破している場合は呼び出されない
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_)if (enemy->BossDead()) return;
	FadeIn(deepRed_);//赤くする
	//完全に赤くなったらゲームオーバーへ遷移
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		sceneManager_->ChangeScene("GAMEOVER", stageNum_);
	}
}

void GamePlayScene::UpdateIsQuitGame()
{
	FadeIn(black_);//黒くする
	//完全に黒くなったら各シーンへ
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//ポーズからステージセレクト、タイトルを選んだとき
		if (menuCount_ == GPSPMI_StageSelect)
		{
			//ステージ選択へ
			if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
			else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
			else sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);
		}
		else if (menuCount_ == GPSPMI_Title) sceneManager_->ChangeScene("TITLE", stageNum_);
	}


}

void GamePlayScene::UpdateTutorial()
{
	//チュートリアルの更新を一括で行う
	if (stageNum_ == SL_StageTutorial_Area1)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			nullptr, nullptr, spriteTutorialInfo1_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area2)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), nullptr, spriteTutorialInfo2_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area3)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo3_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo4_.get());
	}
}

void GamePlayScene::FadeIn(const DirectX::XMFLOAT3& color)
{

	if (isPause_)//ポーズ時の場合
	{
		if (!isFadeInPause_)
		{
			//ここでスタンバイ
			easeFadeInOutPause_.Standby(true);
			isFadeInPause_ = true;
		}
		else
		{
			//ここでフェードインアウト
			easeFadeInOutPause_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOutPause_.num_X });//透明度だけ変える

		}
	}
	else//シーン遷移時の場合
	{
		if (!isFadeInScene_)
		{
			//ここでスタンバイ
			easeFadeInOut_.Standby(true);
			isFadeInScene_ = true;
		}
		else
		{
			//ここでフェードインアウト
			const XMFLOAT3 negapozi = { 1.0f - color.x,1.0f - color.y,1.0f - color.z };
			easeFadeInOut_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOut_.num_X });//透明度だけ変える
			if (isQuit_)spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ネガポジの応用
		}
	}
}

void GamePlayScene::FadeOut(const DirectX::XMFLOAT3& color)
{

	if (isPause_)//ポーズ時の場合にしか使わない
	{
		if (!isFadeOutPause_)
		{
			//ここでスタンバイ
			easeFadeInOutPause_.Standby(false);
			isFadeOutPause_ = true;
		}
		else
		{
			//ここでフェードインアウト
			easeFadeInOutPause_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOutPause_.num_X });//透明度だけ変える

		}
	}
}

void GamePlayScene::Draw()
{
	//モデル
	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	for (std::unique_ptr<Player>& player : players_)player->Draw();					   //自機
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Draw();	   //自機の弾
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->Draw();				   //敵
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();		   //敵の弾
	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_) gimmick->Draw();		   //ギミック
	for (std::unique_ptr<Goal>& goal : goals_)goal->Draw();							   //ゴール
	for (std::unique_ptr<Item>& item : items_)item->Draw();							   //アイテム
	for (auto& skydome : skydomes_)skydome->Draw();									   //天球(背景)
	for (auto& object : objects_)object->Draw();									   //地形

	//モデル描画後処理
	Object3d::PostDraw();

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画
	pm_->Draw();
	for (std::unique_ptr<Player>& player : players_)player->DrawParticle();						//自機のパーティクル
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->DrawParticle();		//自機の弾のパーティクル
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->DrawParticle();					//敵のパーティクル
	for (std::unique_ptr<Goal>& goal : goals_) goal->DrawParticle();							//ゴールのパーティクル
	for (std::unique_ptr<Item>& item : items_)item->DrawParticle();								//アイテムのパーティクル
	//エフェクト描画後処理
	ParticleManager::PostDraw();

	//スプライト描画前処理
	spCommon_->PreDraw();
	//前景スプライト
	if (isPause_)//ポーズ時
	{
		//フェードインアウト
		spriteFadeInOut_->Draw();

		//ポーズ関係のスプライト
		spritePause_->Draw();
		spritePauseResume_->Draw();
		spritePauseHint_->Draw();
		if (menuCount_ == GPSPMI_Hint)spriteHintInfo_->Draw();//メニューカーソルをヒントに合わせた場合のみ表示
		spritePauseStageSelect_->Draw();
		spritePauseTitle_->Draw();
		spritePauseUI_->Draw();
		spriteDone_->Draw();
		spriteCursor_->Draw();
		

	}
	else//ゲームプレイ時
	{
		//アイテムバー
		for (std::unique_ptr<Item>& item : items_) { item->DrawSprite(); }

		for (std::unique_ptr<Player>& player : players_) {
			player->DrawSprite();

			if (!player->IsBreak())//自機が生きてる場合
			{
				//ポーズインフォ
				if (isGamePlay_)spritePauseInfo_->Draw();

				//チュートリアルインフォ
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
				else//チュートリアルじゃない場合
				{
					//操作説明のリストを描画
					spriteHowToPlayList_->Draw();
				}
			}
		}
		//フェードインアウトとロードと現ステージ
		spriteFadeInOut_->Draw();
		spriteLoad_->Draw();
		spriteStageInfoNow_->Draw();
		spriteStageName_->Draw();
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
	//オブジェクト関係
	objects_.clear();
	skydomes_.clear();
	//モデル
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
			newplayer = Player::Create(&modelPlayerList_, this);
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
			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newplayer->SetScale(scale);
			//ここで開始時プレイヤー演出セット
			easePlayerPositionGameStart_[XYZ_X].SetEasing(startEasePlayerPosition_.x, endEasePlayerPosition.x, maxTime);
			easePlayerPositionGameStart_[XYZ_Y].SetEasing(startEasePlayerPosition_.y, endEasePlayerPosition.y, maxTime);
			easePlayerPositionGameStart_[XYZ_Z].SetEasing(startEasePlayerPosition_.z, endEasePlayerPosition.z, maxTime);

			//カメラセット
			newplayer->SetCamera(camera_.get());

			//更新
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
			newenemy->SetStartPos(pos);//初期座標のセット
			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newenemy->SetRotation(rot);

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newenemy->SetScale(scale);

			//カメラセット
			newenemy->SetCamera(camera_.get());

			//更新
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newGimmick->SetScale(scale);

			//カメラセット
			newGimmick->SetCamera(camera_.get());

			//更新
			newGimmick->Update();

			//リストに登録
			gimmicks_.push_back(std::move(newGimmick));
		}
		//動く床
		else if (objectData.objectType.find("MOVEGROUND") == LDTOF_TRUE)
		{
			//敵初期化
			std::unique_ptr<BaseGimmick> newGimmick;
			std::unique_ptr<Player>& player = players_.front();

			newGimmick = gimmickFactory_->CreateMoveGround(objectData.objectPattern, model, player.get());

			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newGimmick->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newGimmick->SetRotation(rot);

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newGimmick->SetScale(scale);

			//カメラセット
			newGimmick->SetCamera(camera_.get());

			//更新
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newgoal->SetScale(scale);

			//カメラセット
			newgoal->SetCamera(camera_.get());

			//更新
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newitem->SetScale(scale);

			//カメラセット
			newitem->SetCamera(camera_.get());

			//更新
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//カメラセット
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//カメラセット
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

			// スケーリング
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//カメラセット
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
		//反転させる
		infoColor_.x -= speedColor;
		infoColor_.y -= speedColor;
		infoColor_.z -= speedColor;
	}

	else
	{
		//色は自動的に増減
		infoColor_.x += speedColor;
		infoColor_.y += speedColor;
		infoColor_.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.5f,0.0f };//{max,min}

	if (infoColor_.x >= maxAndMinSpeedColor.x)//Max値に到達したら反転
	{
		isColorReverse_ = true;
	}
	if (infoColor_.x <= maxAndMinSpeedColor.y)//Min値に到達したら反転を戻す
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
	//モデル読み込んでマップに登録する
	ModelMapping(modelPlayer_, "playeridle");					//自機モデル
	ModelMapping(modelPlayerBullet_, "playerbullet");			//自機弾モデル
	ModelMapping(modelPlayerDash_, "playerdash");				//自機ダッシュモデル
	ModelMapping(modelPlayerJump_, "playerjump");				//自機ジャンプモデル
	ModelMapping(modelPlayerAttack_, "playerattack");			//自機攻撃モデル
	ModelMapping(modelPlayerHit_, "playerhit");					//自機死亡モデル
	ModelMapping(modelEnemy1_, "enemy1");						//通常敵モデル
	ModelMapping(modelEnemy2_, "enemy2");						//ドッスン風敵モデル
	ModelMapping(modelEnemyDanger_, "enemydanger");				//危険な敵モデル
	ModelMapping(modelEnemyBullet_, "enemybullet");				//敵弾モデル
	ModelMapping(modelBoss1_, "boss1");							//ステージ1のボスモデル
	ModelMapping(modelBossCore1_, "core1");						//ステージ1のボスの周りにあるコアのモデル
	ModelMapping(modelGoal_, "sphere");							//ゴールモデル
	ModelMapping(modelItemJump_, "itemjump");					//ジャンプ力強化アイテムモデル
	ModelMapping(modelItemHeal_, "itemheal");					//ライフ回復アイテムモデル
	ModelMapping(modelSpike_, "spikeball");						//トゲのモデル
	ModelMapping(modelBoxUpDown_, "boxud");						//上下する床のモデル
	ModelMapping(modelStageT_, "skydomet");						//チュートリアルステージモデル(天球)
	ModelMapping(modelStage1_, "skydome");						//ステージ1モデル(天球)
	ModelMapping(modelStage2_, "skydome2");						//ステージ2モデル(天球)
	ModelMapping(modelGround_, "ground");						//床のモデル
	ModelMapping(modelSphere_, "sphere2");						//球モデル
	ModelMapping(modelBox_, "ground2");							//AABB床モデル

	//自機に使うモデル
	modelPlayerList_.playerModel = modelPlayer_.get();				//基本態勢
	modelPlayerList_.playerBullet = modelPlayerBullet_.get();		//弾
	modelPlayerList_.playerHit = modelPlayerHit_.get();				//ヒット
	modelPlayerList_.playerMove = modelPlayerDash_.get();			//ダッシュ
	modelPlayerList_.playerJump = modelPlayerJump_.get();			//ジャンプ
	modelPlayerList_.playerAttack = modelPlayerAttack_.get();		//攻撃
}

void GamePlayScene::SettingTutorialEase(const int num, Sprite* s1, Sprite* s2,
	Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)const
{
	//チュートリアルイージングのセッティング
	switch (num)
	{
	case GPSSTEN_Stundby://スタンバイ時
		if (s1 != nullptr)s1->SetPosition({ easeInfoTutorial_[TIEN_Move].start,tutorialInfoPosY_[TIEN_Move] });
		if (s2 != nullptr)s2->SetPosition({ easeInfoTutorial_[TIEN_Dash].start,tutorialInfoPosY_[TIEN_Dash] });
		if (s3 != nullptr)s3->SetPosition({ easeInfoTutorial_[TIEN_Jump].start,tutorialInfoPosY_[TIEN_Jump] });
		if (s4 != nullptr)s4->SetPosition({ easeInfoTutorial_[TIEN_MoveBack].start,tutorialInfoPosY_[TIEN_MoveBack] });
		if (s5 != nullptr)s5->SetPosition({ easeInfoTutorial_[TIEN_Attack].start,tutorialInfoPosY_[TIEN_Attack] });
		if (s6 != nullptr)s6->SetPosition({ easeInfoTutorial_[TIEN_Info].start,tutorialInfoPosY_[TIEN_Info] });
		break;
	case GPSSTEN_Active://イージング開始時

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
	if (stageNum_ >= SL_StageTutorial_Area1)//チュートリアルステージ以外だと何も起こらない
	{
		//チュートリアル説明カラー
		const DirectX::XMFLOAT4 infoColorRed = { 0.1f + infoColor_.x,0.0f,0.0f,1.0f };
		const DirectX::XMFLOAT4 infoColorBlue = { 0.0f,0.0f,0.1f + infoColor_.z,1.0f };

		//操作説明文字カラー
		const DirectX::XMFLOAT4 howToPlayColorActiveRed = { 1.0f,0.1f,0.1f,1.0f };
		const DirectX::XMFLOAT4 howToPlayColorActiveBlue = { 0.1f,0.1f,1.0f,1.0f };
		const DirectX::XMFLOAT4 howToPlayColorDefault = { 0.01f,0.01f,0.01f,1.0f };
		//カラー更新
		UpdateChangeColor();

		//カラーセット
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
		else//通常移動
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
		//カラーセット
		spriteTutorialInfo1_->SetColor(infoColorRed);
		spriteTutorialInfo2_->SetColor(infoColorRed);
		spriteTutorialInfo3_->SetColor(infoColorRed);
		spriteTutorialInfo4_->SetColor(infoColorBlue);
	}
	//チュートリアルスプライトを更新
	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();
	
	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();
}

void GamePlayScene::DrawTutorialSprite(const Sprite* s1, const Sprite* s2,
	const Sprite* s3, const Sprite* s4, const Sprite* s5, const Sprite* s6)const
{
	//チュートリアル用のスプライトを一括描画
	if (s1 != nullptr)s1->Draw();
	if (s2 != nullptr)s2->Draw();
	if (s3 != nullptr)s3->Draw();
	if (s4 != nullptr)s4->Draw();
	if (s5 != nullptr)s5->Draw();
	if (s6 != nullptr)s6->Draw();
}

void GamePlayScene::LoadStageNameSprite()
{
	//ステージごとにステージ名を変更
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-1.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-1.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-2.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-3.png");
	else if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-4.png");
	//アンカーポイントなどをセッティング
	spriteStageName_->Initialize(spCommon_, GPSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化
	spriteStageName_->Update();
}

void GamePlayScene::LoadTutorialSprite()
{
	//チュートリアル時の説明文字スプライト
	spCommon_->LoadTexture(GPSTTI_TutorialInfo1Tex, "texture/info/tinfo1.png");//1
	spriteTutorialInfo1_->Initialize(spCommon_, GPSTTI_TutorialInfo1Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo2Tex, "texture/info/tinfo2.png");//2
	spriteTutorialInfo2_->Initialize(spCommon_, GPSTTI_TutorialInfo2Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo3Tex, "texture/info/tinfo3.png");//3
	spriteTutorialInfo3_->Initialize(spCommon_, GPSTTI_TutorialInfo3Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo4Tex, "texture/info/tinfo4.png");//4
	spriteTutorialInfo4_->Initialize(spCommon_, GPSTTI_TutorialInfo4Tex);

	//チュートリアル時の操作説明
	spCommon_->LoadTexture(GPSTTI_HowToPlayMoveTex, "texture/info/moveinfo.png");//移動　1~
	spriteTutorialHTPMove_->Initialize(spCommon_, GPSTTI_HowToPlayMoveTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayDashTex, "texture/info/dashinfo.png");//ダッシュ　1~
	spriteTutorialHTPDash_->Initialize(spCommon_, GPSTTI_HowToPlayDashTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayJumpTex, "texture/info/jumpinfo.png");//ジャンプ　1~
	spriteTutorialHTPJump_->Initialize(spCommon_, GPSTTI_HowToPlayJumpTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayBackMoveTex, "texture/info/backmoveinfo2.png");//手前、奥側ジャンプ　2~
	spriteTutorialHTPMoveBack_->Initialize(spCommon_, GPSTTI_HowToPlayBackMoveTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayAttackTex, "texture/info/attackinfo2.png");//攻撃　3~
	spriteTutorialHTPAttack_->Initialize(spCommon_, GPSTTI_HowToPlayAttackTex);

	//チュートリアルの進んだ面によってチュートリアル情報を変える
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
		//カラーセット
		spriteTutorialHTPDash_->SetColor(howToPlayColor);
		spriteTutorialHTPMove_->SetColor(howToPlayColor);
		spriteTutorialHTPJump_->SetColor(howToPlayColor);
		spriteTutorialHTPMoveBack_->SetColor(howToPlayColor);
		spriteTutorialHTPAttack_->SetColor(howToPlayColor);
	}
}

void GamePlayScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
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

void GamePlayScene::LoadSprite()
{
	//どのキでポーズするのかを書いたスプライト
	spCommon_->LoadTexture(GPSTI_PauseInfoTex, "texture/pauseinfo.png");
	spritePauseInfo_->Initialize(spCommon_, GPSTI_PauseInfoTex);

	//ポーズ時のスプライト
	spCommon_->LoadTexture(GPSTI_PauseTex, "texture/pause.png");
	spritePause_->Initialize(spCommon_, GPSTI_PauseTex);
	spritePause_->SetPosition({ easePauseMenuPosX_[PMEN_Menu].start,pausePosY_[PMEN_Menu] });

	//ポーズ時にゲーム再開するかを書いたスプライト
	spCommon_->LoadTexture(GPSTI_PauseResumeTex, "texture/resume.png");
	spritePauseResume_->Initialize(spCommon_, GPSTI_PauseResumeTex);
	spritePauseResume_->SetPosition({ easePauseMenuPosX_[PMEN_Resume].start,pausePosY_[PMEN_Resume] });

	//ポーズ時にヒントを確認するかを書いたスプライト
	spCommon_->LoadTexture(GPSTI_PauseHowToPlayTex, "texture/hint.png");
	spritePauseHint_->Initialize(spCommon_, GPSTI_PauseHowToPlayTex);
	spritePauseHint_->SetPosition({ easePauseMenuPosX_[PMEN_Hint].start,pausePosY_[PMEN_Hint] });

	//ポーズ時にステージセレクトへ戻るかを書いたスプライト
	spCommon_->LoadTexture(GPSTI_PauseStageSelectTex, "texture/backstageselect.png");
	spritePauseStageSelect_->Initialize(spCommon_, GPSTI_PauseStageSelectTex);
	spritePauseStageSelect_->SetPosition({ easePauseMenuPosX_[PMEN_StageSelect].start,pausePosY_[PMEN_StageSelect] });

	//ポーズ時にタイトルへ戻るかを書いたスプライト
	spCommon_->LoadTexture(GPSTI_PauseTitleTex, "texture/backtitle.png");
	spritePauseTitle_->Initialize(spCommon_, GPSTI_PauseTitleTex);
	spritePauseTitle_->SetPosition({ easePauseMenuPosX_[PMEN_Title].start,pausePosY_[PMEN_Title] });

	//メニュー操作スプライト
	spCommon_->LoadTexture(GPSTI_PauseUITex, "texture/menuui.png");
	spritePauseUI_->Initialize(spCommon_, GPSTI_PauseUITex);
	spritePauseUI_->SetPosition({ easePauseMenuPosX_[PMEN_UI].start,pausePosY_[PMEN_UI] });

	//決定表示のスプライト
	spCommon_->LoadTexture(GPSTI_PauseDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, GPSTI_PauseDoneTex);
	spriteDone_->SetPosition({ easePauseMenuPosX_[PMEN_SelectSpace].start,pausePosY_[PMEN_SelectSpace] });

	//Ready表記文字用のスプライト
	spCommon_->LoadTexture(GPSTI_ReadyTex, "texture/ready2.png");
	spriteReady_->Initialize(spCommon_, GPSTI_ReadyTex);
	spriteReady_->SetPosition({ easeReadyPosition_[XXY_X1].start,easeReadyPosition_[XXY_Y].start });
	spriteReady_->SetColor({ black_.x,black_.y,black_.z,easeFadeInOut_.start });//真っ黒

	//Go表記文字用のスプライト
	spCommon_->LoadTexture(GPSTI_GoTex, "texture/go.png");
	spriteGo_->Initialize(spCommon_, GPSTI_GoTex);
	spriteGo_->SetSize({ easeGoSizeAndAlpha_[XYW_X].start,easeGoSizeAndAlpha_[XYW_Y].start });
	spriteGo_->SetPosition({ goPosition_[XY_X],goPosition_[XY_Y] });

	const XMFLOAT2 centralAnchorPoint = { 0.5f,0.5f };//アンカーポイントは中央
	spriteGo_->SetAnchorPoint(centralAnchorPoint);
	spriteGo_->SetColor({ black_.x,black_.y,black_.z,easeGoSizeAndAlpha_[XYW_W].start });

	//フェードインアウトスプライト
	spCommon_->LoadTexture(GPSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, GPSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });//真っ白

	//ロードスプライト
	spCommon_->LoadTexture(GPSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, GPSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//現在ステージスプライト
	const int32_t remainderNum = stageNum_ % 10;//余りによってスプライトを変える
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage1.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage4) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stagef.png");
	spriteStageInfoNow_->Initialize(spCommon_, GPSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//カーソルスプライト
	spCommon_->LoadTexture(GPSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, GPSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,pausePosY_[PMEN_Resume] });

	//遊び方説明リストのスプライト
	spCommon_->LoadTexture(GPSTI_HowToPlayListTex, "texture/info/howtoplaylist2.png");
	spriteHowToPlayList_->Initialize(spCommon_, GPSTI_HowToPlayListTex);
	const XMFLOAT2 howToPlayListPosition = { 30.0f,70.0f };
	spriteHowToPlayList_->SetPosition(howToPlayListPosition);
	//イージングを初期化
	easeTutorialListScale_[XY_X].SetEasing(0.0f, spriteHowToPlayList_->GetSize().x, presetEaseTutorialListScale_[XY_X].maxtime);
	easeTutorialListScale_[XY_Y].SetEasing(0.0f, spriteHowToPlayList_->GetSize().y, presetEaseTutorialListScale_[XY_Y].maxtime);
	spriteHowToPlayList_->SetSize({ easeTutorialListScale_[XY_X].start,easeTutorialListScale_[XY_Y].start });
	
	//ヒントの内容はステージごとに違う
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-1.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-1.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-2.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-3.png");
	else if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-4.png");
	spriteHintInfo_->Initialize(spCommon_, GPSTI_HintInfoTex);
	spriteHintInfo_->SetPosition({ easePauseMenuPosX_[PMEN_HintInfo].start,pausePosY_[PMEN_HintInfo] });

	//ステージ名スプライト
	LoadStageNameSprite();

	//チュートリアルスプライト
	LoadTutorialSprite();

	//各スプライトを更新
	spritePause_->Update();
	spritePauseInfo_->Update();
	spritePauseResume_->Update();
	spritePauseHint_->Update();
	spritePauseStageSelect_->Update();
	spritePauseTitle_->Update();
	spritePauseUI_->Update();
	spriteDone_->Update();
	spriteReady_->Update();
	spriteGo_->Update();
	spriteFadeInOut_->Update();

	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();
	spriteHintInfo_->Update();

	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();

}
