#pragma once
#include "EnumList.h"
#include "XYZ.h"

#include <DirectXMath.h>
#include <map>

#include "SceneManager.h"

//jsonレベルデータ
struct LevelData;

/*

*	StageSelectScene.h

*	ステージセレクトシーン

*/
class StageSelectScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ番号や天球背景と連動した値)
	StageSelectScene(const int count);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//ステージセレクト中
	void UpdateIsStageSelect();
	//決定時
	void UpdateIsDone();
	//決定からのゲームスタート
	void UpdateIsGameStart();
	//タイトルへ戻るとき 
	void UpdateIsQuitTitle();

	//描画
	void Draw() override;
	//終了
	void Finalize() override;
	//フェードアウト
	void FadeIn(const DirectX::XMFLOAT3& color) override;
	//レベルデータ読み込み
	void LoadLVData(const std::string& stagePath);

private://静的メンバ変数
	//DirectX基盤
	static DirectXCommon* dxCommon_;

	//インプット
	static Input* input_;
	//オーディオ
	static Audio* audio_;
	//シーンマネージャー
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;

private://メンバ変数
	//ステージセレクト用テクスチャインデックス
	enum StageSelectSceneTextureIndex
	{
		SSSTI_MenuTex = 0,
		SSSTI_MenuTutorialTex = 1,
		SSSTI_Menustage1Tex = 2,
		SSSTI_Menustage2Tex = 3,
		SSSTI_MenuDoneTex = 4,
		SSSTI_BackTitleTex = 5,
		SSSTI_FadeInOutTex = 6,
		SSSTI_LoadingTex = 7,
		SSSTI_StageInfoNowTex = 8,
		SSSTI_CursorTex = 9,
		SSSTI_StageNameTex = 10,
		SSSTI_MenuUITex = 11,
	};

	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	//Sprite
	std::unique_ptr<Sprite> spriteMenu_ = std::make_unique<Sprite>();				//メニュー画面スプライト
	std::unique_ptr<Sprite> spriteTutorial_ = std::make_unique<Sprite>();			//チュートリアルステージスプライト
	std::unique_ptr<Sprite> spriteStage1_ = std::make_unique<Sprite>();				//ステージ1スプライト
	std::unique_ptr<Sprite> spriteStage2_ = std::make_unique<Sprite>();				//ステージ2スプライト
	std::unique_ptr<Sprite> spriteDone_ = std::make_unique<Sprite>();				//決定表示スプライト
	std::unique_ptr<Sprite> spriteBack_ = std::make_unique<Sprite>();				//タイトルに戻るスプライト
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//フェードインアウトスプライト
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//ロードスプライト
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//現在ステージスプライト
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//カーソルスプライト
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();			//ステージ名スプライト
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();				//メニュー操作方法スプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//オブジェクト
	std::unique_ptr<Object3d> objStage_ = nullptr;				//ステージオブジェクト
	std::vector<std::unique_ptr<Object3d>> objPlayers_;			//自機オブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objGoals_;			//ゴールオブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objGrounds_;			//床オブジェクト配列

	//モデル
	std::unique_ptr<Model> modelPlayer_ = nullptr;				//自機モデル
	std::unique_ptr<Model> modelGoal_ = nullptr;				//ゴールモデル
	std::unique_ptr<Model> modelGround_ = nullptr;				//床モデル
	std::unique_ptr<Model> modelStageTutorial_ = nullptr;		//チュートリアルステージモデル(天球)
	std::unique_ptr<Model> modelStage1_ = nullptr;				//ステージ1モデル(天球)
	std::unique_ptr<Model> modelStage2_ = nullptr;				//ステージ2モデル(天球)
	
	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_;

	//メニュー説明用の列挙体
	enum StageSelectMenuEasingNum
	{
		SSMEN_Menu = 0,				//メニュー
		SSMEN_Tutorial = 1,			//チュートリアル
		SSMEN_Stage1_Sky = 2,		//ステージ1
		SSMEN_Stage2_Space = 3,		//ステージ2
		SSMEN_SelectSpace = 4,		//スペースで選択
		SSMEN_UI = 5,				//操作方法
		SSMEN_Quit = 6,				//戻る
		SSMEN_Num = 7,				//配列用
	};
	//メニューポジション
	const std::array<float, SSMEN_Num> menuPosY_ = { 0.0f,150.0f,300.0f,450.0f,600.0f,300.0f,50.0f };

	//イージング類
	//メニュー画面出現イージングのプリセット
	const Easing presetEaseMenuPosX_[SSMEN_Num] =
	{
		{1300.0f, 0.0f, 1.0f},					//メニュー
		{1300.0f, 100.0f, 1.2f},				//チュートリアル
		{1300.0f, 100.0f, 1.4f},				//ステージ1
		{1300.0f, 100.0f, 1.6f},				//ステージ2
		{1300.0f, 425.0f, 1.8f},				//スペースで選択
		{1300.0f, 1100.0f, 2.0f},				//操作方法
		{1300.0f, 900.0f, 2.2f}					//戻る
	};
	//メニュー画面出現イージング
	Easing easeMenuPosX_[SSMEN_Num] =
	{
		presetEaseMenuPosX_[SSMEN_Menu],					//メニュー
		presetEaseMenuPosX_[SSMEN_Tutorial],				//チュートリアル
		presetEaseMenuPosX_[SSMEN_Stage1_Sky],				//ステージ1
		presetEaseMenuPosX_[SSMEN_Stage2_Space],			//ステージ2
		presetEaseMenuPosX_[SSMEN_SelectSpace],				//スペースで選択
		presetEaseMenuPosX_[SSMEN_UI],						//操作方法
		presetEaseMenuPosX_[SSMEN_Quit]						//戻る
	};
	//メニュー画面出現イージングのプリセット
	const Easing presetEaseMenuEndPosX_[SSMEN_Num] =
	{
		{ 0.0f, -1300.0f,1.0f},					//メニュー
		{ 100.0f,-1300.0f, 1.1f},				//チュートリアル
		{ 100.0f,-1300.0f, 1.2f},				//ステージ1
		{ 100.0f,-1300.0f, 1.3f},				//ステージ2
		{ 425.0f,-1300.0f, 1.4f},				//スペースで選択
		{ 1100.0f,-1300.0f, 1.5f},				//操作方法
		{ 900.0f,-1300.0f, 1.6f}				//戻る
	};
	//メニュー画面出現イージング
	Easing easeMenuEndPosX_[SSMEN_Num] =
	{
		presetEaseMenuEndPosX_[SSMEN_Menu],					//メニュー
		presetEaseMenuEndPosX_[SSMEN_Tutorial],				//チュートリアル
		presetEaseMenuEndPosX_[SSMEN_Stage1_Sky],			//ステージ1
		presetEaseMenuEndPosX_[SSMEN_Stage2_Space],			//ステージ2
		presetEaseMenuEndPosX_[SSMEN_SelectSpace],			//スペースで選択
		presetEaseMenuEndPosX_[SSMEN_UI],					//操作方法
		presetEaseMenuEndPosX_[SSMEN_Quit]					//戻る
	};
	//カーソルX値のイージングプリセット
	const Easing presetEaseCursorPosX_{ -200.0f,20.0f,1.0f };
	//カーソルX値のイージング
	Easing easeCursorPosX_ = presetEaseCursorPosX_;

	//選んだステージを真ん中に移動させるイージング
	const Easing presetEaseStartStagePosX_[SSSMI_Num] =
	{
		{100.0f, 350.0f, 1.5f},				//チュートリアルへ
		{100.0f, 550.0f, 1.5f},				//ステージ1へ
		{100.0f, 500.0f, 1.5f}				//ステージ2へ
	};
	//選んだステージを真ん中に移動させるイージング
	Easing easeStartStagePosX_[SSSMI_Num] =
	{
		presetEaseStartStagePosX_[SSSMI_StageTutorial_Tutorial],		//チュートリアルへ
		presetEaseStartStagePosX_[SSSMI_Stage1_SkyStage],				//ステージ1へ
		presetEaseStartStagePosX_[SSSMI_Stage2_SpaceStage]				//ステージ2へ
	};

	//選んだステージを上に移動させるイージングのプリセット
	const Easing presetEaseStartStagePosY_[SSSMI_Num] =
	{
		{menuPosY_[SSMEN_Tutorial], 0.0f, 1.5f},				//チュートリアルへ
		{menuPosY_[SSMEN_Stage1_Sky], 0.0f, 1.5f},				//ステージ1へ
		{menuPosY_[SSMEN_Stage2_Space], 0.0f, 1.5f}				//ステージ2へ
	};
	//選んだステージを上に移動させるイージング
	Easing easeStartStagePosY_[SSSMI_Num] =
	{
		presetEaseStartStagePosY_[SSSMI_StageTutorial_Tutorial],		//チュートリアルへ
		presetEaseStartStagePosY_[SSSMI_Stage1_SkyStage],				//ステージ1へ
		presetEaseStartStagePosY_[SSSMI_Stage2_SpaceStage]				//ステージ2へ
	};

	//遷移時すぐに起動するステージセレクト視点カメラワークイージングのプリセット
	const Easing presetEaseEyeStageSelect_[XYZ_Num] =
	{
		{-22.0f, 144.0f, 1.0f},					//X
		{-1.0f, 45.0f, 1.0f},					//Y
		{-60.0f, -98.0f, 1.0f}					//Z
	};
	//遷移時すぐに起動するステージセレクト視点カメラワークイージング
	Easing easeEyeStageSelect_[XYZ_Num] =
	{
		presetEaseEyeStageSelect_[XYZ_X],				//X
		presetEaseEyeStageSelect_[XYZ_Y],				//Y
		presetEaseEyeStageSelect_[XYZ_Z]				//Z
	};

	//遷移時すぐに起動するステージセレクト注視点カメラワークイージングのプリセット
	const Easing presetEaseTargetStageSelect_[XYZ_Num] =
	{
		{50.0f, 132.0f, 1.0f},					//X
		{-8.0f, 39.0f, 1.0f},					//Y
		{-57.0f, -52.0f, 1.0f}					//Z
	};
	//遷移時すぐに起動するステージセレクト注視点カメラワークイージング
	Easing easeTargetStageSelect_[XYZ_Num] =
	{
		presetEaseTargetStageSelect_[XYZ_X],				//X
		presetEaseTargetStageSelect_[XYZ_Y],				//Y
		presetEaseTargetStageSelect_[XYZ_Z]					//Z
	};

	//ステージ決定時の視点カメラワークイージングのプリセット
	const Easing presetEaseEyeDoneMenu_[XYZ_Num] =
	{
		{144.0f, 127.0f, 1.0f},					//X
		{45.0f, 52.0f, 1.0f},					//Y
		{-98.0f, -60.0f, 1.0f}					//Z
	};
	//ステージ決定時の視点カメラワークイージング
	Easing easeEyeDoneMenu_[XYZ_Num] =
	{
		presetEaseEyeDoneMenu_[XYZ_X],					//X
		presetEaseEyeDoneMenu_[XYZ_Y],					//Y
		presetEaseEyeDoneMenu_[XYZ_Z]					//Z
	};

	//ステージ決定時の注視点カメラワークイージングのプリセット
	const Easing presetEaseTargetDoneMenu_[XYZ_Num] =
	{
		{132.0f, 192.0f, 1.0f},					//X
		{39.0f, 33.0f, 1.0f},					//Y
		{-52.0f, -60.0f, 1.0f}					//Z
	};
	//ステージ決定時の注視点カメラワークイージング
	Easing easeTargetDoneMenu_[XYZ_Num] =
	{
		presetEaseTargetDoneMenu_[XYZ_X],					//X
		presetEaseTargetDoneMenu_[XYZ_Y],					//Y
		presetEaseTargetDoneMenu_[XYZ_Z]					//Z
	};

	//ステージ決定時の視点カメラワークイージングが終わった後の突入視点カメラワークイージングのプリセット
	const Easing presetEaseEyeGameStart_[XYZ_Num] =
	{
		{127.0f, 227.0f, 2.0f},					//X
		{52.0f, 55.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//ステージ決定時の視点カメラワークイージングが終わった後の突入視点カメラワークイージング
	Easing easeEyeGameStart_[XYZ_Num] =
	{
		presetEaseEyeGameStart_[XYZ_X],					//X
		presetEaseEyeGameStart_[XYZ_Y],					//Y
		presetEaseEyeGameStart_[XYZ_Z]					//Z
	};

	//ステージ決定時の視点カメラワークイージングが終わった後の突入注視点カメラワークイージングのプリセット
	const Easing presetEaseTargetGameStart_[XYZ_Num] =
	{
		{192.0f, 292.0f, 2.0f},					//X
		{33.0f, 33.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//ステージ決定時の視点カメラワークイージングが終わった後の突入注視点カメラワークイージング
	Easing easeTargetGameStart_[XYZ_Num] =
	{
		presetEaseTargetGameStart_[XYZ_X],					//X
		presetEaseTargetGameStart_[XYZ_Y],					//Y
		presetEaseTargetGameStart_[XYZ_Z]					//Z
	};

	//ステージ決定時の視点カメラワークイージングが終わった後の自機移動イージングのプリセット
	const Easing presetEasePlayerStartMove_[XYZ_Num] =
	{
		{150.0f, 250.0f, 2.0f},					//X
		{40.0f, 40.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//ステージ決定時の視点カメラワークイージングが終わった後の自機移動イージング
	Easing easePlayerStartMove_[XYZ_Num] =
	{
		presetEasePlayerStartMove_[XYZ_X],				//X
		presetEasePlayerStartMove_[XYZ_Y],				//Y
		presetEasePlayerStartMove_[XYZ_Z]				//Z
	};

	//タイトルに戻るときの自機移動イージングのプリセット
	const Easing presetEasePlayerQuitMove_[XYZ_Num] =
	{
		{150.0f, 0.0f, 2.0f},					//X
		{40.0f, -8.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//タイトルに戻るときの自機移動イージング
	Easing easePlayerQuitMove_[XYZ_Num] =
	{
		presetEasePlayerQuitMove_[XYZ_X],					//X
		presetEasePlayerQuitMove_[XYZ_Y],					//Y
		presetEasePlayerQuitMove_[XYZ_Z]					//Z
	};

	//フェードインアウトのプリセット
	const Easing presetEaseFadeInOut_ = { 1.0f, 0.0f, 1.0f };
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;

	//選択しているメニュー表示
	int menuCount_ = SSSMI_StageTutorial_Tutorial;
	//色を変えるスピード
	DirectX::XMFLOAT3 selectColor_;
	//色反転フラグ
	bool isColorReverse_ = false;

	//遷移フラグ類
	bool isStageSelect_ = true;		//ステージセレクト時
	bool isStart_ = false;			//ゲームスタート時
	bool isDone_ = false;			//ステージ決定時
	bool outStageSelect_ = false;	//ステージセレクトから抜ける
	bool isFadeIn_ = false;			//フェードイン

	//ライト
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;//炎
	std::unique_ptr<Particle> particleGoal_ = nullptr;//ゴール
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;//炎
	std::unique_ptr<ParticleManager> pmGoal_ = nullptr;//ゴール

public:
	//色が変わる処理
	void UpdateChangeColor();
	//丸影セット
	void SetUpCircleShadow(const DirectX::XMFLOAT3& pos);
};
