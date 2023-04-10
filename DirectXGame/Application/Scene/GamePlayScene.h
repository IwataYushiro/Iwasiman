#pragma once
#include "Audio.h"
#include "Camera.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"

#include "Player.h"
#include "Enemy.h"

//ゲームプレイ
class GamePlayScene
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
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
	//終了
	void Finalize();
	//当たり判定
	void ChackAllCollisions();

private://静的メンバ変数
	//DirectX基盤
	static DirectXCommon* dxCommon_;
	//スプライト基盤
	static SpriteCommon* spCommon_;
	//インプット
	static Input* input_;
	//オーディオ
	static Audio* audio_;
private:

	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteHowToPlay_ = new Sprite();
	Sprite* spriteGameClear_ = new Sprite();
	Sprite* spriteGameOver_ = new Sprite();

	//サウンド読み込み
	Audio::SoundData sound;

	//モデル
	Player* player_ = nullptr;
	Model* modelPlayer_ = nullptr;

	Enemy* enemy_ = nullptr;
	Model* modelEnemy_ = nullptr;

	//3Dオブジェクト
	Object3d* object3DPlayer_ = nullptr;
	Object3d* object3DEnemy_ = nullptr;

	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;

	Particle* particle2_ = nullptr;
	ParticleManager* pm2_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;
	//シーン
	size_t scene_;


};