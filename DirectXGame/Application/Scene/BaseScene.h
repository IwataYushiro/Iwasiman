#pragma once
#include <DirectXMath.h>
//前方宣言
//シーンマネージャー
class SceneManager;

/*

*	BaseScene.h (cpp X)

*	シーン基盤

*/
class BaseScene
{
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
