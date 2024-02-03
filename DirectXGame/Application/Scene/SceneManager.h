#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "EnumList.h"

#include <memory>
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
	void ChangeScene(const std::string& sceneName, const int stagenum = SL_Default);
	//更新
	void Update();

	//描画
	void Draw();
	//ポストエフェクト描画
	void DrawPostEffect();
	//終了
	void Finalize();

private://メンバ変数
	//現在シーン
	std::unique_ptr<BaseScene> scene_ = nullptr;
	//次のシーン
	std::unique_ptr<BaseScene> nextScene_ = nullptr;
	//借りてくるシーンファクトリー
	const AbstractSceneFactory* sceneFactory_ = nullptr;

public://アクセッサ置き場
	//シーンファクトリーのセッター
	void SetSceneFactory(const AbstractSceneFactory* sceneFactory) { this->sceneFactory_ = sceneFactory; }

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
