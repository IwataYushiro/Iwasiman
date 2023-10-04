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

//ゲームプレイシーン

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
	Audio::SoundData sound_;

	//スプライト
	Sprite* spritePause_ = new Sprite();
	Sprite* spritePauseInfo_ = new Sprite();

	Sprite* spriteTutorialHTPMove_ = new Sprite();
	Sprite* spriteTutorialHTPDash_ = new Sprite();
	Sprite* spriteTutorialHTPJump_ = new Sprite();

	Sprite* spriteTutorialHTPMoveBack_ = new Sprite();

	Sprite* spriteTutorialHTPAttack_ = new Sprite();


	Sprite* spriteTutorialInfo1_ = new Sprite();
	Sprite* spriteTutorialInfo2_ = new Sprite();
	Sprite* spriteTutorialInfo3_ = new Sprite();
	Sprite* spriteTutorialInfo4_ = new Sprite();

	Easing easeInfo_[6] =
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
	bool isclear_ = false;
	bool isGameover_ = false;

	//モデル
	std::list<std::unique_ptr<Player>> players_;
	Model* modelPlayer_ = nullptr;
	Model* modelPlayerBullet_ = nullptr;

	std::unique_ptr<AbstractEnemyFactory> enemyFactory_;

	std::list<std::unique_ptr<BaseEnemy>> enemys_;
	Model* modelEnemy1_ = nullptr;
	Model* modelEnemyBullet_ = nullptr;

	Model* modelBoss1_ = nullptr;
	Model* modelBossCore1_ = nullptr;

	std::unique_ptr<AbstractGimmickFactory> gimmickFactory_;

	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;
	Model* modelSpike_ = nullptr;

	std::list<std::unique_ptr<Goal>> goals_;
	Model* modelGoal_ = nullptr;

	std::list<std::unique_ptr<Item>> items_;
	Model* modelItemJump_ = nullptr;
	Model* modelItemHeal_ = nullptr;

	LevelData* levelData_ = nullptr;

	Model* modelStageT_ = nullptr;
	Model* modelStage1_ = nullptr;
	Model* modelStage2_ = nullptr;

	Model* modelGround_ = nullptr;
	Model* modelBox_ = nullptr;

	std::map<std::string, Model*> models_;
	std::vector<Object3d*> objects_;
	std::vector<Object3d*> skydomes_;

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
	0~10		ステージ1
	11~20		ステージ2
	101~110		チュートリアル
	*/

	int stageNum_;
	//自機弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//敵弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//ポーズのイージング(左から右へ)
	Easing easePause_ = Easing(-(float)WinApp::GetInstance()->window_width, 0.0f, 1.0f);
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