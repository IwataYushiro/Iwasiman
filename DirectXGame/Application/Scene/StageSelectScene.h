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

*	StageSelectScene.h

*	ステージセレクトシーン

*/
class StageSelectScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ番号や天球背景と連動した値)
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
	
	//インプット
	static Input* input_;
	//オーディオ
	static Audio* audio_;
	//シーンマネージャー
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;
	
private://メンバ変数

	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//Sprite
	Sprite* spriteMenu_ = new Sprite();				//メニュー画面スプライト
	Sprite* spriteTutorial_ = new Sprite();			//チュートリアルステージスプライト
	Sprite* spriteStage1_ = new Sprite();			//ステージ1スプライト
	Sprite* spriteStage2_ = new Sprite();			//ステージ2スプライト
	Sprite* spriteDone_ = new Sprite();				//決定表示スプライト
	Sprite* spriteBack_ = new Sprite();				//タイトルに戻るスプライト
	Sprite* spriteFadeInOut_ = new Sprite();		//フェードインアウトスプライト
	Sprite* spriteLoad_ = new Sprite();				//ロードスプライト
	Sprite* spriteStageInfoNow_ = new Sprite();		//現在ステージスプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//オブジェクト
	Object3d* objStage_ = nullptr;				//ステージオブジェクト
	std::vector<Object3d*> objPlayers_;			//自機オブジェクト配列
	std::vector<Object3d*> objGoals_;			//ゴールオブジェクト配列
	std::vector<Object3d*> objGrounds_;			//床オブジェクト配列

	//モデル
	Model* modelPlayer_ = nullptr;				//自機モデル
	Model* modelGoal_ = nullptr;				//ゴールモデル
	Model* modelGround_ = nullptr;				//床モデル
	Model* modelStageTutorial_ = nullptr;		//チュートリアルステージモデル(天球)
	Model* modelStage1_ = nullptr;				//ステージ1モデル(天球)
	Model* modelStage2_ = nullptr;				//ステージ2モデル(天球)
	//マッピングモデル
	std::map<std::string, Model*> models_;		
	
	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	//イージング類
	//メニュー画面出現イージング
	Easing easeMenuPosX_[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),				//メニュー
		Easing(1300.0f, 100.0f, 1.2f),				//チュートリアル
		Easing(1300.0f, 100.0f, 1.4f),				//ステージ1
		Easing(1300.0f, 100.0f, 1.6f),				//ステージ2
		Easing(1300.0f, 0.0f, 1.8f),				//スペースで選択
		Easing(1300.0f, 900.0f, 2.0f),				//戻る
	};
	//選んだステージを真ん中に移動させるイージング
	Easing easeStartStagePosX_[3] =
	{
		Easing(100.0f, 200.0f, 1.5f),				//チュートリアルへ
		Easing(100.0f, 400.0f, 1.5f),				//ステージ1へ
		Easing(100.0f, 350.0f, 1.5f),				//ステージ2へ
	};
	//選んだステージを上に移動させるイージング
	Easing easeStartStagePosY_[3] =
	{
		Easing(150.0f, 0.0f, 1.5f),				//チュートリアルへ
		Easing(300.0f, 0.0f, 1.5f),				//ステージ1へ
		Easing(450.0f, 0.0f, 1.5f),				//ステージ2へ
	};
	//遷移時すぐに起動するステージセレクト視点カメラワークイージング
	Easing easeEyeStageSelect_[3]
	{
		Easing(-22.0f, 144.0f, 1.0f),				//X
		Easing(-1.0f, 45.0f, 1.0f),					//Y
		Easing(-60.0f, -98.0f, 1.0f),				//Z
	};
	//遷移時すぐに起動するステージセレクト注視点カメラワークイージング
	Easing easeTargetStageSelect_[3]
	{
		Easing(50.0f, 132.0f, 1.0f),				//X
		Easing(-8.0f, 39.0f, 1.0f),					//Y
		Easing(-57.0f, -52.0f, 1.0f),				//Z
	};
	//ステージ決定時の視点カメラワークイージング
	Easing easeEyeDoneMenu_[3]
	{
		Easing(144.0f, 127.0f, 1.0f),				//X
		Easing(45.0f, 52.0f, 1.0f),					//Y
		Easing(-98.0f, -60.0f, 1.0f),				//Z
	};
	//ステージ決定時の注視点カメラワークイージング
	Easing easeTargetDoneMenu_[3]
	{
		Easing(132.0f, 192.0f, 1.0f),				//X
		Easing(39.0f, 33.0f, 1.0f),					//Y
		Easing(-52.0f, -60.0f, 1.0f),				//Z
	};
	//ステージ決定時の視点カメラワークイージングが終わった後の突入視点カメラワークイージング
	Easing easeEyeGameStart_[3]
	{
		Easing(127.0f, 227.0f, 2.0f),				//X
		Easing(52.0f, 55.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};
	//ステージ決定時の視点カメラワークイージングが終わった後の突入注視点カメラワークイージング
	Easing easeTargetGameStart_[3]
	{
		Easing(192.0f, 292.0f, 2.0f),				//X
		Easing(33.0f, 33.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};
	//ステージ決定時の視点カメラワークイージングが終わった後の自機移動イージング
	Easing easePlayerStartMove_[3] =
	{
		Easing(150.0f, 250.0f, 2.0f),				//X
		Easing(40.0f, 40.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};
	//タイトルに戻るときの自機移動イージング
	Easing easePlayerQuitMove_[3] =
	{
		Easing(150.0f, 0.0f, 2.0f),					//X
		Easing(40.0f, -8.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};

	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//選択しているメニュー表示
	int menuCount_ = 0;
	//色を変えるスピード
	float speedColor_ = 0.0f;
	//色反転フラグ
	bool isColorReverse_ = false;

	//遷移フラグ類
	bool isStageSelect_ = true;		//ステージセレクト時
	bool isStart_ = false;			//ゲームスタート時
	bool isDone_ = false;			//ステージ決定時
	bool outStageSelect_ = false;	//ステージセレクトから抜ける
	bool isFadeOut_ = false;		//フェードアウト

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pm1_ = nullptr;

};
