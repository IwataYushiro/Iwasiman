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
#include "EnumList.h"
#include "XYZ.h"

#include <map>

#include "SceneManager.h"

//jsonレベルデータ
struct LevelData;

/*

*	TitleScene.h

*	タイトルシーン

*/
class TitleScene :public BaseScene
{
public://メンバ関数(ステージ番号)
	TitleScene(int stagenum);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//状態更新(ゲーム開始するとき)
	void UpdateIsStartGame();
	//状態更新(ステージセレクトへ遷移するとき)
	void UpdateIsStageSelect();
	//状態更新(スタートに戻ったとき)
	void UpdateIsBack();
	//状態更新(メニューのとき) 
	void UpdateIsMenu();
	//フェードアウト(色)
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//描画
	void Draw() override;
	//終了
	void Finalize() override;

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
	//タイトル用テクスチャインデックス
	enum TitleSceneTextureIndex
	{
		TSTI_TitleTex = 0,
		TSTI_TitleDoneTex = 1,
		TSTI_MenuTex = 2,
		TSTI_MenuTutorialTex = 3,
		TSTI_MenuStageSerectTex = 4,
		TSTI_MenuDoneTex = 5,
		TSTI_BackTitleTex = 6,
		TSTI_FadeInOutTex = 7,
		TSTI_LoadingTex = 8,
		TSTI_StageInfoNowTex = 9,
	};

	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//選択しているメニュー表示
	int menuCount_;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();				//タイトル画面スプライト
	Sprite* spriteTitleDone_ = new Sprite();			//タイトル決定表示スプライト
	Sprite* spriteMenu_ = new Sprite();					//タイトルメニュー画面スプライト
	Sprite* spriteMenuTutorial_ = new Sprite();			//チュートリアル表示スプライト
	Sprite* spriteMenuStageSelect_ = new Sprite();		//ステージセレクト表示スプライト
	Sprite* spriteMenuDone_ = new Sprite();				//タイトルメニュー決定表示スプライト
	Sprite* spriteBack_ = new Sprite();					//タイトルメニュー→タイトル移行のスプライト
	Sprite* spriteFadeInOut_ = new Sprite();			//フェードインアウトスプライト
	Sprite* spriteLoad_ = new Sprite();					//ロードスプライト
	Sprite* spriteStageInfoNow_ = new Sprite();			//現在ステージスプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//モデル
	Model* modelPlayer_ = nullptr;						//自機モデル
	Model* modelSkydome_ = nullptr;						//チュートリアルステージモデル(天球)
	Model* modelSkydomeStage1_ = nullptr;				//ステージ1モデル(天球)
	Model* modelSkydomeStage2_ = nullptr;				//ステージ2モデル(天球)
	Model* modelGround_ = nullptr;						//床モデル
	Model* modelGoal_ = nullptr;						//ゴールモデル

	std::vector<Object3d*> objPlayers_;					//自機オブジェクト配列
	std::vector<Object3d*> objSkydomes_;				//天球オブジェクト配列
	std::vector<Object3d*> objGrounds_;					//床オブジェクト配列
	std::vector<Object3d*> objGoals_;					//ゴールオブジェクト配列

	//マッピングモデル
	std::map<std::string, Model*> models_;

	//フラグ類
	bool isMenu_ = false;				//タイトルメニュー画面にいるとき
	bool isBack_ = false;				//タイトルメニューからタイトルに戻るとき
	bool isStartGame_ = false;			//ゲーム開始するとき
	bool isStageSelect_ = false;		//ステージセレクトに行くとき
	bool isFadeOut_ = false;			//フェードアウト

	//タイトル→タイトルメニューの列挙体
	enum TitleStart
	{
		TS_Title=0,		//タイトル
		TS_Done=1,		//決定
		TS_Num=2		//配列用
	};
	//最初の画面のY値
	const std::array<float, TS_Num> startTitlePosY_ = { 50.0f,600.0f };

	//タイトル→タイトルメニューのイージングのプリセット
	const Easing presetEaseTitlePosX_[TS_Num] =
	{
		{225.0f, -1300.0f, 1.0f},
		{300.0f, -1300.0f, 1.0f}
	};
	//タイトル→タイトルメニューのイージング
	Easing easeTitlePosX_[TS_Num] =
	{
		presetEaseTitlePosX_[TS_Title],
		presetEaseTitlePosX_[TS_Done]
	};

	//メニュー説明用の列挙体
	enum TitleMenuEasingNum
	{
		TMEN_Menu = 0,				//メニュー
		TMEN_Tutorial = 1,			//チュートリアルへ
		TMEN_StageSelect = 2,		//ステージセレクトへ
		TMEN_SelectSpace = 3,		//スペースで選択
		TMEN_Quit = 4,				//戻る
		TMEN_Num = 5,				//配列用
	};
	//メニューのY値
	const std::array<float, TMEN_Num> menuPosY_ = { 0.0f,150.0f,300.0f,550.0f,50.0f };

	//タイトルメニューの出現イージングのプリセット
	const Easing presetEaseMenuPosX_[TMEN_Num] =
	{
		{1300.0f, 0.0f, 1.0f},		//メニュー
		{1300.0f, 50.0f, 1.2f},		//チュートリアルへ
		{1300.0f, 50.0f, 1.4f},		//ステージセレクトへ
		{1300.0f, 0.0f, 1.6f},		//スペースで選択
		{1300.0f, 900.0f, 1.8f}		//戻る
	};
	//タイトルメニューの出現イージング
	Easing easeMenuPosX_[TMEN_Num] =
	{
		presetEaseMenuPosX_[TMEN_Menu],			//メニュー
		presetEaseMenuPosX_[TMEN_Tutorial],		//チュートリアルへ
		presetEaseMenuPosX_[TMEN_StageSelect],	//ステージセレクトへ
		presetEaseMenuPosX_[TMEN_SelectSpace],	//スペースで選択
		presetEaseMenuPosX_[TMEN_Quit]			//戻る
	};

	//選んだステージを真ん中に移動させるイージングのプリセット
	const Easing presetEaseStartStagePosX_ = {0.0f, 350.0f, 1.5f};//チュートリアルへ
	//選んだステージを真ん中に移動させるイージング
	Easing easeStartStagePosX_ = presetEaseStartStagePosX_;//チュートリアルへ


	//選んだステージを上に移動させるイージングのプリセット
	const Easing presetEaseStartStagePosY_ = { menuPosY_[TMEN_Tutorial], 0.0f, 1.5f};//チュートリアルへ
	//選んだステージを上に移動させるイージング
	Easing easeStartStagePosY_ = presetEaseStartStagePosY_;//チュートリアルへ

	//タイトル→タイトルメニューの視点カメラワークイージングのプリセット
	const Easing presetEaseEyeMenu_[XYZ_Num] =
	{
		{0.0f, 21.0f, 1.8f},			//X
		{1.0f, -4.0f, 1.8f},			//Y
		{-110.0f, -60.0f, 1.8f}			//Z
	};
	//タイトル→タイトルメニューの視点カメラワークイージング
	Easing easeEyeMenu_[XYZ_Num] =
	{
		presetEaseEyeMenu_[XYZ_X],		//X
		presetEaseEyeMenu_[XYZ_Y],		//Y
		presetEaseEyeMenu_[XYZ_Z]		//Z
	};

	//タイトル→タイトルメニューの注視点カメラワークイージングのプリセット
	const Easing presetEaseTargetMenu_[XYZ_Num] =
	{
		{0.0f, -100.0f, 1.8f},			//X
		{0.0f, -10.0f, 1.8f},			//Y
		{-10.0f, -62.0f, 1.8f}			//Z
	};
	//タイトル→タイトルメニューの注視点カメラワークイージング
	Easing easeTargetMenu_[XYZ_Num] =
	{
		presetEaseTargetMenu_[XYZ_X],			//X
		presetEaseTargetMenu_[XYZ_Y],			//Y
		presetEaseTargetMenu_[XYZ_Z]			//Z
	};

	//タイトルメニュー→ゲーム開始の視点カメラワークイージングのプリセット
	const Easing presetEaseEyeGameStart_[XYZ_Num] =
	{
		{21.0f, -22.0f, 1.0f},			//X
		{-4.0f, -1.0f, 1.0f},			//Y
		{-60.0f, -60.0f, 1.0f}			//Z
	};
	//タイトルメニュー→ゲーム開始の視点カメラワークイージング
	Easing easeEyeGameStart_[XYZ_Num] =
	{
		presetEaseEyeGameStart_[XYZ_X],			//X
		presetEaseEyeGameStart_[XYZ_Y],			//Y
		presetEaseEyeGameStart_[XYZ_Z]			//Z
	};

	//タイトルメニュー→ゲーム開始の注視点カメラワークイージングのプリセット
	const Easing presetEaseTargetGameStart_[XYZ_Num] =
	{
		{-100.0f, 50.0f, 1.0f},			//X
		{-10.0f, -8.0f, 1.0f},			//Y
		{-62.0f, -57.0f, 1.0f}			//Z
	};
	//タイトルメニュー→ゲーム開始の注視点カメラワークイージング
	Easing easeTargetGameStart_[XYZ_Num] =
	{
		presetEaseTargetGameStart_[XYZ_X],			//X
		presetEaseTargetGameStart_[XYZ_Y],			//Y
		presetEaseTargetGameStart_[XYZ_Z]			//Z
	};

	//タイトルメニュー→ステージセレクトの自機移動イージングのプリセット
	const Easing presetEasePlayerMove_[XYZ_Num] =
	{
		{0.0f, 150.0f, 2.0f},			//X
		{-8.0f, 40.0f, 2.0f},			//Y
		{-60.0f, -60.0f, 2.0f}			//Z
	};
	//タイトルメニュー→ステージセレクトの自機移動イージング
	Easing easePlayerMove_[XYZ_Num] =
	{
		presetEasePlayerMove_[XYZ_X],			//X
		presetEasePlayerMove_[XYZ_Y],			//Y
		presetEasePlayerMove_[XYZ_Z]			//Z
	};
	//フェードインアウトのプリセット
	const Easing presetEaseFadeInOut_ = { 1.0f, 0.0f, 1.0f };
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;

	//選択中の色
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb
	//選択されていない色
	const DirectX::XMFLOAT4 otherMenuColor_ = { 0.0f,0.0f,0.0f,1.0f };
	//タイトルメニュー→タイトルへ戻るスプライトの色
	const DirectX::XMFLOAT4 backTitleColor_ = { 0.0f,0.0f,0.1f,1.0f };

	//色反転フラグ
	bool isColorReverse_ = false;
	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	Particle* particle2_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pm1_ = nullptr;
	ParticleManager* pm2_ = nullptr;
	
	//開始時のポジション
	DirectX::XMFLOAT3 startPos_;
	
	//どのステージにいるのかを受け取るための変数
	int stageNum_;

public:
	//色が変わる処理
	void UpdateChangeColor();

};
