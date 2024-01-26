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
	//イージングのロード
	void LoadEasing() override;

private://基盤メンバ変数
	//DirectX基盤
	DirectXCommon* dxCommon_ = nullptr;
	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//インプット
	Input* input_ = nullptr;
	//オーディオ
	Audio* audio_ = nullptr;
	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;
	//imgui
	ImGuiManager* imguiManager_ = nullptr;

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

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	//Sprite
	std::unique_ptr<Sprite> spriteMenu_ = std::make_unique<Sprite>();				//メニュー画面スプライト
	std::unique_ptr<Sprite> spriteTutorial_ = std::make_unique<Sprite>();			//チュートリアルステージスプライト
	std::unique_ptr<Sprite> spriteStage1_ = std::make_unique<Sprite>();				//ステージ1スプライト
	std::unique_ptr<Sprite> spriteStage2_ = std::make_unique<Sprite>();				//ステージ2スプライト
	std::unique_ptr<Sprite> spriteDone_ = std::make_unique<Sprite>();				//決定表示兼スキップキースプライト
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
	//メニュー画面出現イージング
	Easing easeMenuPosX_[SSMEN_Num];
	//メニュー画面通過イージング
	Easing easeMenuEndPosX_[SSMEN_Num];
	//カーソルX値のイージング
	Easing easeCursorPosX_;
	//選んだステージを真ん中に移動させるイージング
	Easing easeStartStagePosX_[SSSMI_Num];
	//選んだステージを上に移動させるイージング
	Easing easeStartStagePosY_[SSSMI_Num];
	//遷移時すぐに起動するステージセレクト視点カメラワークイージング
	Easing easeEyeStageSelect_[XYZ_Num];
	//遷移時すぐに起動するステージセレクト注視点カメラワークイージング
	Easing easeTargetStageSelect_[XYZ_Num];
	//ステージ決定時の視点カメラワークイージング
	Easing easeEyeDoneMenu_[XYZ_Num];
	//ステージ決定時の注視点カメラワークイージング
	Easing easeTargetDoneMenu_[XYZ_Num];
	//ステージ決定時の視点カメラワークイージングが終わった後の突入視点カメラワークイージング
	Easing easeEyeGameStart_[XYZ_Num];
	//ステージ決定時の視点カメラワークイージングが終わった後の突入注視点カメラワークイージング
	Easing easeTargetGameStart_[XYZ_Num];
	//ステージ決定時の視点カメラワークイージングが終わった後の自機移動イージング
	Easing easePlayerStartMove_[XYZ_Num];
	//タイトルに戻るときの自機移動イージング
	Easing easePlayerQuitMove_[XYZ_Num];
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_;

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
