#pragma once
#include "Framework.h"
#include "Audio.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"

#include "Player.h"

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

private:
	
	//Sprite
	Sprite* sprite_ = new Sprite();
	//Audio
	Audio* audio_ = new Audio();

	//imgui
	ImGuiManager* imguiManager_ = new ImGuiManager();

	//サウンド読み込み
	Audio::SoundData sound;

	//モデル
	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;
	//天球
	Model* modelSkyDome_ = nullptr;

	//3Dオブジェクト
	Object3d* object3DPlayer_ = nullptr;
	Object3d* object3DEnemy_ = nullptr;
	Object3d* object3DSkyDome_ = nullptr;

	Player* player_ = nullptr;

	//シーン
	size_t scene_;

	//ゲーム終了フラグ
	bool EndGame_ = false;

};

