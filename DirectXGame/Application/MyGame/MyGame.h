#pragma once
#include "Framework.h"
#include "Audio.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"

#include "Player.h"
#include "Enemy.h"

class MyGame :public Framework
{
public:

	enum Scene { //シーンID
		title,
		howtoplay,
		stage,
		clear,
		gameover,
	};

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//終了
	void Finalize() override;

	//当たり判定
	void ChackAllCollisions();

private:
	
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteHowToPlay_ = new Sprite();
	Sprite* spriteGameClear_ = new Sprite();
	Sprite* spriteGameOver_ = new Sprite();
	//Audio
	Audio* audio_ = new Audio();

	//imgui
	ImGuiManager* imguiManager_ = new ImGuiManager();

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
	ParticleManager * pm1_ = nullptr;

	Particle* particle2_ = nullptr;
	ParticleManager* pm2_ = nullptr;
	//シーン
	size_t scene_;

	//ゲーム終了フラグ
	bool EndGame_ = false;

};

