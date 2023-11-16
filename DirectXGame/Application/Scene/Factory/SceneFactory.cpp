#include "SceneFactory.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "StageClearScene.h"
#include "StageSelectScene.h"
#include "GameOverScene.h"

/*

*	SceneFactory.cpp

*	��̓I(���̃Q�[���p)�ȃV�[���t�@�N�g���[

*/

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName, int stagenum)
{
	//���̃V�[���𐶐�
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "TITLE")
	{
		newScene = std::make_unique<TitleScene>(stagenum);
	}
	else if (sceneName == "GAMEPLAY")
	{
		newScene = std::make_unique<GamePlayScene>(stagenum);
	}
	else if (sceneName == "STAGECLEAR")
	{
		newScene = std::make_unique<StageClearScene>(stagenum);
	}
	else if (sceneName == "STAGESELECT")
	{
		newScene = std::make_unique<StageSelectScene>(stagenum);
	}
	else if (sceneName == "GAMEOVER")
	{
		newScene = std::make_unique<GameOverScene>(stagenum);
	}
	return newScene;
}
