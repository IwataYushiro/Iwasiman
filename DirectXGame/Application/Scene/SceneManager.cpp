#include "SceneManager.h"

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
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
			delete scene_;
		}

		//シーン切り替え
		scene_ = nextScene_;
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
	scene_->Draw();
}

void SceneManager::Finalize()
{
	scene_->Finalize();
	delete scene_;
}
