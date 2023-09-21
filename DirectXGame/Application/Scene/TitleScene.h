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

#include <map>

#include "SceneManager.h"

//jsonレベルデータ
struct LevelData;

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

	//レベルデータ読み込み
	void LoadLVData();

private://静的メンバ変数
	//DirectX基盤
	static DirectXCommon* dxCommon_;
	//スプライト基盤
	SpriteCommon* spCommon_=nullptr;
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

	Sprite* spriteMenu_ = new Sprite();
	Sprite* spriteMenuTutorial_ = new Sprite();
	Sprite* spriteMenuStageSelect_ = new Sprite();
	Sprite* spriteMenuDone_ = new Sprite();

	//FBX
	ModelFbx* modelF = nullptr;
	ObjectFbx* objF = nullptr;

	//モデル
	Model* modelPlayer_ = nullptr;
	Object3d* object3DPlayer_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;
	Model* modelGround = nullptr;
	Model* modelFighter = nullptr;
	Model* modelSphere = nullptr;

	Object3d* objSkydome = nullptr;
	Object3d* objGround = nullptr;
	Object3d* objFighter = nullptr;
	Object3d* objSphere = nullptr;
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	bool isStart = false;
	bool isMenu = false;
	Easing easeTitlePosX = Easing(0.0f, -1300.0f, 1.0f);

	Easing easeMenuPosX[4] = 
	{
		Easing(1300.0f, 0.0f, 1.0f),//メニュー
		Easing(1300.0f, 0.0f, 1.2f),//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),//スペースで選択
	};
	int MenuCount = 0;
	
	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
};
