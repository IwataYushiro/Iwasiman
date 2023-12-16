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

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName, const int stagenum)const
{
	//���̃V�[���𐶐�
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "TITLE")				//�^�C�g���V�[��
	{
		newScene = std::make_unique<TitleScene>(stagenum);				
	}
	else if (sceneName == "GAMEPLAY")		//�Q�[���v���C�V�[��
	{
		newScene = std::make_unique<GamePlayScene>(stagenum);			
	}
	else if (sceneName == "STAGECLEAR")		//�X�e�[�W�N���A�V�[��
	{
		newScene = std::make_unique<StageClearScene>(stagenum);			
	}
	else if (sceneName == "STAGESELECT")	//�X�e�[�W�Z���N�g�V�[��
	{
		newScene = std::make_unique<StageSelectScene>(stagenum);		
	}
	else if (sceneName == "GAMEOVER")		//�Q�[���I�[�o�[�V�[��
	{
		newScene = std::make_unique<GameOverScene>(stagenum);			
	}
	return newScene;
}
