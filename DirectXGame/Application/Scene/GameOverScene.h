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
	//ステージ番号
	int stageNum_;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();			//ゲームオーバースプライト
	Sprite* spriteContinue_ = new Sprite();			//コンティニュー表示スプライト
	Sprite* spriteStageSelect_ = new Sprite();		//ステージセレクト表示スプライト
	Sprite* spriteTitle_ = new Sprite();			//タイトル表示スプライト
	Sprite* spriteDone_ = new Sprite();				//決定スプライト

	//FBX
	//ModelFbx* modelF_ = nullptr;
	//ObjectFbx* objF_ = nullptr;

	//モデル
	//Model* modelPlayer_ = nullptr;
	//Object3d* object3DPlayer_ = nullptr;

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;
	//ステージオブジェクト
	Object3d* objStage_ = nullptr;

	Model* modelStageTutorial_ = nullptr;	//チュートリアルステージモデル(天球)
	Model* modelStage1_ = nullptr;			//ステージ1モデル(天球)
	Model* modelStage2_ = nullptr;			//ステージ2モデル(天球)

	//オブジェクト回転用
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	//マッピングモデル
	std::map<std::string, Model*> models_;
	//オブジェクト配列
	std::vector<Object3d*> objects_;

	//スタート時フラグ
	bool isStart_ = false;
	//メニュー時フラグ
	bool isMenu_ = false;
	
	//メニュー表示用のイージング
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),			//メニュー
		Easing(1300.0f, 0.0f, 1.2f),			//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),			//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),			//タイトルへ
		Easing(1300.0f, 0.0f, 1.8f),			//スペースで選択
	};
	//選択しているメニュー表示
	int menuCount_ = 0;
	//色を変えるスピード
	float speedColor_ = 0.0f;
	//色反転フラグ
	bool isColorReverse_ = false;

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pm1_ = nullptr;
	

};