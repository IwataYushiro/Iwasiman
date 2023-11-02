#pragma once
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"
#include "XYZ.h"
#include "EnumList.h"

#include <DirectXMath.h>
#include <map>

#include "SceneManager.h"

//前方宣言
//jsonレベルデータ
struct LevelData;

/*

*	GameOverScene.h

*	ゲームオーバーシーン

*/
class GameOverScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ情報)
	GameOverScene(int stagenum);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//ゲームオーバー中
	void UpdateIsGameOver();
	//コンティニュー時
	void UpdateIsContinue();
	//ステージセレクト遷移時
	void UpdateIsQuitStageSelect();
	//タイトル遷移時
	void UpdateIsQuitTitle();
	//描画
	void Draw() override;
	//終了
	void Finalize() override;
	//レベルデータ読み込み(ステージファイルパス)
	void LoadLVData(const std::string& stagePath);
	//フェードアウト
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//色が変わる処理
	void UpdateChangeColor();
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

	//ゲームオーバー用テクスチャインデックス
	enum GameOverSceneTextureIndex
	{
		GOSTI_MenuTex = 0,
		GOSTI_ContinueTex = 1,
		GOSTI_StageSelectTex = 2,
		GOSTI_TitleTex = 3,
		GOSTI_MenuDoneTex = 4,
		GOSTI_FadeInOutTex = 5,
		GOSTI_LoadingTex = 6,
		GOSTI_StageInfoNowTex = 7,
	};

	//ゲームオーバー用メニューインデックス
	enum GameOverSceneMenuIndex
	{
		GOSMI_Continue = 0,
		GOSMI_StageSelect = 1,
		GOSMI_Title = 2,
	};
	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//ステージ番号
	int stageNum_;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();			//ゲームオーバースプライト
	Sprite* spriteContinue_ = new Sprite();			//コンティニュー表示スプライト
	Sprite* spriteStageSelect_ = new Sprite();		//ステージセレクト表示スプライト
	Sprite* spriteTitle_ = new Sprite();			//タイトル表示スプライト
	Sprite* spriteDone_ = new Sprite();				//決定スプライト
	Sprite* spriteFadeInOut_ = new Sprite();		//フェードインアウトスプライト
	Sprite* spriteLoad_ = new Sprite();				//ロードスプライト
	Sprite* spriteStageInfoNow_ = new Sprite();		//現在ステージスプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//モデル
	Model* modelPlayer_ = nullptr;				//自機モデル
	Model* modelGoal_ = nullptr;				//ゴールモデル
	Model* modelStageTutorial_ = nullptr;		//チュートリアルステージモデル(天球)
	Model* modelStage1_ = nullptr;				//ステージ1モデル(天球)
	Model* modelStage2_ = nullptr;				//ステージ2モデル(天球)

	std::vector<Object3d*> objPlayers_;			//自機オブジェクト配列
	std::vector<Object3d*> objGoals_;			//ゴールオブジェクト配列
	std::vector<Object3d*> objStages_;			//ステージオブジェクト配列

	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_;

	//マッピングモデル
	std::map<std::string, Model*> models_;
	//オブジェクト配列
	std::vector<Object3d*> objects_;

	//フラグ類
	bool isGameover_ = true;					//メニュー時フラグ
	bool isContinue_ = false;					//コンティニューする場合
	bool isQuitStageSelect_ = false;			//ステージセレクトに戻る場合
	bool completeRotate_ = false;				//回り終わった時 
	bool isQuitTitle_ = false;					//タイトルに戻る場合
	bool isFadeOut_ = false;					//フェードインアウト

	//メニュー説明用の列挙体
	enum GameOverMenuEasingNum
	{
		GOMEN_Menu = 0,				//メニュー
		GOMEN_Continue = 1,			//コンティニューへ
		GOMEN_StageSelect = 2,		//ステージセレクトへ
		GOMEN_Title = 3,			//タイトルへ
		GOMEN_SelectSpace = 4,		//スペースで選択
		GOMEN_Num = 5,				//配列用
	};
	//メニューポジション
	const std::array<float,GOMEN_Num> menuPosY_= { 0.0f,150.0f,300.0f,450.0f,550.0f };

	//メニュー表示用のイージングのプリセット
	const Easing presetEaseMenuPosX_[GOMEN_Num]
	{
		{1300.0f, 0.0f, 1.0f},			//メニュー
		{1300.0f, 0.0f, 1.2f},			//コンティニューへ
		{1300.0f, 0.0f, 1.4f},			//ステージセレクトへ
		{1300.0f, 0.0f, 1.6f},			//タイトルへ
		{1300.0f, 0.0f, 1.8f}				//スペースで選択
	};
	//メニュー表示用のイージング
	Easing easeMenuPosX_[GOMEN_Num]
	{
		presetEaseMenuPosX_[GOMEN_Menu],				//メニュー
		presetEaseMenuPosX_[GOMEN_Continue],			//コンティニューへ
		presetEaseMenuPosX_[GOMEN_StageSelect],			//ステージセレクトへ
		presetEaseMenuPosX_[GOMEN_Title],				//タイトルへ
		presetEaseMenuPosX_[GOMEN_SelectSpace],			//スペースで選択
	};
	
	//コンティニュー文字を真ん中に移動させるイージングのプリセット
	const Easing presetEaseContinuePosX_ = Easing(0.0f, 380.0f, 1.0f);
	//コンティニュー文字を真ん中に移動させるイージング
	Easing easeContinuePosX_ = presetEaseContinuePosX_;

	//コンティニュー文字を上に移動させるイージングのプリセット
	const Easing presetEaseContinuePosY_ = Easing(150.0f, 0.0f, 1.0f);
	//コンティニュー文字を上に移動させるイージング
	Easing easeContinuePosY_ = presetEaseContinuePosY_;

	//コンティニューするときの視点イージングのプリセット
	const Easing presetEaseEyeContinue_[XYZ_Num]
	{
		{-6.0f, -21.0f, 1.0f},				//X
		{-8.0f, -5.0f, 1.0f},				//Y
		{-110.0f, -60.0f, 1.0f}				//Z
	};
	//コンティニューするときの視点イージング
	Easing easeEyeContinue_[XYZ_Num]
	{
		presetEaseEyeContinue_[XYZ_X],				//X
		presetEaseEyeContinue_[XYZ_Y],				//Y
		presetEaseEyeContinue_[XYZ_Z]				//Z
	};

	//コンティニューするときの注視点イージングのプリセット
	const Easing presetEaseTargetContinue_[XYZ_Num]
	{
		{-32.0f, 90.0f, 1.0f},				//X
		{-24.0f, -22.0f, 1.0f},				//Y
		{-10.0f, -61.0f, 1.0f}				//Z
	};

	//コンティニューするときの注視点イージング
	Easing easeTargetContinue_[XYZ_Num]
	{
		presetEaseTargetContinue_[XYZ_X],				//X
		presetEaseTargetContinue_[XYZ_Y],				//Y
		presetEaseTargetContinue_[XYZ_Z]				//Z
	};

	//コンティニューするときの自機回転イージングのプリセット
	const Easing presetEasePlayerRotateContinue_[XYZ_Num]
	{
		{90.0f, 0.0f, 1.0f},				//X
		{-90.0f, 90.0f, 1.0f},				//Y
		{0.0f, 0.0f, 1.0f}					//Z
	};
	//コンティニューするときの自機回転イージング
	Easing easePlayerRotateContinue_[XYZ_Num]
	{
		presetEasePlayerRotateContinue_[XYZ_X],				//X
		presetEasePlayerRotateContinue_[XYZ_Y],				//Y
		presetEasePlayerRotateContinue_[XYZ_Z]				//Z
	};

	//コンティニューするときの自機移動イージングのプリセット
	const Easing presetEasePlayerMoveContinue_[XYZ_Num]
	{
		{0.0f, 90.0f, 1.0f},				//X
		{-8.0f, -8.0f, 1.0f},				//Y
		{-60.0f, -60.0f, 1.0f}				//Z
	};
	//コンティニューするときの自機移動イージング
	Easing easePlayerMoveContinue_[XYZ_Num]
	{
		presetEasePlayerMoveContinue_[XYZ_X],				//X
		presetEasePlayerMoveContinue_[XYZ_Y],				//Y
		presetEasePlayerMoveContinue_[XYZ_Z]				//Z
	};

	//ステージセレクトへ遷移するときの視点イージングのプリセット
	const Easing presetEaseEyeQuitStageSelect_[XYZ_Num]
	{
		{-6.0f, -21.0f, 1.0f},				//X
		{-8.0f, -5.0f, 1.0f},				//Y
		{-110.0f, -60.0f, 1.0f}				//Z
	};
	//ステージセレクトへ遷移するときの視点イージング
	Easing easeEyeQuitStageSelect_[XYZ_Num]
	{
		presetEaseEyeQuitStageSelect_[XYZ_X],				//X
		presetEaseEyeQuitStageSelect_[XYZ_Y],				//Y
		presetEaseEyeQuitStageSelect_[XYZ_Z]				//Z
	};

	//ステージセレクトへ遷移するときの注視点イージングのプリセット
	const Easing presetEaseTargetQuitStageSelect_[XYZ_Num]
	{
		{-32.0f, 90.0f, 1.0f},				//X
		{-24.0f, -22.0f, 1.0f},				//Y
		{-10.0f, -61.0f, 1.0f}				//Z
	};
	//ステージセレクトへ遷移するときの注視点イージング
	Easing easeTargetQuitStageSelect_[XYZ_Num]
	{
		presetEaseTargetQuitStageSelect_[XYZ_X],				//X
		presetEaseTargetQuitStageSelect_[XYZ_Y],				//Y
		presetEaseTargetQuitStageSelect_[XYZ_Z]					//Z
	};

	//ステージセレクトへ遷移するときの自機回転イージングのプリセット
	const Easing presetEasePlayerRotateQuitStageSelect_[XYZ_Num]
	{
		{90.0f, 0.0f, 1.0f},				//X
		{-90.0f, 90.0f, 1.0f},				//Y
		{0.0f, 0.0f, 1.0f}					//Z
	};
	//ステージセレクトへ遷移するときの自機回転イージング
	Easing easePlayerRotateQuitStageSelect_[XYZ_Num]
	{
		presetEasePlayerRotateQuitStageSelect_[XYZ_X],				//X
		presetEasePlayerRotateQuitStageSelect_[XYZ_Y],				//Y
		presetEasePlayerRotateQuitStageSelect_[XYZ_Z]				//Z
	};

	//ステージセレクトへ遷移するときの自機移動イージングのプリセット
	const Easing presetEasePlayerMoveQuitStageSelect_[XYZ_Num]
	{
		{0.0f, 150.0f, 1.0f},					//X
		{-8.0f, 20.0f, 1.0f},					//Y
		{-60.0f, -60.0f, 1.0f}					//Z
	};
	//ステージセレクトへ遷移するときの自機移動イージング
	Easing easePlayerMoveQuitStageSelect_[XYZ_Num]
	{
		presetEasePlayerMoveQuitStageSelect_[XYZ_X],					//X
		presetEasePlayerMoveQuitStageSelect_[XYZ_Y],					//Y
		presetEasePlayerMoveQuitStageSelect_[XYZ_Z]						//Z
	};

	//フェードインアウトのプリセット
	const Easing presetEaseFadeInOut_ = { 1.0f, 0.0f, 1.0f };
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;

	//選択中の色
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb
	//選択しているメニュー表示
	int menuCount_ = GOSMI_Continue;
	//色反転フラグ
	bool isColorReverse_ = false;

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pm1_ = nullptr;

};