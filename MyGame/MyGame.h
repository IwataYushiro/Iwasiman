#pragma once
#include "Framework.h"
#include "WinApp.h"
#include "Audio.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Object3d.h"
#include "SpriteCommon.h"
#include "Sprite.h"

class MyGame :public Framework
{
public:
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//終了
	void Finalize() override;
	
private:
	//WinApp
	WinApp* winApp_ = new WinApp();
	//DXCommon
	DirectXCommon* dxCommon_ = new DirectXCommon();
	//SpriteCommon
	SpriteCommon* sprCommon_ = new SpriteCommon();
	//Sprite
	Sprite* sprite_ = new Sprite();
	//Audio
	Audio* audio_ = new Audio();
	//Input
	Input* input_ = new Input();
	//imgui
	ImGuiManager* imguiManager_ = new ImGuiManager();

	//サウンド読み込み
	Audio::SoundData sound;

	//モデル
	Model* model_ = nullptr;

	//3Dオブジェクト
	Object3d* object3D_ = nullptr;

	//ゲーム終了フラグ
	bool EndGame_ = false;

};

