#pragma once
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"

#include <DirectXMath.h>
#include <map>

#include "SceneManager.h"

//jsonレベルデータ
struct LevelData;

//タイトルシーン
class StageSelectScene :public BaseScene
{
public://メンバ関数
	StageSelectScene(int count);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;
	//レベルデータ読み込み
	void LoadLVData(const std::string& stagePath);

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

private://メンバ変数
	//Sprite
	Sprite* spriteMenu_ = new Sprite();
	Sprite* spriteTutorial_ = new Sprite();
	Sprite* spriteStage1_ = new Sprite();
	Sprite* spriteStage2_ = new Sprite();
	Sprite* spriteDone_ = new Sprite();
	Sprite* spriteBack_ = new Sprite();

	//FBX
	ModelFbx* modelF = nullptr;
	ObjectFbx* objF = nullptr;

	//モデル
	//Model* modelPlayer_ = nullptr;
	//Object3d* object3DPlayer_ = nullptr;

	LevelData* levelData = nullptr;

	Object3d* objStage = nullptr;
	
	Model* modelStageTutorial = nullptr;
	Model* modelStage1 = nullptr;
	Model* modelStage2 = nullptr;

	//オブジェクト回転用
	DirectX::XMFLOAT3 rot = { 0.0f,0.0f,0.0f };

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	bool isStart = false;
	bool isMenu = false;
	Easing easeTitlePosX = Easing(0.0f, -1300.0f, 1.0f);

	Easing easeMenuPosX[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//メニュー
		Easing(1300.0f, 0.0f, 1.2f),//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),//スペースで選択
		Easing(1300.0f, 0.0f, 1.8f),//スペースで選択
		Easing(1300.0f, 900.0f, 2.0f),//戻る
	};
	int MenuCount = 0;
	//色を変えるスピード
	float speedColor = 0.0f;
	//色反転フラグ
	bool isColorReverse_ = false;
	//ステージセレクトから抜ける
	bool outStageSerect = false;

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//モデル読み込み
	//void LoadModel();

};
