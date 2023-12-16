#include "SceneFactory.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "StageClearScene.h"
#include "StageSelectScene.h"
#include "GameOverScene.h"

/*

*	SceneFactory.cpp

*	具体的(このゲーム用)なシーンファクトリー

*/

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName, const int stagenum)const
{
	//次のシーンを生成
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "TITLE")				//タイトルシーン
	{
		newScene = std::make_unique<TitleScene>(stagenum);				
	}
	else if (sceneName == "GAMEPLAY")		//ゲームプレイシーン
	{
		newScene = std::make_unique<GamePlayScene>(stagenum);			
	}
	else if (sceneName == "STAGECLEAR")		//ステージクリアシーン
	{
		newScene = std::make_unique<StageClearScene>(stagenum);			
	}
	else if (sceneName == "STAGESELECT")	//ステージセレクトシーン
	{
		newScene = std::make_unique<StageSelectScene>(stagenum);		
	}
	else if (sceneName == "GAMEOVER")		//ゲームオーバーシーン
	{
		newScene = std::make_unique<GameOverScene>(stagenum);			
	}
	return newScene;
}
