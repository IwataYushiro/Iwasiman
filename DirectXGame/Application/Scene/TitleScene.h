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
	TitleScene(int stagenum);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//状態更新(ゲーム開始するとき)
	void UpdateIsStartGame();
	//状態更新(ステージセレクトへ遷移するとき)
	void UpdateIsStageSelect();
	//状態更新(スタートに戻ったとき)
	void UpdateIsBack();
	//状態更新(メニューのとき) 
	void UpdateIsMenu();
	//フェードアウト
	void FadeOut(DirectX::XMFLOAT3 rgb);
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
	
	int menuCount_;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteTitleDone_ = new Sprite();
	Sprite* spriteMenu_ = new Sprite();
	Sprite* spriteMenuTutorial_ = new Sprite();
	Sprite* spriteMenuStageSelect_ = new Sprite();
	Sprite* spriteMenuDone_ = new Sprite();
	Sprite* spriteBack_ = new Sprite();
	Sprite* spriteFadeInOut_ = new Sprite();

	//FBX
	//ModelFbx* modelF = nullptr;
	//ObjectFbx* objF = nullptr;

	LevelData* levelData_ = nullptr;

	//モデル
	Model* modelPlayer_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelSkydomeStage1_ = nullptr;
	Model* modelSkydomeStage2_ = nullptr;
	Model* modelGround_ = nullptr;
	Model* modelGoal_ = nullptr;

	std::vector<Object3d*> objPlayers_;
	std::vector<Object3d*> objSkydomes_;
	std::vector<Object3d*> objGrounds_;
	std::vector<Object3d*> objGoals_;

	std::map<std::string, Model*> models_;

	//フラグ類
	bool isMenu_ = false;
	bool isBack_ = false;
	bool isStartGame_ = false;
	bool isStageSelect_ = false;
	bool isFadeOut_ = false;

	Easing easeTitlePosX_[2] =
	{
		Easing(0.0f, -1300.0f, 1.0f),
		Easing(0.0f, -1300.0f, 1.0f)
	};

	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//メニュー
		Easing(1300.0f, 0.0f, 1.2f),//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),//スペースで選択
		Easing(1300.0f, 900.0f, 1.8f),//戻る
	};

	Easing easeEyeMenu_[3]
	{
		Easing(0.0f, 21.0f, 1.8f),//X
		Easing(1.0f, -4.0f, 1.8f),//Y
		Easing(-110.0f, -60.0f, 1.8f),//Z
	};
	Easing easeTargetMenu_[3]
	{
		Easing(0.0f, -100.0f, 1.8f),//X
		Easing(0.0f, -10.0f, 1.8f),//Y
		Easing(-10.0f, -62.0f, 1.8f),//Z
	};

	Easing easeEyeGameStart_[3]
	{
		Easing(21.0f, -22.0f, 1.0f),//X
		Easing(-4.0f, -1.0f, 1.0f),//Y
		Easing(-60.0f, -60.0f, 1.0f),//Z
	};
	Easing easeTargetGameStart_[3]
	{
		Easing(-100.0f, 50.0f, 1.0f),//X
		Easing(-10.0f, -8.0f, 1.0f),//Y
		Easing(-62.0f, -57.0f, 1.0f),//Z
	};

	Easing easePlayerMove_[3]
	{
		Easing(0.0f, 150.0f, 3.0f),//X
		Easing(-8.0f, 40.0f, 3.0f),//Y
		Easing(-60.0f, -60.0f, 3.0f),//Z
	};

	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//選択中の色
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb

	const DirectX::XMFLOAT4 otherMenuColor_ = { 0.0f,0.0f,0.0f,1.0f };
	const DirectX::XMFLOAT4 backTitleColor_ = { 0.0f,0.0f,0.1f,1.0f };

	//色反転フラグ
	bool isColorReverse_ = false;
	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;

	//開始時のポジション
	DirectX::XMFLOAT3 startPos_;
	
	//どのステージにいるのかを受け取るための変数
	int stageNum_;

public:
	//色が変わる処理
	void UpdateChangeColor();

};
