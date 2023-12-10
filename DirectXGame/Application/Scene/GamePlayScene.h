#pragma once
#include "Audio.h"
#include "Camera.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"

#include "Goal.h"
#include "Player.h"
#include "EnemyFactory.h"
#include "GimmickFactory.h"

#include "SceneManager.h"
#include "CollisionPrimitive.h"
#include "Easing.h"

#include "Item.h"
#include "EnumList.h"
#include "XYZ.h"

#include <vector>
#include <map>
#include <sstream>
#include <string>

//jsonレベルデータ
struct LevelData;

class CollisionManager;
class TouchableObject;

/*

*	GamyPlayScene.h

*	ゲームプレイシーン

*/
class GamePlayScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ情報)
	GamePlayScene(const int stagenum);

	//初期化
	void Initialize()override;
	//更新
	void Update() override;
	//状態更新(ゲーム開始時)
	void UpdateIsStartGame();
	//状態更新(ゲームプレイ時)
	void UpdateIsPlayGame();
	//状態更新(ポーズ画面時)
	void UpdateIsPause();
	//状態更新(遊び方説明時)
	void UpdateHowToPlay();
	//状態更新(ステージクリア時)
	void UpdateIsStageClear();
	//状態更新(ゲームオーバー時)
	void UpdateIsGameOver();
	//状態更新(ゲーム終了時)
	void UpdateIsQuitGame();
	//チュートリアル更新
	void UpdateTutorial();
	
	//フェードアウト(色)
	void FadeOut(const DirectX::XMFLOAT3& rgb);
	//フェードイン(色)
	void FadeIn(const DirectX::XMFLOAT3& rgb);
	//描画
	void Draw() override;
	//終了
	void Finalize() override;

	//レベルデータ読み込み(ステージファイルパス)
	void LoadLVData(const std::string& stagePath);

	//色が変わる処理(色)
	void UpdateChangeColor();

public:
	//自機弾追加(自機の弾)
	void AddPlayerBullet(const std::unique_ptr<PlayerBullet> playerBullet);
	//敵弾追加(敵の弾)
	void AddEnemyBullet(const std::unique_ptr<EnemyBullet> enemyBullet);

private://静的メンバ変数

	//DirectX基盤
	static DirectXCommon* dxCommon_;
	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//インプット
	static Input* input_;
	//オーディオ
	static Audio* audio_;
	//シーンマネージャー
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;

private:

	//サウンド読み込み
	Audio::SoundData sound_;

	//スプライト	
	std::unique_ptr<Sprite> spritePause_ = std::make_unique<Sprite>();				//ポーズ時のスプライト
	std::unique_ptr<Sprite> spritePauseInfo_ = std::make_unique<Sprite>();			//どのキーでポーズするのかを書いたスプライト
	std::unique_ptr<Sprite> spritePauseResume_ = std::make_unique<Sprite>();		//ポーズ時にゲーム再開するかを書いたスプライト
	std::unique_ptr<Sprite> spritePauseHowToPlay_ = std::make_unique<Sprite>();		//ポーズ時に遊び方を確認するかを書いたスプライト
	std::unique_ptr<Sprite> spritePauseStageSelect_ = std::make_unique<Sprite>();	//ポーズ時にステージセレクトへ戻るかを書いたスプライト
	std::unique_ptr<Sprite> spritePauseTitle_ = std::make_unique<Sprite>();			//ポーズ時にタイトルへ戻るかを書いたスプライト
	std::unique_ptr<Sprite> spriteDone_ = std::make_unique<Sprite>();				//決定表示のスプライト
	std::unique_ptr<Sprite> spriteQuitHowtoPlay_ = std::make_unique<Sprite>();		//遊び方説明時ポーズに戻る案内用のスプライト
	std::unique_ptr<Sprite> spriteReady_ = std::make_unique<Sprite>();				//Ready表記文字用のスプライト
	std::unique_ptr<Sprite> spriteGo_ = std::make_unique<Sprite>();					//Go表記文字用のスプライト
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//フェードインアウトのスプライト
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//ロードスプライト
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//現在ステージスプライト
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//カーソルスプライト
	std::unique_ptr<Sprite> spriteHowToPlayList_ = std::make_unique<Sprite>();		//遊び方説明リストスプライト

	std::unique_ptr<Sprite> spriteTutorialHTPMove_ = std::make_unique<Sprite>();		//チュートリアルの移動方法スプライト
	std::unique_ptr<Sprite> spriteTutorialHTPDash_ = std::make_unique<Sprite>();		//チュートリアルのダッシュ方法スプライト
	std::unique_ptr<Sprite> spriteTutorialHTPJump_ = std::make_unique<Sprite>();		//チュートリアルのジャンプ方法スプライト
	std::unique_ptr<Sprite> spriteTutorialHTPMoveBack_ = std::make_unique<Sprite>();	//チュートリアルの手前、奥側移動方法スプライト
	std::unique_ptr<Sprite> spriteTutorialHTPAttack_ = std::make_unique<Sprite>();		//チュートリアルの攻撃方法スプライト

	std::unique_ptr<Sprite> spriteTutorialInfo1_ = std::make_unique<Sprite>();			//チュートリアル説明文字スプライト(チュートリアル1面)
	std::unique_ptr<Sprite> spriteTutorialInfo2_ = std::make_unique<Sprite>();			//チュートリアル説明文字スプライト(チュートリアル2面)
	std::unique_ptr<Sprite> spriteTutorialInfo3_ = std::make_unique<Sprite>();			//チュートリアル説明文字スプライト(チュートリアル3面)
	std::unique_ptr<Sprite> spriteTutorialInfo4_ = std::make_unique<Sprite>();			//チュートリアル説明文字スプライト(チュートリアル4面)
	std::unique_ptr<Sprite> spriteTutorialInfoHowToPlay_ = std::make_unique<Sprite>();	//チュートリアル説明文字スプライト(チュートリアル中の遊び方説明について)

	//チュートリアル用のイージング状態
	enum GamePlaySceneSettingTutorialEasingNum
	{
		GPSSTEN_Stundby = 0,
		GPSSTEN_Active = 1,
	};

	//チュートリアル説明用の列挙体
	enum TutorialInfoEasingNum
	{
		TIEN_Move = 0,				//移動方法
		TIEN_Dash = 1,				//ダッシュ方法
		TIEN_Jump = 2,				//ジャンプ方法
		TIEN_MoveBack = 3,			//手前、奥側移動方法
		TIEN_Attack = 4,			//攻撃方法
		TIEN_Info = 5,				//ゲーム説明文字
		TIEN_Num=6,					//配列用
	};
	//チュートリアル説明のY軸の値
	const std::array<float, TIEN_Num> tutorialInfoPosY_ = { 70.0f,120.0f,170.0f,220.0f,270.0f,50.0f };

	//チュートリアル表示のイージングのプリセット
	const Easing presetEaseInfoTutorial_[TIEN_Num] =
	{
		{1300.0f, 30.0f, 1.0f},		//移動方法
		{1300.0f, 30.0f, 1.2f},		//ダッシュ方法
		{1300.0f, 30.0f, 1.4f},		//ジャンプ方法
		{1300.0f, 30.0f, 1.6f},		//手前、奥側移動方法
		{1300.0f, 30.0f, 1.8f},		//攻撃方法
		{1300.0f, 530.0f, 2.0f}		//ゲーム説明文字
	};
	//チュートリアル表示のイージング
	Easing easeInfoTutorial_[TIEN_Num] =
	{
		presetEaseInfoTutorial_[TIEN_Move],		//移動方法
		presetEaseInfoTutorial_[TIEN_Dash],		//ダッシュ方法
		presetEaseInfoTutorial_[TIEN_Jump],		//ジャンプ方法
		presetEaseInfoTutorial_[TIEN_MoveBack],	//手前、奥側移動方法
		presetEaseInfoTutorial_[TIEN_Attack],	//攻撃方法
		presetEaseInfoTutorial_[TIEN_Info]		//ゲーム説明文字
	};

	//チュートリアルリストのスケールイージングのプリセット
	const Easing presetEaseTutorialListScale_[XY_Num] =
	{
		{0.0f,252.0f,2.0f},		//X
		{0.0f,196.0f,2.0f}		//Y
	};
	//チュートリアルリストのスケールイージング
	Easing easeTutorialListScale_[XY_Num] =
	{
		presetEaseTutorialListScale_[XY_X],		//X
		presetEaseTutorialListScale_[XY_Y] 		//Y
	};

	//ポーズメニュー用の列挙体
	enum PauseMenuEasingNum
	{
		PMEN_Menu = 0,								//メニュー
		PMEN_Resume = 1,							//再開
		PMEN_HowToPlay = 2,							//遊び方確認
		PMEN_StageSelect = 3,						//ステージセレクトへ
		PMEN_Title = 4,								//タイトルへ
		PMEN_SelectSpace = 5,						//スペースで選択
		PMEN_TutorialHowToPlayInfo = 6,				//チュートリアル時の遊び方説明について
		PMEN_Num=7,									//配列用
	};
	//ポーズメニューのY値
	const std::array<float, PMEN_Num> pausePosY_ = { 0.0f,120.0f,240.0f,360.0f,480.0f,600.0f,240.0f };

	//ポーズメニュー画面出現イージングのプリセット
	const Easing presetEasePauseMenuPosX_[PMEN_Num] =
	{
		{1300.0f, 100.0f, 0.5f},			//メニュー
		{1300.0f, 100.0f, 0.6f},			//再開
		{1300.0f, 100.0f, 0.7f},			//遊び方確認
		{1300.0f, 100.0f, 0.8f},			//ステージセレクトへ
		{1300.0f, 100.0f, 0.9f},			//タイトルへ
		{1300.0f, 425.0f, 1.0f},			//スペースで選択
		{1300.0f, 800.0f, 0.75f}			//チュートリアル時の遊び方説明について
	};
	//ポーズメニュー画面出現イージング
	Easing easePauseMenuPosX_[PMEN_Num] =
	{
		presetEasePauseMenuPosX_[PMEN_Menu],							//メニュー
		presetEasePauseMenuPosX_[PMEN_Resume],							//再開
		presetEasePauseMenuPosX_[PMEN_HowToPlay],						//遊び方確認
		presetEasePauseMenuPosX_[PMEN_StageSelect],						//ステージセレクトへ
		presetEasePauseMenuPosX_[PMEN_Title],							//タイトルへ
		presetEasePauseMenuPosX_[PMEN_SelectSpace],						//スペースで選択
		presetEasePauseMenuPosX_[PMEN_TutorialHowToPlayInfo]			//チュートリアル時の遊び方説明について
	};

	//カーソルX値のイージングプリセット
	const Easing presetEaseCursorPosX_{ -200.0f,20.0f,1.0f};
	//カーソルX値のイージング
	Easing easeCursorPosX_ = presetEaseCursorPosX_;

	//遊び方説明用の列挙体
	enum HowToPlayEasingNum
	{
		HTPEN_Move = 0,							//移動
		HTPEN_Dash = 1,							//ダッシュ
		HTPEN_Jump = 2,							//ジャンプ
		HTPEN_MoveBack = 3,						//手前、奥側移動
		HTPEN_Attack = 4,						//攻撃
		HTPEN_Quit = 5,							//遊び方を抜ける
		HTPEN_Num = 6,							//配列用							
	};
	//遊び方説明のY値
	const std::array<float, HTPEN_Num> howToPlayPosY_ = { 0.0f,120.0f,240.0f,360.0f,480.0f,600.0f};

	//遊び方説明画面出現イージングのプリセット
	const Easing presetEaseHowToPlayPosX_[HTPEN_Num] =
	{
		{1300.0f, 100.0f, 0.5f},			//移動
		{1300.0f, 200.0f, 0.6f},			//ダッシュ
		{1300.0f, 300.0f, 0.7f},			//ジャンプ
		{1300.0f, 400.0f, 0.8f},			//手前、奥側移動
		{1300.0f, 500.0f, 0.9f},			//攻撃
		{1300.0f, 425.0f, 1.0f}				//遊び方を抜ける
	};
	//遊び方説明画面出現イージング
	Easing easeHowToPlayPosX_[HTPEN_Num] =
	{
		presetEaseHowToPlayPosX_[HTPEN_Move],			//移動
		presetEaseHowToPlayPosX_[HTPEN_Dash],			//ダッシュ
		presetEaseHowToPlayPosX_[HTPEN_Jump],			//ジャンプ
		presetEaseHowToPlayPosX_[HTPEN_MoveBack],		//手前、奥側移動
		presetEaseHowToPlayPosX_[HTPEN_Attack],			//攻撃
		presetEaseHowToPlayPosX_[HTPEN_Quit]			//遊び方を抜ける
	};

	//入場用の視点カメラワークイージングのプリセット
	const Easing presetEaseEyeGameStart_[XYZ_Num]=
	{
		{-110.0f, -20.0f, 4.0f},				//X
		{101.0f, 1.0f, 4.0f},					//Y
		{-210.0f, -100.0f, 3.5f}				//Z
	};
	//入場用の視点カメラワークイージング
	Easing easeEyeGameStart_[XYZ_Num]=
	{
		presetEaseEyeGameStart_[XYZ_X],				//X
		presetEaseEyeGameStart_[XYZ_Y],				//Y
		presetEaseEyeGameStart_[XYZ_Z]				//Z
	};
	//入場用の注視点カメラワークイージングのプリセット
	const Easing presetEaseTargetGameStart_[XYZ_Num]
	{
		{-110.0f, -20.0f, 4.0f},				//X
		{100.0f, 0.0f, 4.0f},					//Y
		{-110.0f, 0.0f, 3.5f}					//Z
	};
	//入場用の注視点カメラワークイージング
	Easing easeTargetGameStart_[XYZ_Num]
	{
		presetEaseTargetGameStart_[XYZ_X],				//X
		presetEaseTargetGameStart_[XYZ_Y],				//Y
		presetEaseTargetGameStart_[XYZ_Z]				//Z
	};
	//入場用のプレイヤーポジションイージング
	Easing easePlayerPositionGameStart_[XYZ_Num];

	//入場用のイージングの表記は少し特殊
	enum XXY
	{
		XXY_X1=0,	//X(パート1)
		XXY_X2=1,	//X(パート2)
		XXY_Y=2,	//Y
		XXY_Num=3	//配列用
	};			
	//入場用のレディー表記のイージングのプリセット
	const Easing presetEaseReadyPosition_[XXY_Num]
	{
		{1300.0f, 375.0f, 1.5f},				//X(パート1)
		{375.0f,-600.0f,1.5f},					//X(パート2)
		{300.0f, 300.0f, 3.0f}					//Y
	};
	//入場用のレディー表記のイージング
	Easing easeReadyPosition_[XXY_Num]
	{
		presetEaseReadyPosition_[XXY_X1],				//X(パート1)
		presetEaseReadyPosition_[XXY_X2],				//X(パート2)
		presetEaseReadyPosition_[XXY_Y]					//Y
	};

	//レディーイージングその2が始まるフラグ
	bool isStartReadyPart2_ = false;
	//レディーイージングが終わったかのフラグ
	bool isEndReady_ = false;

	//入場用のゴー表記のイージングのプリセット
	const Easing presetEaseGoSizeAndAlpha_[XYW_Num]
	{
		{0.0f, 2000.0f, 1.0f},						//Xサイズ
		{0.0f, 1000.0f, 1.0f},						//Yサイズ
		{1.0f,0.0f,0.8f}							//アルファ値
	};
	//入場用のゴー表記のイージング
	Easing easeGoSizeAndAlpha_[XYW_Num]
	{
		presetEaseGoSizeAndAlpha_[XYW_X],					//Xサイズ
		presetEaseGoSizeAndAlpha_[XYW_Y],					//Yサイズ
		presetEaseGoSizeAndAlpha_[XYW_W]					//アルファ値
	};
	//ゴー表記の座標値
	const std::array<float, XY_Num> goPosition_ = { 640.0f,360.0f };

	//入場時のイージングスタート地点を決める変数
	//DirectX::XMFLOAT3 startEaseCameraWorkEye_;			//視点
	//DirectX::XMFLOAT3 startEaseCameraWorkTarget_;		//注視点
	DirectX::XMFLOAT3 startEasePlayerPosition_;			//プレイヤーポジション

	//フェードインアウトのプリセット(false フェードイン、true フェードアウト)
	const Easing presetEaseFadeInOut_ = {1.0f, 0.0f, 1.0f};
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;
	
	//ポーズ用のフェードインアウトイージングのプリセット
	const Easing presetEaseFadeInOutPause_ = {0.8f, 0.0f, 1.0f};
	//ポーズ用のフェードインアウトイージング
	Easing easeFadeInOutPause_ = presetEaseFadeInOutPause_;

	//プレイ中か
	bool isGamePlay_ = false;
	//遊び方説明画面時か
	bool isHowToPlay_ = false;
	//遊び方説明からポーズへ戻る時か
	bool isBackPause_ = false;
	//ゲームプレイシーンから離れるか
	bool isQuit_ = false;
	//ポーズしたか
	bool isPause_ = false;
	//クリアしたか
	bool isClear_ = false;
	//ゲームオーバーになったか
	bool isGameOver_ = false;
	//スタート時
	bool isStart_ = true;
	//フェードアウト(遷移時)
	bool isFadeOutScene_ = false;
	//フェードアウト(ポーズ時)
	bool isFadeOutPause_ = false;
	//フェードイン(ポーズ時)
	bool isFadeInPause_ = false;
	//モデル
	//自機
	std::list<std::unique_ptr<Player>> players_;						//自機リスト
	Player::PlayerModelList modelPlayerList_;							//自機モデル構造体
	std::unique_ptr<Model> modelPlayer_ = nullptr;						//自機モデル
	std::unique_ptr<Model> modelPlayerBullet_ = nullptr;				//自機弾モデル
	std::unique_ptr<Model> modelPlayerDash_ = nullptr;					//自機ダッシュモデル
	std::unique_ptr<Model> modelPlayerJump_ = nullptr;					//自機ジャンプモデル
	std::unique_ptr<Model> modelPlayerAttack_ = nullptr;				//自機攻撃モデル
	std::unique_ptr<Model> modelPlayerHit_ = nullptr;					//自機死亡モデル
	//敵
	std::unique_ptr<AbstractEnemyFactory> enemyFactory_;					//敵の工場
	std::list<std::unique_ptr<BaseEnemy>> enemys_;							//敵リスト
	std::unique_ptr<Model> modelEnemy1_ = nullptr;							//敵モデル
	std::unique_ptr<Model> modelEnemyBullet_ = nullptr;						//敵弾モデル
	std::unique_ptr<Model> modelBoss1_ = nullptr;							//ステージ1のボスモデル
	std::unique_ptr<Model> modelBossCore1_ = nullptr;						//ステージ1のボスの周りにあるコアのモデル

	//ギミック
	std::unique_ptr<AbstractGimmickFactory> gimmickFactory_;					//ギミックの工場
	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;							//ギミックリスト
	std::unique_ptr<Model> modelSpike_ = nullptr;								//トゲのモデル
	std::unique_ptr<Model> modelBoxUpDown_ = nullptr;								//上下する床のモデル

	//ゴール
	std::list<std::unique_ptr<Goal>> goals_;					//ゴールリスト
	std::unique_ptr<Model> modelGoal_ = nullptr;				//ゴールモデル

	//アイテム
	std::list<std::unique_ptr<Item>> items_;					//アイテムリスト
	std::unique_ptr<Model> modelItemJump_ = nullptr;			//ジャンプ力強化アイテムモデル
	std::unique_ptr<Model> modelItemHeal_ = nullptr;			//ライフ回復アイテムモデル
	std::unique_ptr<Model> modelStageT_ = nullptr;				//チュートリアルステージモデル(天球)
	std::unique_ptr<Model> modelStage1_ = nullptr;				//ステージ1モデル(天球)
	std::unique_ptr<Model> modelStage2_ = nullptr;				//ステージ2モデル(天球)

	//床
	std::unique_ptr<Model> modelGround_ = nullptr;				//床のモデル
	std::unique_ptr<Model> modelSphere_ = nullptr;					//球モデル
	std::unique_ptr<Model> modelBox_ = nullptr;					//AABB床モデル

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//マッピングモデル
	std::map<std::string, Model*> models_;
	//オブジェクト配列
	std::vector<std::unique_ptr<Object3d>> objects_;
	//天球配列
	std::vector<std::unique_ptr<Object3d>> skydomes_;

	//パーティクル
	std::unique_ptr<Particle> particle1_ = nullptr;

	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pm_ = nullptr;

	//カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	//ライト
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;

	//コリジョンマネージャー
	CollisionManager* colManager_ = nullptr;

	//チュートリアル表示の色
	DirectX::XMFLOAT3 infoColor_;//xyz=rgb

private:
	
	
	/*
	stagenumの値
	0~10		ステージ1
	11~20		ステージ2
	101~110		チュートリアル
	*/
	//ステージ番号↑
	int stageNum_;
	//自機弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//敵弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	//メニュー関係
	//メニュー番号
	int menuCount_ = GPSPMI_Resume;
	//○○した瞬間に○○解除を防ぐ用のフラグ
	bool isBack_ = false;
	//色を変えるスピード
	float speedColor_ = 0.0f;
	//色反転フラグ
	bool isColorReverse_ = false;

private:
	//スプライト読み込み
	void LoadSprite();
	//モデル読み込み
	void LoadModel();
	/*
	チュートリアル用のイージング(スプライト1～6枚)
	num=0 スタンバイ
	num=1 イージング中
	*/
	void SettingTutorialEase(const int num, Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)const;

	//チュートリアルスプライトの更新
	void UpdateTutorialSprite();

	//チュートリアル用のスプライト描画(スプライト1～6枚)
	void DrawTutorialSprite(const Sprite* s1, const Sprite* s2,
		const Sprite* s3, const Sprite* s4, const Sprite* s5, const Sprite* s6)const;
};