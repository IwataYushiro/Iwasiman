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
#include "Earth.h"

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
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;

public://構造体類
	GamePlayScene(int stagenum);
	enum Scene { //シーンID
		tutorial,
		stage1,
		stage2,
	};

	//キースプライト構造
	struct KeySprite {
	public:
		void Initialize();
		void Update();
		void Draw();
		void Finalize();
		//キー部分
		Sprite* key_ = nullptr;
		//ラベル
		Sprite* label_ = nullptr;

		//テクスチャの左上の点
		XMFLOAT2 leftTop_ = { 0,0 };
		//位置
		XMFLOAT2 position_ = { 0,0 };
		//サイズ
		XMFLOAT2 size_ = { 0,0 };
		//押されているか？
		bool isPress_ = false;
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

	int GetEnemyCount() { return EnemyCount; }
	void SetEnemyCount(int enemycount) { this->EnemyCount = enemycount; }

private://静的メンバ変数

	//DirectX基盤
	static DirectXCommon* dxCommon_;
	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//インプット
	static Input* input_;
	//オーディオ
	Audio* audio_=nullptr;
	//シーンマネージャー
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;
	//カメラ
	static Camera* camera_;
private:

	//サウンド読み込み
	Audio::SoundData stageBGM;
	Audio::SoundData doneSE;
	Sprite* spritePause_ = new Sprite();
	Sprite* spriteClear_ = new Sprite();
	Sprite* spritePauseInfo_ = new Sprite();
	Sprite* spriteGameover_ = new Sprite();
#pragma endregion

#pragma region キースプライト
	KeySprite* keySpriteA_ = nullptr;
	KeySprite* keySpriteD_ = nullptr;
	KeySprite* keySpriteX_ = nullptr;
#pragma endregion

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
	Model* modelEnemy1Power_ = nullptr;
	Model* modelEnemy1Guard_ = nullptr;
	Model* modelEnemy1Speed_ = nullptr;
	Model* modelEnemy1Death_ = nullptr;
	Model* modelEnemyBullet_ = nullptr;

	Model* modelEnemy2_ = nullptr;
	Model* modelEnemy2Power_ = nullptr;
	Model* modelEnemy2Guard_ = nullptr;
	Model* modelEnemy2Speed_ = nullptr;
	Model* modelEnemy2Death_ = nullptr;


	Model* modelBoss1_ = nullptr;
	Model* modelBossCore1_ = nullptr;

	std::unique_ptr<AbstractGimmickFactory> gimmickFactory;

	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;

	std::list<std::unique_ptr<Goal>> goals_;
	Model* modelGoal_ = nullptr;

	std::list<std::unique_ptr<Earth>> earths_;
	Model* modelEarth_ = nullptr;

	std::list<std::unique_ptr<Item>> items_;
	Model* modelItemJump_ = nullptr;
	Model* modelItemHeal_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;
	Model* modelGround = nullptr;
	Model* modelBox = nullptr;
	Model* modelRail = nullptr;

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
	int stageNum;
	//自機弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//敵弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//イージングマネージャー(左から右へ)
	Easing es = Easing(-(float)WinApp::GetInstance()->window_width, 0.0f, 1.0f);
	//○○した瞬間に○○解除を防ぐ用のフラグ
	bool isBack = false;
	//敵カウント
	int EnemyCount = 0;

private:
	//スプライト読み込み
	void LoadSprite();
	//モデル読み込み
	void LoadModel();

};