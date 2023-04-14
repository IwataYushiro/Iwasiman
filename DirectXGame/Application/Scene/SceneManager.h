#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

class SceneManager
{
public://シングルトンインスタンス
	static SceneManager* GetInstance();

public://メンバ関数
	//次のシーンを予約
	void ChangeScene(const std::string& sceneName);
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
	//借りてくるシーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

public://アクセッサ置き場
	//シーンファクトリーのセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory_ = sceneFactory; }

};
