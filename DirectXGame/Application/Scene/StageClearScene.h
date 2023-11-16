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

*	StageClearScene.h

*	ステージクリアシーン

*/
class StageClearScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ番号)
	StageClearScene(int stagenum);
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
	void FadeOut(DirectX::XMFLOAT3 rgb);
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
	};
	//ステージクリア用メニューインデックス
	enum StageClearSceneMenuIndex
	{
		SCSMI_NextStage = 0,
		SCSMI_StageSelect = 1,
		SCSMI_Title = 2,
	};

	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
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
	std::unique_ptr<Sprite> spriteDone_ =std::make_unique<Sprite>();			//決定表示のスプライト
	std::unique_ptr<Sprite> spriteFadeInOut_ =std::make_unique<Sprite>();		//フェードインアウトのスプライト
	std::unique_ptr<Sprite> spriteLoad_ =std::make_unique<Sprite>();			//ロードスプライト
	std::unique_ptr<Sprite> spriteStageInfoNow_ =std::make_unique<Sprite>();	//現在ステージスプライト

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

	//マッピングモデル
	std::map<std::string, Model*> models_;

	//フラグ類
	bool isFinalStage_ = false;					//最終面の場合
	bool isNextStage_ = false;					//次のステージへ行く場合
	bool isStageSelect_ = false;				//ステージセレクトへ行く場合
	bool isQuitTitle_ = false;					//タイトルに戻る場合
	bool isFadeOut_ = false;					//フェードインアウト

	//メニュー説明用の列挙体
	enum StageClearMenuEasingNum
	{
		SCMEN_Menu = 0,					//メニュー
		SCMEN_NextStage = 1,			//コンティニューへ
		SCMEN_StageSelect = 2,			//ステージセレクトへ
		SCMEN_Title = 3,				//タイトルへ
		SCMEN_SelectSpace = 4,			//スペースで選択
		SCMEN_Num = 5,					//配列用
	};
	//イージング類
	//メニュー表示用のイージングのプリセット
	const Easing presetEaseMenuPosX_[SCMEN_Num]
	{
		{1300.0f, 200.0f, 1.0f},			//メニュー
		{1300.0f, 0.0f, 1.2f},				//次のステージへ
		{1300.0f, 0.0f, 1.4f},				//ステージセレクトへ
		{1300.0f, 0.0f, 1.6f},				//タイトルへ
		{1300.0f, 425.0f, 2.0f}				//スペースで選択
	};
	//メニュー表示用のイージング
	Easing easeMenuPosX_[SCMEN_Num]
	{
		presetEaseMenuPosX_[SCMEN_Menu],				//メニュー
		presetEaseMenuPosX_[SCMEN_NextStage],			//次のステージへ
		presetEaseMenuPosX_[SCMEN_StageSelect],			//ステージセレクトへ
		presetEaseMenuPosX_[SCMEN_Title],				//タイトルへ
		presetEaseMenuPosX_[SCMEN_SelectSpace],			//スペースで選択
	};
	//メニューポジション
	const std::array<float, SCMEN_Num> menuPosY_ = { 50.0f,150.0f,300.0f,450.0f,550.0f };

	//次のステージへ行くときの視点イージングのプリセット
	const Easing presetEaseEyeStageClear_[XYZ_Num]
	{
		{0.0f, -22.0f, 1.8f},				//X
		{1.0f, -1.0f, 1.8f},				//Y
		{-110.0f, -60.0f, 1.8f},			//Z
	};
	//次のステージへ行くときの視点イージング
	Easing easeEyeStageClear_[XYZ_Num]
	{
		presetEaseEyeStageClear_[XYZ_X],			//X
		presetEaseEyeStageClear_[XYZ_Y],			//Y
		presetEaseEyeStageClear_[XYZ_Z]				//Z
	};

	//次のステージへ行くときの注視点イージングのプリセット
	const Easing presetEaseTargetStageClear_[XYZ_Num]
	{
		{0.0f, 50.0f, 1.8f},				//X
		{0.0f, -8.0f, 1.8f},				//Y
		{-10.0f, -57.0f, 1.8f},				//Z
	};
	//次のステージへ行くときの注視点イージング
	Easing easeTargetStageClear_[XYZ_Num]
	{
		 presetEaseTargetStageClear_[XYZ_X],			//X
		 presetEaseTargetStageClear_[XYZ_Y],			//Y
		 presetEaseTargetStageClear_[XYZ_Z]				//Z
	};

	//ステージセレクトへ行くときの自機移動イージングのプリセット
	const Easing presetEasePlayerMoveStageSelect_[XYZ_Num]
	{
		{0.0f, 150.0f, 2.0f},				//X
		{-8.0f, 40.0f, 2.0f},				//Y
		{-60.0f, -60.0f, 2.0f},				//Z
	};
	//ステージセレクトへ行くときの自機移動イージング
	Easing easePlayerMoveStageSelect_[XYZ_Num]
	{
		presetEasePlayerMoveStageSelect_[XYZ_X],			//X
		presetEasePlayerMoveStageSelect_[XYZ_Y],			//Y
		presetEasePlayerMoveStageSelect_[XYZ_Z],			//Z
	};

	//フェードインアウトのプリセット
	const Easing presetEaseFadeInOut_ = { 1.0f, 0.0f, 1.0f };
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;

	//選択中の色
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb

	//色反転フラグ
	bool isColorReverse_ = false;
	//ライト
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//パーティクル
	std::unique_ptr<Particle> particleClear_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmClear_ = nullptr;
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;

	//開始時のポジション
	DirectX::XMFLOAT3 startPos_;

public:
	//色が変わる処理
	void UpdateChangeColor();

};
