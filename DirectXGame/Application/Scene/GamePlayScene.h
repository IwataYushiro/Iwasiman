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

public:
	//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	float elapsed;
	const float X_START = (float)WinApp::GetInstance()->window_width;
	const float X_END = 0.0f;
	const float TIME_END = 1.0f;

	void UpdateVelPos()
	{
		vel_x += acc_x;
		pos_x += vel_x;

	}

	float ease_in(float t, float b, float c, float d)
	{
		float x = min(t / d,1.0f);
		float v = ease_in_out_cric(x);
		float ret = c * v + b;
		return ret;
	}
	float ease_in_cubic(float x)
	{
		return x * x * x;
	}
	float ease_in_out_cric(float x)
	{
		return x < 0.5f ?
			(1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f
			: (sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	}

private:
	//位置
	float pos_x;
	//速度
	float vel_x;
	//加速度
	float acc_x;
	//
	float t;
	float b = X_START;
	float c = X_END - X_START;
	float d = TIME_END;

private:
	//スプライト読み込み
	void LoadSprite();
	
};