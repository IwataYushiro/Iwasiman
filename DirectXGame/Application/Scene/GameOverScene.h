#pragma once
#include "Audio.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"
#include "Earth.h"

#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <DirectXMath.h>

#include "SceneManager.h"

//jsonレベルデータ
struct LevelData;

//タイトルシーン
class GameOverScene :public BaseScene
{
private:
	// DirectX::を省略
	using XMFLOAT3 = DirectX::XMFLOAT3;

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
	Audio::SoundData BGM;
	Audio::SoundData doneSE;
	int stageNum;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();

	std::list<std::unique_ptr<Earth>> earths_;
	Model* modelEarth_ = nullptr;
	
	Model* modelEnemy1_ = nullptr;
	Model* modelEnemy1Power_ = nullptr;
	Model* modelEnemy1Guard_ = nullptr;
	Model* modelEnemy1Speed_ = nullptr;
	Model* modelEnemy1Death_ = nullptr;
	Model* modelEnemyBullet_ = nullptr;

	Model* modelEnemy2_ = nullptr;
	Model* modelEnemy2Power_ = nullptr;
	Model* modelEnemy2Guard_ = nullptr;
	Model* modelEnemy2Speed_ = nullptr;
	Model* modelEnemy2Death_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
	//ライト
	LightGroup* lightGroup_ = nullptr;
	//モデル読み込み
	void LoadModel();
};


