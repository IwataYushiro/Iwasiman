#include "TitleScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"

#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

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

TitleScene::TitleScene(int stagenum) : stageNum_(stagenum) {}

void TitleScene::Initialize()
{
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());

	spCommon_ = SpriteCommon::GetInstance();

	//カメラ初期化
	camera_ = new Camera();
	//オーディオ
	audio_->Initialize();

	// 視点座標
	camera_->SetEye({ easeEyeMenu_[XYZ_X].start, easeEyeMenu_[XYZ_Y].start, easeEyeMenu_[XYZ_Z].start });
	// 注視点座標
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].start, easeTargetMenu_[XYZ_Y].start, easeTargetMenu_[XYZ_Z].start });

	//レベルデータ読み込み
	if (stageNum_ == SL_Default)LoadLVData("scene/titlet");
	else if (stageNum_ <= SL_Stage1_StageID)LoadLVData("scene/title1");
	else if (stageNum_ <= SL_Stage2_StageID)LoadLVData("scene/title2");
	else LoadLVData("scene/titlet");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	spCommon_->LoadTexture(TSTI_TitleTex, "texture/title3.png");
	spriteTitle_->Initialize(spCommon_, TSTI_TitleTex);
	spriteTitle_->SetColor(backTitleColor_);
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].start,startTitlePosY_[TS_Title]});

	spCommon_->LoadTexture(TSTI_TitleDoneTex, "texture/space2.png");
	spriteTitleDone_->Initialize(spCommon_, TSTI_TitleDoneTex);
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].start,startTitlePosY_[TS_Done] });

	spCommon_->LoadTexture(TSTI_MenuTex, "texture/titlemenu.png");
	spriteMenu_->Initialize(spCommon_, TSTI_MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].start,menuPosY_[TMEN_Menu]});
	spriteMenu_->SetColor(otherMenuColor_);

	spCommon_->LoadTexture(TSTI_MenuTutorialTex, "texture/titlemenut.png");
	spriteMenuTutorial_->Initialize(spCommon_, TSTI_MenuTutorialTex);
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].start,easeStartStagePosY_.start });

	spCommon_->LoadTexture(TSTI_MenuStageSerectTex, "texture/titlemenus.png");
	spriteMenuStageSelect_->Initialize(spCommon_, TSTI_MenuStageSerectTex);
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].start,menuPosY_[TMEN_StageSelect] });

	spCommon_->LoadTexture(TSTI_MenuDoneTex, "texture/titlemenud.png");
	spriteMenuDone_->Initialize(spCommon_, TSTI_MenuDoneTex);
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].start,menuPosY_[TMEN_SelectSpace] });
	spriteMenuDone_->SetColor(otherMenuColor_);

	spCommon_->LoadTexture(TSTI_BackTitleTex, "texture/back.png");
	spriteBack_->Initialize(spCommon_, TSTI_BackTitleTex);
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].start,menuPosY_[TMEN_Quit] });
	spriteBack_->SetColor(backTitleColor_);

	spCommon_->LoadTexture(TSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, TSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.start });

	spCommon_->LoadTexture(TSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, TSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.end });//透明化

	spCommon_->LoadTexture(TSTI_StageInfoNowTex, "texture/stage1.png");
	spriteStageInfoNow_->Initialize(spCommon_, TSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//透明化

	//FBX
	//objF = ObjectFbx::Create();
	//modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	//objF->SetModelFBX(modelF);
	//objF->SetCamera(camera_);
	//objF->PlayAnimation();//更新で呼ぶと止まるから注意

	//パーティクル
	particle1_ = Particle::LoadFromParticleTexture("particle8.png");
	particle2_ = Particle::LoadFromParticleTexture("particle1.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	pm2_ = ParticleManager::Create();
	pm2_->SetBlendMode(ParticleManager::BP_ADD);
	pm2_->SetParticleModel(particle2_);
	pm2_->SetCamera(camera_);


	easeFadeInOut_.Standby(false);
}

void TitleScene::Update()
{

	//メニュー上下限
	if (menuCount_ <= TSMI_Tutorial)menuCount_ = TSMI_Tutorial;
	else if (menuCount_ >= TSMI_StageSelect)menuCount_ = TSMI_StageSelect;

	const DirectX::XMFLOAT4 TitleDoneColor = { 0.0f,0.0f,0.1f + selectColor_.z,1.0f };
	//SetColorより前に呼び出す
	UpdateChangeColor();

	if (isStartGame_)UpdateIsStartGame();
	else if (isStageSelect_)UpdateIsStageSelect();
	else if (isMenu_)UpdateIsMenu();
	else if (isBack_)UpdateIsBack();
	else
	{
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });//透明度だけ変える

		if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.end)
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
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();

	for (Object3d*& player : objPlayers_)
	{
		//煙プリセット
		const ParticleManager::Preset smoke =
		{
			particle2_,
			player->GetPosition(),
			{ 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			3,
			{ 1.0f, 0.0f },
			{ 1.0f,1.0f,1.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		//パーティクル
		pm2_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		player->Update();
	}

	for (Object3d*& ground : objGrounds_)
	{
		DirectX::XMFLOAT3 move = ground->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.0f,0.0f,0.0f };
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
	pm2_->Update();
	
	//objF->Update();

	imguiManager_->Begin();
#ifdef _DEBUG
	//camera_->DebugCamera(true);
#endif // _DEBUG

	imguiManager_->End();
}

void TitleScene::UpdateIsStartGame()
{
	for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_out_expo();
	easeStartStagePosX_.ease_out_expo();
	easeStartStagePosY_.ease_out_expo();

	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeStartStagePosX_.num_X,easeStartStagePosY_.num_X });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });

	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });

	for (Object3d*& goal : objGoals_)
	{

		DirectX::XMFLOAT3 move = goal->GetPosition();
		const DirectX::XMFLOAT3 speed = { -1.5f,0.0f,0.0f };
		//X値がここまで来たらイージング
		const float gameStartPos_ = 60.0f;

		move.x += speed.x;
		goal->SetPosition(move);
		if (goal->GetPosition().x <= gameStartPos_)
		{
			FadeOut(white_);//ゲームプレイ遷移時は白くする
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//チュートリアルステージへ
				sceneManager_->ChangeScene("GAMEPLAY", SL_StageTutorial_Area1);
			}
		};


	}

}

void TitleScene::UpdateIsStageSelect()
{
	for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].ease_in_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easePlayerMove_[i].ease_in_out_expo();

	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial]});
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });


	//カメラもセット
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });

	for (Object3d*& player : objPlayers_)
	{
		player->SetPosition({ easePlayerMove_[XYZ_X].num_X,easePlayerMove_[XYZ_Y].num_X,easePlayerMove_[XYZ_Z].num_X });

		if (spriteMenu_->GetPosition().x == easeMenuPosX_[TMEN_Menu].start)
		{
			FadeOut(black_);//黒くする
			if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
			{
				//ステージ選択
				if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
				else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_TowerStage);
				else sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);//チュートリアルに飛ばすと本末転倒
			}
		}
	}

}

void TitleScene::UpdateIsBack()
{
	//イージング
	for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < 5; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < 3; i++)easeTargetMenu_[i].ease_out_expo();
	//座標セット
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title]});
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].num_X,startTitlePosY_[TS_Done] });
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial] });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });

	//カメラもセット
	camera_->SetEye({ easeEyeMenu_[XYZ_X].num_X, easeEyeMenu_[XYZ_Y].num_X, easeEyeMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].num_X, easeTargetMenu_[XYZ_Y].num_X, easeTargetMenu_[XYZ_Z].num_X });

	if (spriteBack_->GetPosition().x == easeMenuPosX_[TMEN_Quit].start)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{

			for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].Standby(false);
			for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].Standby(false);
			for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].Standby(false);
			isMenu_ = true;
			isBack_ = false;
		}
	}
}

void TitleScene::UpdateIsMenu()
{
	const DirectX::XMFLOAT4 selectMenuColor = { 0.1f + selectColor_.x,0.1f,0.1f,1.0f };

	//イージング
	for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].ease_out_expo();
	for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].ease_out_expo();
	for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].ease_out_expo();

	//座標セット
	spriteTitle_->SetPosition({ easeTitlePosX_[TS_Title].num_X,startTitlePosY_[TS_Title] });
	spriteTitleDone_->SetPosition({ easeTitlePosX_[TS_Done].num_X,startTitlePosY_[TS_Done] });
	spriteMenu_->SetPosition({ easeMenuPosX_[TMEN_Menu].num_X,menuPosY_[TMEN_Menu] });
	spriteMenuTutorial_->SetPosition({ easeMenuPosX_[TMEN_Tutorial].num_X,menuPosY_[TMEN_Tutorial] });
	spriteMenuStageSelect_->SetPosition({ easeMenuPosX_[TMEN_StageSelect].num_X,menuPosY_[TMEN_StageSelect] });
	spriteMenuDone_->SetPosition({ easeMenuPosX_[TMEN_SelectSpace].num_X,menuPosY_[TMEN_SelectSpace] });
	spriteBack_->SetPosition({ easeMenuPosX_[TMEN_Quit].num_X,menuPosY_[TMEN_Quit] });

	//カメラもセット
	camera_->SetEye({ easeEyeMenu_[XYZ_X].num_X, easeEyeMenu_[XYZ_Y].num_X, easeEyeMenu_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetMenu_[XYZ_X].num_X, easeTargetMenu_[XYZ_Y].num_X, easeTargetMenu_[XYZ_Z].num_X });

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


	if (easeMenuPosX_[0].num_X == easeMenuPosX_[0].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			if (menuCount_ == TSMI_Tutorial)
			{
				for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
				easeStartStagePosX_.Standby(false);
				easeStartStagePosY_.Standby(false);
				isStartGame_ = true;
			}
			else if (menuCount_ == TSMI_StageSelect)
			{
				for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(true);
				for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
				for (int i = 0; i < XYZ_Num; i++)easePlayerMove_[i].Standby(false);
				isStageSelect_ = true;

			}
		}
		if (input_->TriggerKey(DIK_Q))
		{
			for (int i = 0; i < TS_Num; i++)easeTitlePosX_[i].Standby(true);
			for (int i = 0; i < TMEN_Num; i++)easeMenuPosX_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeEyeMenu_[i].Standby(true);
			for (int i = 0; i < XYZ_Num; i++)easeTargetMenu_[i].Standby(true);
			isBack_ = true;
			isMenu_ = false;
		}
	}

}

void TitleScene::FadeOut(DirectX::XMFLOAT3 rgb)
{
	if (!isFadeOut_)
	{
		easeFadeInOut_.Standby(true);
		isFadeOut_ = true;
	}
	else
	{
		const DirectX::XMFLOAT3 negapozi = { 1.0f - rgb.x,1.0f - rgb.y,1.0f - rgb.z };
		easeFadeInOut_.ease_in_out_quint();
		spriteFadeInOut_->SetColor({ rgb.x,rgb.y,rgb.z, easeFadeInOut_.num_X });//透明度だけ変える
		spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ネガポジの応用
		if (isStartGame_)spriteStageInfoNow_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//ステージ開始時に出る
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
	if (!isStageSelect_)for (Object3d*& goal : objGoals_)goal->Draw();
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
	pm2_->Draw();
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
	spriteFadeInOut_->Draw();
	spriteLoad_->Draw();
	spriteStageInfoNow_->Draw();
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
	delete spriteFadeInOut_;
	delete spriteLoad_;
	delete spriteStageInfoNow_;

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

	//カメラ
	delete camera_;
	//ライト
	delete lightGroup_;
	//パーティクル
	delete particle1_;
	delete particle2_;
	delete pm1_;
	delete pm2_;
	//FBX
	//delete objF;
	//delete modelF;
}

void TitleScene::LoadLVData(const std::string& stagePath)
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

		if (objectData.objectType.find("PLAYER") == LDTOF_TRUE)
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
		else if (objectData.objectType.find("PLANE") == LDTOF_TRUE)
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
		else if (objectData.objectType.find("SKYDOME") == LDTOF_TRUE)
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
		else if (objectData.objectType.find("GOAL") == LDTOF_TRUE)
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
	const float speedColor = 0.02f;

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
