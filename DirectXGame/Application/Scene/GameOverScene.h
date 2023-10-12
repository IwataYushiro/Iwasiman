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

/*

*	GameOverScene.h

*	ゲームオーバーシーン

*/
class GameOverScene :public BaseScene
{
public://メンバ関数
	GameOverScene(int stagenum);
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
	int stageNum;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();
	Sprite* spriteContinue_ = new Sprite();
	Sprite* spriteStageSelect_ = new Sprite();
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteDone_ = new Sprite();

	//FBX
	//ModelFbx* modelF_ = nullptr;
	//ObjectFbx* objF_ = nullptr;

	//モデル
	//Model* modelPlayer_ = nullptr;
	//Object3d* object3DPlayer_ = nullptr;

	LevelData* levelData_ = nullptr;

	Object3d* objStage_ = nullptr;

	Model* modelStageTutorial_ = nullptr;
	Model* modelStage1_ = nullptr;
	Model* modelStage2_ = nullptr;

	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	std::map<std::string, Model*> models_;
	std::vector<Object3d*> objects_;

	bool isStart_ = false;
	bool isMenu_ = false;
	Easing easeTitlePosX_ = Easing(0.0f, -1300.0f, 1.0f);

	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//メニュー
		Easing(1300.0f, 0.0f, 1.2f),//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),//タイトルへ
		Easing(1300.0f, 0.0f, 1.8f),//スペースで選択
	};
	int menuCount_ = 0;
	//色を変えるスピード
	float speedColor_ = 0.0f;
	//色反転フラグ
	bool isColorReverse_ = false;

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//モデル読み込み
	//void LoadModel();

};