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
	enum TextureIndex
	{
		TITitle = 0,
		TITitleDone = 1,
		TIMenu = 2,
		TIMenuTutorial = 3,
		TIMenuStageSerect = 4,
		TIMenuDone = 5,
		TIBackTitle = 6,
	};

	enum MenuIndex
	{
		Tutorial = 0,
		StageSelect = 1,
	};
	int MenuCount = Tutorial;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteTitleDone_ = new Sprite();
	Sprite* spriteMenu_ = new Sprite();
	Sprite* spriteMenuTutorial_ = new Sprite();
	Sprite* spriteMenuStageSelect_ = new Sprite();
	Sprite* spriteMenuDone_ = new Sprite();
	Sprite* spriteBack_ = new Sprite();

	//FBX
	//ModelFbx* modelF = nullptr;
	//ObjectFbx* objF = nullptr;

	LevelData* levelData = nullptr;

	//モデル
	Model* modelPlayer_ = nullptr;
	Model* modelSkydome = nullptr;
	Model* modelGround = nullptr;

	std::vector<Object3d*> objPlayers_;
	std::vector<Object3d*> objSkydomes_;
	std::vector<Object3d*> objGrounds_;

	std::map<std::string, Model*> models;

	bool isStart = false;
	bool isMenu = false;
	bool isBack = false;
	Easing easeTitlePosX[2] =
	{
		Easing(0.0f, -1300.0f, 1.0f),
		Easing(0.0f, -1300.0f, 1.0f)
	};
		

	Easing easeMenuPosX[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//メニュー
		Easing(1300.0f, 0.0f, 1.2f),//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),//スペースで選択
		Easing(1300.0f, 900.0f, 1.8f),//戻る
	};

	Easing easeEye[3]
	{
		Easing(0.0f, 21.0f, 1.8f),//X
		Easing(1.0f, -4.0f, 1.8f),//Y
		Easing(-50.0f, 0.0f, 1.8f),//Z
	};
	Easing easeTarget[3]
	{
		Easing(0.0f, -100.0f, 1.8f),//X
		Easing(0.0f, -10.0f, 1.8f),//Y
		Easing(50.0f, -2.0f, 1.8f),//Z
	};
	//選択中の色
	DirectX::XMFLOAT3 selectColor = { 0.0f,0.0f,0.0f };//xyz=rgb

	const DirectX::XMFLOAT4 otherMenuColor = { 0.0f,0.0f,0.0f,1.0f };
	const DirectX::XMFLOAT4 backTitleColor = { 0.0f,0.0f,0.1f,1.0f };

	//色反転フラグ
	bool isColorReverse_ = false;
	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;

	//開始時のポジション
	DirectX::XMFLOAT3 startPos;
	//X値がここまで来たらループ
	const float returnPos = -120.0f;

	//天球回転用
	DirectX::XMFLOAT3 rotSkydome{ 0.0f,0.0f,0.0f };

public:
	//色が変わる処理
	void UpdateChangeColor();

};
