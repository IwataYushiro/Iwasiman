#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "EnumList.h"

/*

*	SceneManager.h

*	シーンマネージャー

*/
class SceneManager final
{
public://シングルトンインスタンス
	static SceneManager* GetInstance();

public://メンバ関数
	//次のシーンを予約(シーン名、ステージ番号)
	void ChangeScene(const std::string& sceneName, int stagenum = SL_Default);
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

private:
	//コンストラクタ（シングルトンパターン）
	SceneManager() = default;
	//デストラクタ（シングルトンパターン）
	~SceneManager() = default;
public:
	//コピーコンストラクタの防止（シングルトンパターン）
	SceneManager(const SceneManager& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	SceneManager& operator=(const SceneManager& obj) = delete;
};
