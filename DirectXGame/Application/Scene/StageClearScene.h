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

*	StageClearScene.h

*	ステージクリアシーン

*/
class StageClearScene :public BaseScene
{
public://メンバ関数
	//コンストラクタ(ステージ番号)
	StageClearScene(int stagenum);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//状態更新(ゲーム開始するとき)
	void UpdateIsNextStage();
	//状態更新(ステージセレクトへ遷移するとき)
	void UpdateIsStageSelect();
	//状態更新(タイトルへ戻るとき)
	void UpdateIsQuitTitle();
	//状態更新(メニューのとき) 
	void UpdateIsMenu();
	//描画
	void Draw() override;
	//フェードアウト
	void FadeOut(DirectX::XMFLOAT3 rgb);
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
	
	//ステージ番号
	int stageNum_;
	//メニュー番号
	int menuCount_ = 0;
	//Sprite
	Sprite* spriteStageClear_ = new Sprite();	//ステージクリア時のスプライト
	Sprite* spriteNextStage_ = new Sprite();	//次のステージ表示のスプライト
	Sprite* spriteStageSelect_ = new Sprite();	//ステージセレクト表示のスプライト
	Sprite* spriteTitle_ = new Sprite();		//タイトル表示のスプライト
	Sprite* spriteDone_ = new Sprite();			//決定表示のスプライト
	Sprite* spriteFadeInOut_ = new Sprite();	//フェードインアウトのスプライト
	
	//jsonレベルデータ
	LevelData* levelData_ = nullptr;			

	//モデル
	Model* modelPlayer_ = nullptr;				//自機モデル
	Model* modelSkydome_ = nullptr;				//チュートリアルステージモデル(天球)
	Model* modelSkydomeStage1_ = nullptr;		//ステージ1モデル(天球)
	Model* modelSkydomeStage2_ = nullptr;		//ステージ2モデル(天球)
	Model* modelGround_ = nullptr;				//床のモデル
	Model* modelGoal_ = nullptr;				//ゴールモデル

	std::vector<Object3d*> objPlayers_;			//自機配列
	std::vector<Object3d*> objSkydomes_;		//天球配列
	std::vector<Object3d*> objGrounds_;			//床配列
	std::vector<Object3d*> objGoals_;			//ゴール配列

	//マッピングモデル
	std::map<std::string, Model*> models_;

	//フラグ類
	bool isFinalStage_ = false;					//最終面の場合
	bool isNextStage_ = false;					//次のステージへ行く場合
	bool isStageSelect_ = false;				//ステージセレクトへ行く場合
	bool isQuitTitle_ = false;					//タイトルに戻る場合
	bool isFadeOut_ = false;					//フェードインアウト

	//イージング類
	//メニュー画面出現イージング
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),			//メニュー
		Easing(1300.0f, 0.0f, 1.2f),			//チュートリアルへ
		Easing(1300.0f, 0.0f, 1.4f),			//ステージセレクトへ
		Easing(1300.0f, 0.0f, 1.6f),			//タイトルへ
		Easing(1300.0f, 0.0f, 1.8f),			//スペースで選択
	};
	//次のステージへ行くときの視点イージング
	Easing easeEyeStageClear_[3]
	{
		Easing(0.0f, -22.0f, 1.8f),				//X
		Easing(1.0f, -1.0f, 1.8f),				//Y
		Easing(-110.0f, -60.0f, 1.8f),			//Z
	};
	//次のステージへ行くときの注視点イージング
	Easing easeTargetStageClear_[3]
	{
		Easing(0.0f, 50.0f, 1.8f),				//X
		Easing(0.0f, -8.0f, 1.8f),				//Y
		Easing(-10.0f, -57.0f, 1.8f),			//Z
	};
	//ステージセレクトへ行くときの自機移動イージング
	Easing easePlayerMoveStageSelect_[3]
	{
		Easing(0.0f, 150.0f, 2.0f),				//X
		Easing(-8.0f, 40.0f, 2.0f),				//Y
		Easing(-60.0f, -60.0f, 2.0f),			//Z
	};
	//フェードインアウト(false フェードイン、true フェードアウト)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//選択中の色
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb

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

public:
	//色が変わる処理
	void UpdateChangeColor();

};
