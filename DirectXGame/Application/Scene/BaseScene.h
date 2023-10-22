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
	DirectX::XMFLOAT2 stageInfoNowPos_ = { 300.0f,100.0f };
	//ローディング中のポジション
	DirectX::XMFLOAT2 loadPos_ = { 350.0f,600.0f };
	//色サンプル
	DirectX::XMFLOAT3 white_ = { 1.0f,1.0f,1.0f };	//白
	DirectX::XMFLOAT3 black_ = { 0.0f,0.0f,0.0f };	//黒
	DirectX::XMFLOAT3 deepRed_ = { 0.2f,0.0f,0.0f };	//濃い赤

private://メンバ変数
	//シーンマネージャー(借りてくるのでここでdeleteはダメゼッタイ)
	SceneManager* sceneManager_ = nullptr;

public://アクセッサ置き場
	virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager_ = sceneManager; }
};
