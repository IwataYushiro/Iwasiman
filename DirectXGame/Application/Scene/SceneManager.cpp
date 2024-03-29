#include "SceneManager.h"
#include <cassert>

/*

*	SceneManager.cpp

*	シーンマネージャー

*/

SceneManager* SceneManager::GetInstance()
{
	//インスタンス取得
	static SceneManager instance;
	return &instance;
}

void SceneManager::ChangeScene(const std::string& sceneName, const int stagenum)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//次のシーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName, stagenum);
}

void SceneManager::Update()
{
	//シーン切り替え機構
	//次のシーンの予約が入ったら
	if (nextScene_)
	{
		//旧シーン終了
		if (scene_)
		{
			scene_->Finalize();
		}

		//シーン切り替え
		scene_ = std::move(nextScene_);
		nextScene_ = nullptr;

		//SceneManagerのセット
		scene_->SetSceneManager(this);
		
		//切り替えたシーンを初期化
		scene_->Initialize();
	}
	 
	//実行中のシーンを更新
	scene_->Update();
}

void SceneManager::Draw()
{
	scene_->Draw();//シーン描画
}

void SceneManager::DrawPostEffect()
{
	scene_->DrawPostEffect();
}

void SceneManager::Finalize()
{
	scene_->Finalize();//終了処理
}
