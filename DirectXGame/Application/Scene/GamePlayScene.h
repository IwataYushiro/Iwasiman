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
	GamePlayScene(int stagenum);

	//初期化
	void Initialize()override;
	//更新
	void Update() override;
	//状態更新(ゲームプレイ時)
	void UpdateIsPlayGame();
	//状態更新(ポーズ画面時)
	void UpdateIsPause();
	//状態更新(ステージクリア時)
	void UpdateIsStageClear();
	//チュートリアル更新
	void UpdateTutorial();
	
	//フェードアウト(色)
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//フェードイン(色)
	void FadeIn(DirectX::XMFLOAT3 rgb);
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
	void AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet);
	//敵弾追加(敵の弾)
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);

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
	Sprite* spritePause_ = new Sprite();				//ポーズ時のスプライト
	Sprite* spritePauseInfo_ = new Sprite();			//どのキーでポーズするのかを書いたスプライト
	Sprite* spritePauseResume_ = new Sprite();			//ポーズ時にゲーム再開するかを書いたスプライト
	Sprite* spritePauseHowToPlay_ = new Sprite();		//ポーズ時に遊び方を確認するかを書いたスプライト
	Sprite* spritePauseStageSelect_ = new Sprite();		//ポーズ時にステージセレクトへ戻るかを書いたスプライト
	Sprite* spritePauseTitle_ = new Sprite();			//ポーズ時にタイトルへ戻るかを書いたスプライト
	Sprite* spriteDone_ = new Sprite();					//決定表示のスプライト
	Sprite* spriteFadeInOut_ = new Sprite();			//フェードインアウトのスプライト


	Sprite* spriteTutorialHTPMove_ = new Sprite();		//チュートリアルの移動方法スプライト
	Sprite* spriteTutorialHTPDash_ = new Sprite();		//チュートリアルのダッシュ方法スプライト
	Sprite* spriteTutorialHTPJump_ = new Sprite();		//チュートリアルのジャンプ方法スプライト
	Sprite* spriteTutorialHTPMoveBack_ = new Sprite();	//チュートリアルの手前、奥側移動方法スプライト
	Sprite* spriteTutorialHTPAttack_ = new Sprite();	//チュートリアルの攻撃方法スプライト

	Sprite* spriteTutorialInfo1_ = new Sprite();		//チュートリアル説明文字スプライト(チュートリアル1面)
	Sprite* spriteTutorialInfo2_ = new Sprite();		//チュートリアル説明文字スプライト(チュートリアル2面)
	Sprite* spriteTutorialInfo3_ = new Sprite();		//チュートリアル説明文字スプライト(チュートリアル3面)
	Sprite* spriteTutorialInfo4_ = new Sprite();		//チュートリアル説明文字スプライト(チュートリアル4面)

	//チュートリアル表示のイージング
	Easing easeInfoTutorial_[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),	//移動方法
		Easing(1300.0f, 500.0f, 1.2f),	//ダッシュ方法
		Easing(1300.0f, 0.0f, 1.4f),	//ジャンプ方法
		Easing(1300.0f, 500.0f, 1.6f),	//手前、奥側移動方法
		Easing(1300.0f, 800.0f, 1.8f),	//攻撃方法
		Easing(1300.0f, 0.0f, 2.0f),	//ゲーム説明文字
	};

	//ポーズメニュー画面出現イージング
	Easing easePauseMenuPosX_[6] =
	{
		Easing(1300.0f, 100.0f, 0.5f),			//メニュー
		Easing(1300.0f, 50.0f, 0.6f),			//再開
		Easing(1300.0f, 50.0f, 0.7f),			//遊び方確認
		Easing(1300.0f, 50.0f, 0.8f),			//ステージセレクトへ
		Easing(1300.0f, 50.0f, 0.9f),			//タイトルへ
		Easing(1300.0f, 0.0f, 1.0f),			//スペースで選択
	};

	//遊び方説明画面出現イージング
	Easing easeHowToPlayPosX_[6] =
	{
		Easing(1300.0f, 0.0f, 0.5f),			//メニュー
		Easing(1300.0f, 0.0f, 0.6f),			//再開
		Easing(1300.0f, 0.0f, 0.7f),			//遊び方確認
		Easing(1300.0f, 0.0f, 0.8f),			//ステージセレクトへ
		Easing(1300.0f, 0.0f, 0.9f),			//タイトルへ
		Easing(1300.0f, 0.0f, 1.0f),			//スペースで選択
	};

	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 0.5f);
	//ポーズ用のフェードインアウトイージング
	Easing easeFadeInOutPause_ = Easing(0.8f, 0.0f, 0.5f);

	//プレイ中か
	bool isGamePlay_ = true;
	//遊び方説明画面時か
	bool isHowToPlay_ = false;
	//ゲームプレイシーンから離れるか
	bool isQuit_ = false;
	//ポーズしたか
	bool isPause_ = false;
	//クリアしたか
	bool isClear_ = false;
	//ゲームオーバーになったか
	bool isGameover_ = false;
	//フェードアウト(遷移時)
	bool isFadeOutScene_ = false;
	//フェードアウト(ポーズ時)
	bool isFadeOutPause_ = false;
	//フェードイン(ポーズ時)
	bool isFadeInPause_ = false;
	//モデル
	//自機
	std::list<std::unique_ptr<Player>> players_;		//自機リスト
	Model* modelPlayer_ = nullptr;						//自機モデル
	Model* modelPlayerBullet_ = nullptr;				//自機弾モデル

	//敵
	std::unique_ptr<AbstractEnemyFactory> enemyFactory_;	//敵の工場
	std::list<std::unique_ptr<BaseEnemy>> enemys_;			//敵リスト
	Model* modelEnemy1_ = nullptr;							//敵モデル
	Model* modelEnemyBullet_ = nullptr;						//敵弾モデル
	Model* modelBoss1_ = nullptr;							//ステージ1のボスモデル
	Model* modelBossCore1_ = nullptr;						//ステージ1のボスの周りにあるコアのモデル

	//ギミック
	std::unique_ptr<AbstractGimmickFactory> gimmickFactory_;	//ギミックの工場
	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;			//ギミックリスト
	Model* modelSpike_ = nullptr;								//トゲのモデル

	//ゴール
	std::list<std::unique_ptr<Goal>> goals_;	//ゴールリスト
	Model* modelGoal_ = nullptr;				//ゴールモデル

	//アイテム
	std::list<std::unique_ptr<Item>> items_;	//アイテムリスト
	Model* modelItemJump_ = nullptr;			//ジャンプ力強化アイテムモデル
	Model* modelItemHeal_ = nullptr;			//ライフ回復アイテムモデル
	Model* modelStageT_ = nullptr;				//チュートリアルステージモデル(天球)
	Model* modelStage1_ = nullptr;				//ステージ1モデル(天球)
	Model* modelStage2_ = nullptr;				//ステージ2モデル(天球)

	//床
	Model* modelGround_ = nullptr;				//床のモデル
	Model* modelBox_ = nullptr;					//AABB床モデル

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//マッピングモデル
	std::map<std::string, Model*> models_;
	//オブジェクト配列
	std::vector<Object3d*> objects_;
	//天球配列
	std::vector<Object3d*> skydomes_;

	//パーティクル
	Particle* particle1_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* pm_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//ライト
	LightGroup* lightGroup_ = nullptr;

	//コリジョンマネージャー
	CollisionManager* colManager_ = nullptr;

	//チュートリアル表示の色
	DirectX::XMFLOAT3 infoColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb

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
	int menuCount_ = 0;
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
	void SettingTutorialEase(int num, Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);

	//チュートリアルスプライトの更新
	void UpdateTutorialSprite();

	//チュートリアル用のスプライト描画(スプライト1～6枚)
	void DrawTutorialSprite(Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);
};