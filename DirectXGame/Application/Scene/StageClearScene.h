#pragma once
#include "Audio.h"
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

//ステージクリアシーン
class StageClearScene :public BaseScene
{
public://メンバ関数
	StageClearScene(int stagenum);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;


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
	Sprite* spriteStageClear_ = new Sprite();
	
	//ライト
	LightGroup* lightGroup_ = nullptr;
	
};
