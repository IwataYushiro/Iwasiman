#pragma once
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "LevelLoaderJson.h"

#include "ParticleManager.h"
#include "Sprite.h"
#include "EnumList.h"
#include "XYZ.h"

#include <map>
#include <memory>

#include "SceneManager.h"

/*

*	TitleScene.h

*	タイトルシーン

*/
class TitleScene :public BaseScene
{
public://メンバ関数(ステージ番号)
	TitleScene(const int stagenum);
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
	void FadeIn(const DirectX::XMFLOAT3& color) override;
	//描画
	void Draw() override;
	//ポストエフェクト描画
	void DrawPostEffect() override;
	//終了
	void Finalize() override;
	//レベルデータ読み込み
	void LoadLVData(const std::string& stagePath);
	//イージングのロード
	void LoadEasing() override;
	//ポストエフェクトテスト用
	void TestPostEffect(const std::string& path);
private://基盤メンバ変数
	//DirectX基盤
	DirectXCommon* dxCommon_ = nullptr;
	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//インプット
	Input* input_ = nullptr;
	//オーディオ
	Audio* audio_ =nullptr;
	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;
	//imgui
	ImGuiManager* imguiManager_ = nullptr;


private://メンバ変数
	//タイトル用テクスチャインデックス
	enum TitleSceneTextureIndex
	{
		TSTI_TitleTex = 0,
		TSTI_MenuTex = 1,
		TSTI_MenuTutorialTex = 2,
		TSTI_MenuStageSerectTex = 3,
		TSTI_MenuDoneTex = 4,
		TSTI_BackTitleTex = 5,
		TSTI_FadeInOutTex = 6,
		TSTI_LoadingTex = 7,
		TSTI_StageInfoNowTex = 8,
		TSTI_CursorTex = 9,
		TSTI_TitleBackTex = 10,
		TSTI_StageNameTex = 11,
		TSTI_MenuUITex = 12,
	};

	
	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	//選択しているメニュー表示
	int menuCount_;

	//Sprite
	std::unique_ptr<Sprite> spriteTitle_ = std::make_unique<Sprite>();				//タイトル画面スプライト
	std::unique_ptr<Sprite> spriteMenu_ = std::make_unique<Sprite>();				//タイトルメニュー画面スプライト
	std::unique_ptr<Sprite> spriteMenuTutorial_ = std::make_unique<Sprite>();		//チュートリアル表示スプライト
	std::unique_ptr<Sprite> spriteMenuStageSelect_ = std::make_unique<Sprite>();	//ステージセレクト表示スプライト
	std::unique_ptr<Sprite> spriteMenuDone_ = std::make_unique<Sprite>();			//タイトルメニュー決定表示兼スキップキー表示スプライト
	std::unique_ptr<Sprite> spriteBack_ = std::make_unique<Sprite>();				//タイトルメニュー→タイトル移行のスプライト
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//フェードインアウトスプライト
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//ロードスプライト
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//現在ステージスプライト
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//カーソルスプライト
	std::unique_ptr<Sprite> spriteTitleBack_ = std::make_unique<Sprite>();			//タイトル画面スプライト(タイトルの後ろ)
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();			//ステージ名スプライト
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();				//メニュー操作方法スプライト

	//モデル
	std::unique_ptr<Model> modelPlayer_ = nullptr;					//自機モデル
	std::unique_ptr<Model> modelSkydome_ = nullptr;					//チュートリアルステージモデル(天球)
	std::unique_ptr<Model> modelSkydomeStage1_ = nullptr;			//ステージ1モデル(天球)
	std::unique_ptr<Model> modelSkydomeStage2_ = nullptr;			//ステージ2モデル(天球)
	std::unique_ptr<Model> modelGround_ = nullptr;					//床モデル
	std::unique_ptr<Model> modelGoal_ = nullptr;					//ゴールモデル

	std::vector<std::unique_ptr<Object3d>> objPlayers_;					//自機オブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objSkydomes_;				//天球オブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objGrounds_;					//床オブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objGoals_;					//ゴールオブジェクト配列


	//フラグ類
	bool isMenu_ = false;				//タイトルメニュー画面にいるとき
	bool isBack_ = false;				//タイトルメニューからタイトルに戻るとき
	bool isStartGame_ = false;			//ゲーム開始するとき
	bool isStageSelect_ = false;		//ステージセレクトに行くとき
	bool isFadeIn_ = false;				//フェードイン

	
	//最初の画面のY値
	const float startTitlePosY_ = 50.0f;

	//タイトル→タイトルメニューのイージング
	Easing easeTitlePosX_;


	//メニュー説明用の列挙体
	enum TitleMenuEasingNum
	{
		TMEN_Menu = 0,				//メニュー
		TMEN_Tutorial = 1,			//チュートリアルへ
		TMEN_StageSelect = 2,		//ステージセレクトへ
		TMEN_SelectSpace = 3,		//スペースで選択
		TMEN_UI = 4,				//操作方法
		TMEN_Quit = 5,				//戻る
		TMEN_Num = 6,				//配列用
	};
	//メニューのY値
	const std::array<float, TMEN_Num> menuPosY_ = { 50.0f,200.0f,350.0f,600.0f,300.0f,50.0f };
	
	//タイトルメニューの出現イージング
	Easing easeMenuPosX_[TMEN_Num];
	//タイトルメニューの通過イージング
	Easing easeMenuEndPosX_[TMEN_Num];
	//カーソルX値のイージング
	Easing easeCursorPosX_;
	//選んだステージを真ん中に移動させるイージング
	Easing easeStartStagePosX_;//チュートリアルへ
	//選んだステージを上に移動させるイージング
	Easing easeStartStagePosY_;//チュートリアルへ

	//タイトル→タイトルメニューの視点カメラワークイージング
	Easing easeEyeMenu_[XYZ_Num];
	//タイトル→タイトルメニューの注視点カメラワークイージング
	Easing easeTargetMenu_[XYZ_Num];
	//タイトルメニュー→ゲーム開始の視点カメラワークイージング
	Easing easeEyeGameStart_[XYZ_Num];
	//タイトルメニュー→ゲーム開始の注視点カメラワークイージング
	Easing easeTargetGameStart_[XYZ_Num];
	//タイトルメニュー→ステージセレクトの自機移動イージング
	Easing easePlayerMove_[XYZ_Num];
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_;

	//選択中の色
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb

	//タイトルメニュー→タイトルへ戻るスプライトの色
	const DirectX::XMFLOAT4 backTitleColor_ = { 0.0f,0.0f,0.1f,1.0f };

	//色反転フラグ
	bool isColorReverse_ = false;
	//ライト
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;//炎
	std::unique_ptr<Particle> particleGoal_ = nullptr;//ゴール
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;//炎
	std::unique_ptr<ParticleManager> pmGoal_ = nullptr;//ゴール

	//開始時のポジション
	DirectX::XMFLOAT3 startPos_;

	//どのステージにいるのかを受け取るための変数
	int stageNum_;
	//ダーティフラグ
	bool dirty_ = false;
public:
	//色が変わる処理
	void UpdateChangeColor();
	//丸影セット
	void SetUpCircleShadow(const DirectX::XMFLOAT3& pos);
};
