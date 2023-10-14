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

//前方宣言
//jsonレベルデータ
struct LevelData;

/*

*	GameOverScene.h

*	ゲームオーバーシーン

*/
class GameOverScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ情報)
	GameOverScene(int stagenum);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;
	//レベルデータ読み込み(ステージファイルパス)
	void LoadLVData(const std::string& stagePath);
	//フェードアウト
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//色が変わる処理
	void UpdateChangeColor();
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

	//ステージ番号
	int stageNum_;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();			//ゲームオーバースプライト
	Sprite* spriteContinue_ = new Sprite();			//コンティニュー表示スプライト
	Sprite* spriteStageSelect_ = new Sprite();		//ステージセレクト表示スプライト
	Sprite* spriteTitle_ = new Sprite();			//タイトル表示スプライト
	Sprite* spriteDone_ = new Sprite();				//決定スプライト
	Sprite* spriteFadeInOut_ = new Sprite();		//フェードインアウトスプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;
	
	//モデル
	Model* modelPlayer_ = nullptr;				//自機モデル
	Model* modelGoal_ = nullptr;				//ゴールモデル
	Model* modelStageTutorial_ = nullptr;		//チュートリアルステージモデル(天球)
	Model* modelStage1_ = nullptr;				//ステージ1モデル(天球)
	Model* modelStage2_ = nullptr;				//ステージ2モデル(天球)
	
	std::vector<Object3d*> objPlayers_;			//自機オブジェクト配列
	std::vector<Object3d*> objGoals_;			//ゴールオブジェクト配列
	std::vector<Object3d*> objStages_;			//ステージオブジェクト配列

	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	//マッピングモデル
	std::map<std::string, Model*> models_;
	//オブジェクト配列
	std::vector<Object3d*> objects_;

	//スタート時フラグ
	bool isMenu_ = false;						//メニュー時フラグ
	bool isContinue_ = false;					//コンティニューする場合
	bool isStageSelect_ = false;				//ステージセレクトへ行く場合
	bool isQuitTitle_ = false;					//タイトルに戻る場合
	bool isFadeOut_ = false;					//フェードインアウト
	
	//メニュー表示用のイージング
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),			//メニュー
		Easing(1300.0f, 0.0f, 1.2f),			//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),			//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),			//タイトルへ
		Easing(1300.0f, 0.0f, 1.8f),			//スペースで選択
	};
	//コンティニューするときの視点イージング
	Easing easeEyeContinue_[3]
	{
		Easing(-6.0f, -22.0f, 1.8f),				//X
		Easing(-8.0f, -1.0f, 1.8f),				//Y
		Easing(-110.0f, -60.0f, 1.8f),			//Z
	};
	//コンティニューするときの注視点イージング
	Easing easeTargetContinue_[3]
	{
		Easing(-32.0f, 50.0f, 1.8f),				//X
		Easing(-24.0f, -8.0f, 1.8f),				//Y
		Easing(-10.0f, -57.0f, 1.8f),			//Z
	};
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//選択中の色
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb
	//選択しているメニュー表示
	int menuCount_ = 0;
	//色反転フラグ
	bool isColorReverse_ = false;

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pm1_ = nullptr;
	

};