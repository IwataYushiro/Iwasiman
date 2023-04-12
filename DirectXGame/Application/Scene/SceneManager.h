#pragma once
#include "BaseScene.h"

class Scenemanager
{
public://メンバ関数
	//デストラクタ
	~Scenemanager();
	//更新
	void Update();

	//描画
	void Draw();

	//終了
	void Finalize();

private://メンバ変数
	//現在シーン
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;

public://アクセッサ置き場
	//次のシーンを予約
	void SetNextScene(BaseScene* nextScene) { this->nextScene_ = nextScene; }

};
