#pragma once
#include <DirectXMath.h>
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "LevelLoaderJson.h"
#include "Sprite.h"
#include "ParticleManager.h"

//前方宣言
//シーンマネージャー
class SceneManager;

/*

*	BaseScene.h (cpp X)

*	シーン基盤

*/
class BaseScene
{
protected://エイリアス
	using DirectXCommon = IwasiEngine::DirectXCommon;
	using Input = IwasiEngine::Input;
	using Audio = IwasiEngine::Audio;
	using ImGuiManager = IwasiEngine::ImGuiManager;
	using Sprite = IwasiEngine::Sprite;
	using SpriteCommon = IwasiEngine::SpriteCommon;
	using Model = IwasiEngine::Model;
	using Object3d = IwasiEngine::Object3d;
	using Camera = IwasiEngine::Camera;
	using Easing = IwasiEngine::Easing;
	using LevelData = IwasiEngine::LevelData;
	using LightGroup = IwasiEngine::LightGroup;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;

public://メンバ関数
	virtual~BaseScene() = default;

	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;
	//終了処理
	virtual void Finalize() = 0;

protected://継承メンバ変数
	//現在ステージのポジション
	const DirectX::XMFLOAT2 stageInfoNowPos_ = { 300.0f,100.0f };
	//ローディング中のポジション
	const DirectX::XMFLOAT2 loadPos_ = { 350.0f,600.0f };
	//ステージ名のポジション
	const DirectX::XMFLOAT2 stageNamePos_ = { 630.0f,500.0f };
	//色サンプル
	const DirectX::XMFLOAT3 white_ = { 1.0f,1.0f,1.0f };	//白
	const DirectX::XMFLOAT3 black_ = { 0.0f,0.0f,0.0f };	//黒
	const DirectX::XMFLOAT3 deepRed_ = { 0.2f,0.0f,0.0f };	//濃い赤
	//アンカーポイントのプリセット
	const DirectX::XMFLOAT2 ANCHOR_POINT_CENTRAL = { 0.5f,0.5f };//中央

private://メンバ変数
	//シーンマネージャー(借りてくるのでここでdeleteはダメゼッタイ)
	SceneManager* sceneManager_ = nullptr;

public://アクセッサ置き場
	virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager_ = sceneManager; }
};
