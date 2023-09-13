#pragma once
#include "Audio.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"

#include "ParticleManager.h"
#include "Sprite.h"

#include "Player.h"
#include "Earth.h"

#include <vector>
#include <map>
#include <sstream>
#include <string>

#include "Easing.h"
#include "SceneManager.h"
#include <DirectXMath.h>
#include <chrono>

//jsonレベルデータ
struct LevelData;

//タイトルシーン
class TitleScene :public BaseScene
{
private:
	// DirectX::を省略
	using XMFLOAT3 = DirectX::XMFLOAT3;
public://定数
	//int MAX_TIME = 120;

public://メンバ関数
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;

	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//レベルデータ読み込み
	void LoadLVData(const std::string& stagePath);

private://静的メンバ変数
	//DirectX基盤
	static DirectXCommon* dxCommon_;
	//スプライト基盤
	SpriteCommon* spCommon_=nullptr;
	//インプット
	static Input* input_;
	//オーディオ
	Audio* audio_ = nullptr;
	//シーンマネージャー
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;
	//カメラ
	static Camera* camera_;

private://メンバ変数
	//音
	Audio::SoundData titleBGM;
	Audio::SoundData doneSE;
	Audio::SoundData startSE;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteHTP_ = new Sprite();
	//モデル
	std::list<std::unique_ptr<Player>> players_;
	Model* modelPlayer_ = nullptr;
	Model* modelPlayerBullet_ = nullptr;

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

	std::list<std::unique_ptr<Earth>> earths_;
	Model* modelEarth_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;
	Model* modelGround = nullptr;
	Model* modelBox = nullptr;
	Model* modelRail = nullptr;
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	//自機弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;

	//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	float	maxTime = 2.0f;					//全体時間
	float	timeRate;
	//制御点(視点)
	XMFLOAT3 startEye;
	XMFLOAT3 p1Eye;
	XMFLOAT3 p2Eye;
	XMFLOAT3 endEye;
	//制御点(注視点)
	XMFLOAT3 startTarget;
	XMFLOAT3 p1Target;
	XMFLOAT3 p2Target;
	XMFLOAT3 endTarget;

	bool isStart = false;
	bool isHTP = false;
	Easing easeTitlePosY = Easing(0.0f, 800.0f, 1.0f);

	Easing easeHTPPosY = Easing(800.0f, 0.0f, 1.0f);
	//ライト
	LightGroup* lightGroup_ = nullptr;
	//パーティクル
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//モデル読み込み
	void LoadModel();
};
