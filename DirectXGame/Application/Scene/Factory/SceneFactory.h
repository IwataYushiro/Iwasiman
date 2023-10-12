#pragma once

#include "AbstractSceneFactory.h"

/*

*	SceneFactory.h

*	��̓I(���̃Q�[���p)�ȃV�[���t�@�N�g���[

*/
class SceneFactory :public AbstractSceneFactory
{
public:
	//�V�[������(�V�[�����A�X�e�[�W���)
	BaseScene* CreateScene(const std::string& sceneName, int stagenum = 0) override;

};
