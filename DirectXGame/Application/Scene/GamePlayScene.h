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

//ゲームプレイ
class GamePlayScene :public BaseScene
{
public://構造体類
	GamePlayScene(int stagenum);
	enum Scene { //シーンID
		tutorial,
		stage1,
		stage2,
	};

public:

	//初期化
	void Initialize()override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;

	//レベルデータ読み込み
	void LoadLVData(const std::string& stagePath);

public:
	//自機弾追加
	void AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet);
	//敵弾追加
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
	//カメラ
	static Camera* camera_;
private:

	//サウンド読み込み
	Audio::SoundData sound;

	//スプライト
	Sprite* spritePause_ = new Sprite();
	Sprite* spritePauseInfo_ = new Sprite();

	Sprite* spriteTutorialHTPMove = new Sprite();
	Sprite* spriteTutorialHTPDash = new Sprite();
	Sprite* spriteTutorialHTPJump = new Sprite();

	Sprite* spriteTutorialHTPMoveBack = new Sprite();

	Sprite* spriteTutorialHTPAttack = new Sprite();


	Sprite* spriteTutorialInfo1 = new Sprite();
	Sprite* spriteTutorialInfo2 = new Sprite();
	Sprite* spriteTutorialInfo3 = new Sprite();
	Sprite* spriteTutorialInfo4 = new Sprite();

	Easing easeInfo[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//メニュー
		Easing(1300.0f, 500.0f, 1.2f),//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),//ステージセレクトへ
		Easing(1300.0f, 500.0f, 1.6f),//スペースで選択
		Easing(1300.0f, 800.0f, 1.8f) ,
		Easing(1300.0f, 0.0f, 2.0f),
	};

	//ポーズしたか
	bool isPause_ = false;
	bool isclear = false;
	bool isGameover = false;

	//モデル
	std::list<std::unique_ptr<Player>> players_;
	Model* modelPlayer_ = nullptr;
	Model* modelPlayerBullet_ = nullptr;

	std::unique_ptr<AbstractEnemyFactory> enemyFactory;

	std::list<std::unique_ptr<BaseEnemy>> enemys_;
	Model* modelEnemy1_ = nullptr;
	Model* modelEnemyBullet_ = nullptr;

	Model* modelBoss1_ = nullptr;
	Model* modelBossCore1_ = nullptr;

	std::unique_ptr<AbstractGimmickFactory> gimmickFactory;

	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;
	Model* modelSpike_ = nullptr;

	std::list<std::unique_ptr<Goal>> goals_;
	Model* modelGoal_ = nullptr;

	std::list<std::unique_ptr<Item>> items_;
	Model* modelItemJump_ = nullptr;
	Model* modelItemHeal_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelStageT = nullptr;
	Model* modelStage1 = nullptr;
	Model* modelStage2 = nullptr;

	Model* modelGround = nullptr;
	Model* modelBox = nullptr;

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm_ = nullptr;

	Particle* particle2_ = nullptr;

	//ライト
	LightGroup* lightGroup_ = nullptr;

	//衝突マネージャ
	CollisionManager* colManager_ = nullptr;

private:
	/*
	stagenumの値
	1~9		ステージ1
	10~19	ステージ2
	100~109	チュートリアル
	*/
	int stageNum;
	//自機弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//敵弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//イージングマネージャー(左から右へ)
	Easing es = Easing(-(float)WinApp::GetInstance()->window_width, 0.0f, 1.0f);
	//○○した瞬間に○○解除を防ぐ用のフラグ
	bool isBack = false;
	//色を変えるスピード
	float speedColor = 0.0f;
	//色反転フラグ
	bool isColorReverse_ = false;
private:
	//スプライト読み込み
	void LoadSprite();
	//モデル読み込み
	void LoadModel();
	/*
	チュートリアル用のイージング
	num=0 スタンバイ
	num=1 イージング中
	*/
	void SettingTutorialEase(int num, Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);
	void UpdateTutorialSprite();
	//チュートリアル用のスプライト描画
	void DrawTutorialSprite(Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);
};