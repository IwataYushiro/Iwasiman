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

//ステージセレクトシーン
class StageSelectScene :public BaseScene
{
public://メンバ関数
	StageSelectScene(int count);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//ステージセレクト中
	void UpdateIsStageSelect();
	//決定時
	void UpdateIsDone();
	//決定からのゲームスタート
	void UpdateIsGameStart();
	//タイトルへ戻るとき 
	void UpdateIsQuitTitle();

	//描画
	void Draw() override;
	//終了
	void Finalize() override;
	//フェードアウト
	void FadeOut(DirectX::XMFLOAT3 rgb);
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

	//フェードインアウトセット
	Sprite* spriteFadeInOut_ = new Sprite();
	bool isFadeOut_ = false;
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	LevelData* levelData_ = nullptr;

	//オブジェクト
	Object3d* objStage_ = nullptr;
	std::vector<Object3d*> objPlayers_;
	std::vector<Object3d*> objGoals_;
	std::vector<Object3d*> objGrounds_;

	//モデル
	Model* modelPlayer_ = nullptr;
	Model* modelGoal_ = nullptr;
	Model* modelGround_ = nullptr;
	Model* modelStageTutorial_ = nullptr;
	Model* modelStage1_ = nullptr;
	Model* modelStage2_ = nullptr;
	std::map<std::string, Model*> models_;

	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	
	std::vector<Object3d*> objects_;

	Easing easeMenuPosX_[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//メニュー
		Easing(1300.0f, 0.0f, 1.2f),//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),//スペースで選択
		Easing(1300.0f, 0.0f, 1.8f),//スペースで選択
		Easing(1300.0f, 900.0f, 2.0f),//戻る
	};


	Easing easeEyeStageSelect_[3]
	{
		Easing(-22.0f, 144.0f, 1.0f),//X
		Easing(-1.0f, 45.0f, 1.0f),//Y
		Easing(-60.0f, -98.0f, 1.0f),//Z
	};
	Easing easeTargetStageSelect_[3]
	{
		Easing(50.0f, 132.0f, 1.0f),//X
		Easing(-8.0f, 39.0f, 1.0f),//Y
		Easing(-57.0f, -52.0f, 1.0f),//Z
	};

	Easing easeEyeDoneMenu_[3]
	{
		Easing(144.0f, 127.0f, 1.0f),//X
		Easing(45.0f, 52.0f, 1.0f),//Y
		Easing(-98.0f, -60.0f, 1.0f),//Z
	};
	Easing easeTargetDoneMenu_[3]
	{
		Easing(132.0f, 192.0f, 1.0f),//X
		Easing(39.0f, 33.0f, 1.0f),//Y
		Easing(-52.0f, -60.0f, 1.0f),//Z
	};

	Easing easeEyeGameStart_[3]
	{
		Easing(127.0f, 227.0f, 2.0f),//X
		Easing(52.0f, 55.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};
	Easing easeTargetGameStart_[3]
	{
		Easing(192.0f, 292.0f, 2.0f),//X
		Easing(33.0f, 33.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};

	Easing easePlayerStartMove_[3] =
	{
		Easing(150.0f, 250.0f, 2.0f),//X
		Easing(40.0f, 40.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};

	Easing easePlayerQuitMove_[3] =
	{
		Easing(150.0f, 0.0f, 2.0f),//X
		Easing(40.0f, -8.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};

	int menuCount_ = 0;
	//色を変えるスピード
	float speedColor_ = 0.0f;
	//色反転フラグ
	bool isColorReverse_ = false;

	//遷移フラグ類
	//ステージセレクト時
	bool isStageSelect_ = true;
	//ゲームスタート時
	bool isStart_ = false;
	//ステージ決定時
	bool isDone_ = false;
	//ステージセレクトから抜ける
	bool outStageSelect_ = false;

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//モデル読み込み
	//void LoadModel();

};
