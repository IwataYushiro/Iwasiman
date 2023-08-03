#pragma once
#include "Audio.h"
#include "Camera.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "Enemy.h"
#include "Goal.h"
#include <vector>
#include <map>
#include <sstream>
#include <chrono>
#include "SceneManager.h"
#include "CollisionPrimitive.h"
#include "EasingManager.h"

//jsonレベルデータ
struct LevelData;
//前方宣言
class Player;
class CollisionManager;
class TouchableObject;

//ゲームプレイ
class GamePlayScene :public BaseScene
{
public://構造体類
	enum Scene { //シーンID
		title,
		howtoplay,
		stage,
		clear,
		gameover,
	};

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;
	
	//レベルデータ読み込み
	void LoadLVData();

	//敵発生データの読み込み
	void LoadEnemyPopData();
	//敵発生コマンドの更新
	void UpdateEnemyPopCommands();

public:
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

	Sprite* spritePause_ = new Sprite();
	Sprite* spriteClear_ = new Sprite();
	Sprite* spritePauseInfo_ = new Sprite();
	Sprite* spriteGameover_ = new Sprite();
	
	//ポーズしたか
	bool isPause_ = false;
	bool isclear = false;
	bool isGameover = false;

	//モデル
	Player* player_ = nullptr;
	Model* modelPlayer_ = nullptr;

	std::list<std::unique_ptr<Enemy>> enemys_;
	Model* modelEnemy_ = nullptr;

	Goal* goal_ = nullptr;
	Model* modelGoal_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;
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
	//敵弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//敵発生コマンド
	std::stringstream enemyPopCommands;

	EasingManager es = { -(float)WinApp::GetInstance()->window_width,0.0f,1.0f };
	

private:
	//スプライト読み込み
	void LoadSprite();
};