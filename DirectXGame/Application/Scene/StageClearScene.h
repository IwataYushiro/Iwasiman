#pragma once
#include "EnumList.h"
#include "XYZ.h"

#include <map>

#include "SceneManager.h"

//jsonレベルデータ
struct LevelData;

/*

*	StageClearScene.h

*	ステージクリアシーン

*/
class StageClearScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ番号)
	StageClearScene(const int stagenum);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//状態更新(ゲーム開始するとき)
	void UpdateIsNextStage();
	//状態更新(ステージセレクトへ遷移するとき)
	void UpdateIsStageSelect();
	//状態更新(タイトルへ戻るとき)
	void UpdateIsQuitTitle();
	//状態更新(メニューのとき) 
	void UpdateIsMenu();
	//描画
	void Draw() override;
	//フェードアウト
	void FadeIn(const DirectX::XMFLOAT3& color)override;
	//終了
	void Finalize() override;

	//レベルデータ読み込み
	void LoadLVData(const std::string& stagePath);

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
	//ステージクリア用テクスチャインデックス
	enum StageClearSceneTextureIndex
	{
		SCSTI_MenuTex = 0,
		SCSTI_NextStageTex = 1,
		SCSTI_StageSelectTex = 2,
		SCSTI_TitleTex = 3,
		SCSTI_MenuDoneTex = 4,
		SCSTI_FadeInOutTex = 5,
		SCSTI_LoadingTex = 6,
		SCSTI_StageInfoNowTex = 7,
		SCSTI_CursorTex = 8,
		SCSTI_StageNameTex = 9,
		SCSTI_MenuUITex = 10,
	};
	//ステージクリア用メニューインデックス
	enum StageClearSceneMenuIndex
	{
		SCSMI_NextStage = 0,
		SCSMI_StageSelect = 1,
		SCSMI_Title = 2,
	};

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	
	//ステージ番号
	int stageNum_;
	//メニュー番号
	int menuCount_ = SCSMI_NextStage;
	//Sprite
	std::unique_ptr<Sprite> spriteStageClear_ =std::make_unique<Sprite>();		//ステージクリア時のスプライト
	std::unique_ptr<Sprite> spriteNextStage_ =std::make_unique<Sprite>();		//次のステージ表示のスプライト
	std::unique_ptr<Sprite> spriteStageSelect_ =std::make_unique<Sprite>();		//ステージセレクト表示のスプライト
	std::unique_ptr<Sprite> spriteTitle_ =std::make_unique<Sprite>();			//タイトル表示のスプライト
	std::unique_ptr<Sprite> spriteDone_ =std::make_unique<Sprite>();			//決定表示兼スキップキー表示のスプライト
	std::unique_ptr<Sprite> spriteFadeInOut_ =std::make_unique<Sprite>();		//フェードインアウトのスプライト
	std::unique_ptr<Sprite> spriteLoad_ =std::make_unique<Sprite>();			//ロードスプライト
	std::unique_ptr<Sprite> spriteStageInfoNow_ =std::make_unique<Sprite>();	//現在ステージスプライト
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();			//カーソルスプライト
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();		//ステージ名スプライト
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();			//メニュー操作方法スプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;			

	//モデル
	std::unique_ptr<Model> modelPlayer_ = nullptr;				//自機モデル
	std::unique_ptr<Model> modelSkydome_ = nullptr;				//チュートリアルステージモデル(天球)
	std::unique_ptr<Model> modelSkydomeStage1_ = nullptr;		//ステージ1モデル(天球)
	std::unique_ptr<Model> modelSkydomeStage2_ = nullptr;		//ステージ2モデル(天球)
	std::unique_ptr<Model> modelGround_ = nullptr;				//床のモデル
	std::unique_ptr<Model> modelGoal_ = nullptr;				//ゴールモデル

	std::vector<std::unique_ptr<Object3d>> objPlayers_;			//自機配列
	std::vector<std::unique_ptr<Object3d>> objSkydomes_;		//天球配列
	std::vector<std::unique_ptr<Object3d>> objGrounds_;			//床配列
	std::vector<std::unique_ptr<Object3d>> objGoals_;			//ゴール配列

	//フラグ類
	bool isFinalStage_ = false;					//最終面の場合
	bool isNextStage_ = false;					//次のステージへ行く場合
	bool isStageSelect_ = false;				//ステージセレクトへ行く場合
	bool isQuitTitle_ = false;					//タイトルに戻る場合
	bool isFadeIn_ = false;						//フェードイン

	//メニュー説明用の列挙体
	enum StageClearMenuEasingNum
	{
		SCMEN_Menu = 0,					//メニュー
		SCMEN_NextStage = 1,			//コンティニューへ
		SCMEN_StageSelect = 2,			//ステージセレクトへ
		SCMEN_Title = 3,				//タイトルへ
		SCMEN_SelectSpace = 4,			//スペースで選択
		SCMEN_UI = 5,					//操作方法
		SCMEN_Num = 6,					//配列用
	};
	//イージング類
	//メニュー表示用のイージング
	Easing easeMenuPosX_[SCMEN_Num];
	//メニュー終了用のイージング
	Easing easeMenuEndPosX_[SCMEN_Num];
	//カーソルX値のイージング
	Easing easeCursorPosX_;

	//メニューポジション
	const std::array<float, SCMEN_Num> menuPosY_ = { 50.0f,150.0f,300.0f,450.0f,550.0f,300.0f };

	//次のステージへ行くときの視点イージング
	Easing easeEyeStageClear_[XYZ_Num];
	//次のステージへ行くときの注視点イージング
	Easing easeTargetStageClear_[XYZ_Num];
	//ステージセレクトへ行くときの自機移動イージング
	Easing easePlayerMoveStageSelect_[XYZ_Num];
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_;

	//選択中の色
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb

	//色反転フラグ
	bool isColorReverse_ = false;
	//ライト
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//パーティクル
	std::unique_ptr<Particle> particleClear_ = nullptr;	//クリア演出(ゴールエフェクトにも使いまわす)
	std::unique_ptr<Particle> particleFire_ = nullptr;	//炎
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmClear_ = nullptr;	//クリア演出(ゴールエフェクトにも使いまわす)
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;		//炎

	//開始時のポジション
	DirectX::XMFLOAT3 startPos_;

private:
	//色が変わる処理
	void UpdateChangeColor();
	//ステージ名のスプライトをロード
	void LoadStageNameSprite();
	//丸影セット
	void SetUpCircleShadow(const DirectX::XMFLOAT3& pos);
};
