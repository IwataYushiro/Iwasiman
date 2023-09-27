#include "SceneFactory.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "StageClearScene.h"
#include "StageSelectScene.h"
#include "GameOverScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName, int stagenum)
{
	//次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE")
	{
		newScene = new TitleScene();
	}
	else if (sceneName == "GAMEPLAY")
	{
		newScene = new GamePlayScene(stagenum);
	}
	else if (sceneName == "STAGECLEAR")
	{
		newScene = new StageClearScene(stagenum);
	}
	else if (sceneName == "STAGESELECT")
	{
		newScene = new StageSelectScene();
	}
	else if (sceneName == "GAMEOVER")
	{
		newScene = new GameOverScene(stagenum);
	}
	return newScene;
}
