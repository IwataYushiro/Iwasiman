#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"

#include "SceneManager.h"

//タイトルシーン
class TitleScene :public BaseScene
{
public://メンバ関数
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;

private://静的メンバ変数
	//DirectX基盤
	static DirectXCommon* dxCommon_;
	//スプライト基盤
	static SpriteCommon* spCommon_;
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

private://メンバ変数
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	//FBX
	ModelFbx* modelF = nullptr;
	ObjectFbx* objF = nullptr;

	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
};
