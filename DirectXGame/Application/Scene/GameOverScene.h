#pragma once

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
	GameOverScene(const int stagenum);
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
	//ポストエフェクト描画
	void DrawPostEffect() override;
	//終了
	void Finalize() override;
	//レベルデータ読み込み(ステージファイルパス)
	void LoadLVData(const std::string& stagePath);
	//フェードアウト
	void FadeIn(const DirectX::XMFLOAT3& color)override;
	//イージングのロード
	void LoadEasing() override;
	//色が変わる処理
	void UpdateChangeColor();
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
		GOSTI_CursorTex = 8,
		GOSTI_StageNameTex = 9,
		GOSTI_MenuUITex = 10,
	};

	//ゲームオーバー用メニューインデックス
	enum GameOverSceneMenuIndex
	{
		GOSMI_Continue = 0,
		GOSMI_StageSelect = 1,
		GOSMI_Title = 2,
	};
	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	//ステージ番号
	int32_t stageNum_;
	//Sprite
	std::unique_ptr<Sprite> spriteGameOver_ = std::make_unique<Sprite>();			//ゲームオーバースプライト
	std::unique_ptr<Sprite> spriteContinue_ = std::make_unique<Sprite>();			//コンティニュー表示スプライト
	std::unique_ptr<Sprite> spriteStageSelect_ = std::make_unique<Sprite>();		//ステージセレクト表示スプライト
	std::unique_ptr<Sprite> spriteTitle_ = std::make_unique<Sprite>();				//タイトル表示スプライト
	std::unique_ptr<Sprite> spriteDone_ = std::make_unique<Sprite>();				//決定兼スキップキースプライト
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//フェードインアウトスプライト
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//ロードスプライト
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//現在ステージスプライト
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//カーソルスプライト
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();			//ステージ名スプライト
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();				//メニュー操作方法スプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//モデル
	std::unique_ptr<Model> modelPlayer_ = nullptr;				//自機モデル
	std::unique_ptr<Model> modelPlayerContinue_ = nullptr;		//自機モデル（コンティニュー）
	std::unique_ptr<Model> modelGoal_ = nullptr;				//ゴールモデル
	std::unique_ptr<Model> modelStageTutorial_ = nullptr;		//チュートリアルステージモデル(天球)
	std::unique_ptr<Model> modelStage1_ = nullptr;				//ステージ1モデル(天球)
	std::unique_ptr<Model> modelStage2_ = nullptr;				//ステージ2モデル(天球)

	std::vector<std::unique_ptr<Object3d>> objPlayers_;			//自機オブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objGoals_;			//ゴールオブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objStages_;			//ステージオブジェクト配列

	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_;

	//オブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objects_;

	//フラグ類
	bool isGameover_ = true;					//メニュー時フラグ
	bool isContinue_ = false;					//コンティニューする場合
	bool isQuitStageSelect_ = false;			//ステージセレクトに戻る場合
	bool completeRotate_ = false;				//回り終わった時 
	bool isQuitTitle_ = false;					//タイトルに戻る場合
	bool isFadeIn_ = false;						//フェードイン

	//メニュー説明用の列挙体
	enum GameOverMenuEasingNum
	{
		GOMEN_Menu = 0,				//メニュー
		GOMEN_Continue = 1,			//コンティニューへ
		GOMEN_StageSelect = 2,		//ステージセレクトへ
		GOMEN_Title = 3,			//タイトルへ
		GOMEN_SelectSpace = 4,		//スペースで選択
		GOMEN_UI = 5,				//操作方法
		GOMEN_Num = 6,				//配列用
	};
	//メニューポジション
	const std::array<float, GOMEN_Num> menuPosY_ = { 50.0f,150.0f,300.0f,450.0f,600.0f,300.0f };

	//メニュー表示用のイージング
	Easing easeMenuPosX_[GOMEN_Num];
	//メニュー終了用のイージング
	Easing easeMenuEndPosX_[GOMEN_Num];
	//カーソルX値のイージング
	Easing easeCursorPosX_;
	//コンティニュー文字を真ん中に移動させるイージング
	Easing easeContinuePosX_;
	//コンティニュー文字を上に移動させるイージング
	Easing easeContinuePosY_;
	//コンティニューするときの視点イージング
	Easing easeEyeContinue_[XYZ_Num];
	//コンティニューするときの注視点イージング
	Easing easeTargetContinue_[XYZ_Num];
	//コンティニューするときの自機回転イージング
	Easing easePlayerRotateContinue_[XYZ_Num];
	//コンティニューするときの自機移動イージング
	Easing easePlayerMoveContinue_[XYZ_Num];
	//ステージセレクトへ遷移するときの視点イージング
	Easing easeEyeQuitStageSelect_[XYZ_Num];
	//ステージセレクトへ遷移するときの注視点イージング
	Easing easeTargetQuitStageSelect_[XYZ_Num];
	//ステージセレクトへ遷移するときの自機回転イージング
	Easing easePlayerRotateQuitStageSelect_[XYZ_Num];
	//ステージセレクトへ遷移するときの自機移動イージング
	Easing easePlayerMoveQuitStageSelect_[XYZ_Num];
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_;

	//選択中の色
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb
	//選択しているメニュー表示
	int menuCount_ = GOSMI_Continue;
	//色反転フラグ
	bool isColorReverse_ = false;

	//ライト
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;		//炎
	std::unique_ptr<Particle> particleFall_ = nullptr;		//落下風のパーティクル
	std::unique_ptr<Particle> particleSmoke_ = nullptr;		//煙
	std::unique_ptr<Particle> particleGoal_ = nullptr;		//ゴール演出
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;		//炎(落下風のパーティクルにも使用)
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;	//煙
	std::unique_ptr<ParticleManager> pmGoal_ = nullptr;		//ゴール演出

private:
	//落ちてるときのパーティクル(プレイヤー)
	void FallParticle(const std::unique_ptr<Object3d>& player);
	//回転イージングのセット
	void EaseRotateSetUp(const DirectX::XMFLOAT3& rotation, Easing& easing, const int32_t num);

	//ステージ名のスプライトをロード
	void LoadStageNameSprite();
};