#include "SceneManager.h"

Scenemanager::~Scenemanager()
{
	Finalize();
}

void Scenemanager::Update()
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
		
		//切り替えたシーンを初期化
		scene_->Initialize();
	}
	 
	//実行中のシーンを更新
	scene_->Update();
}

void Scenemanager::Draw()
{
	scene_->Draw();
}

void Scenemanager::Finalize()
{
	scene_->Finalize();
	delete scene_;
}
