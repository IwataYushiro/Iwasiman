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

/*

*	TitleScene.h

*	タイトルシーン

*/
class TitleScene :public BaseScene
{
public://メンバ関数(ステージ番号)
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
	//フェードアウト(色)
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

	//選択しているメニュー表示
	int menuCount_;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();				//タイトル画面スプライト
	Sprite* spriteTitleDone_ = new Sprite();			//タイトル決定表示スプライト
	Sprite* spriteMenu_ = new Sprite();					//タイトルメニュー画面スプライト
	Sprite* spriteMenuTutorial_ = new Sprite();			//チュートリアル表示スプライト
	Sprite* spriteMenuStageSelect_ = new Sprite();		//ステージセレクト表示スプライト
	Sprite* spriteMenuDone_ = new Sprite();				//タイトルメニュー決定表示スプライト
	Sprite* spriteBack_ = new Sprite();					//タイトルメニュー→タイトル移行のスプライト
	Sprite* spriteFadeInOut_ = new Sprite();			//フェードインアウトスプライト

	//jsonレベルデータ
	LevelData* levelData_ = nullptr;

	//モデル
	Model* modelPlayer_ = nullptr;						//自機モデル
	Model* modelSkydome_ = nullptr;						//チュートリアルステージモデル(天球)
	Model* modelSkydomeStage1_ = nullptr;				//ステージ1モデル(天球)
	Model* modelSkydomeStage2_ = nullptr;				//ステージ2モデル(天球)
	Model* modelGround_ = nullptr;						//床モデル
	Model* modelGoal_ = nullptr;						//ゴールモデル

	std::vector<Object3d*> objPlayers_;					//自機オブジェクト配列
	std::vector<Object3d*> objSkydomes_;				//天球オブジェクト配列
	std::vector<Object3d*> objGrounds_;					//床オブジェクト配列
	std::vector<Object3d*> objGoals_;					//ゴールオブジェクト配列

	//マッピングモデル
	std::map<std::string, Model*> models_;

	//フラグ類
	bool isMenu_ = false;				//タイトルメニュー画面にいるとき
	bool isBack_ = false;				//タイトルメニューからタイトルに戻るとき
	bool isStartGame_ = false;			//ゲーム開始するとき
	bool isStageSelect_ = false;		//ステージセレクトに行くとき
	bool isFadeOut_ = false;			//フェードアウト

	//タイトル→タイトルメニューのイージング
	Easing easeTitlePosX_[2] =
	{
		Easing(225.0f, -1300.0f, 1.0f),
		Easing(300.0f, -1300.0f, 1.0f)
	};

	//タイトルメニューの出現イージング
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),		//メニュー
		Easing(1300.0f, 0.0f, 1.2f),		//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),		//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),		//スペースで選択
		Easing(1300.0f, 900.0f, 1.8f),		//戻る
	};

	//タイトル→タイトルメニューの視点カメラワークイージング
	Easing easeEyeMenu_[3]
	{
		Easing(0.0f, 21.0f, 1.8f),			//X
		Easing(1.0f, -4.0f, 1.8f),			//Y
		Easing(-110.0f, -60.0f, 1.8f),		//Z
	};
	//タイトル→タイトルメニューの注視点カメラワークイージング
	Easing easeTargetMenu_[3]
	{
		Easing(0.0f, -100.0f, 1.8f),		//X
		Easing(0.0f, -10.0f, 1.8f),			//Y
		Easing(-10.0f, -62.0f, 1.8f),		//Z
	};

	//タイトルメニュー→ゲーム開始の視点カメラワークイージング
	Easing easeEyeGameStart_[3]
	{
		Easing(21.0f, -22.0f, 1.0f),		//X
		Easing(-4.0f, -1.0f, 1.0f),			//Y
		Easing(-60.0f, -60.0f, 1.0f),		//Z
	};
	//タイトルメニュー→ゲーム開始の注視点カメラワークイージング
	Easing easeTargetGameStart_[3]
	{
		Easing(-100.0f, 50.0f, 1.0f),		//X
		Easing(-10.0f, -8.0f, 1.0f),		//Y
		Easing(-62.0f, -57.0f, 1.0f),		//Z
	};
	//タイトルメニュー→ステージセレクトの自機移動イージング
	Easing easePlayerMove_[3]
	{
		Easing(0.0f, 150.0f, 2.0f),			//X
		Easing(-8.0f, 40.0f, 2.0f),			//Y
		Easing(-60.0f, -60.0f, 2.0f),		//Z
	};

	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//選択中の色
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb
	//選択されていない色
	const DirectX::XMFLOAT4 otherMenuColor_ = { 0.0f,0.0f,0.0f,1.0f };
	//タイトルメニュー→タイトルへ戻るスプライトの色
	const DirectX::XMFLOAT4 backTitleColor_ = { 0.0f,0.0f,0.1f,1.0f };

	//色反転フラグ
	bool isColorReverse_ = false;
	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pm1_ = nullptr;

	//開始時のポジション
	DirectX::XMFLOAT3 startPos_;
	
	//どのステージにいるのかを受け取るための変数
	int stageNum_;

public:
	//色が変わる処理
	void UpdateChangeColor();

};
