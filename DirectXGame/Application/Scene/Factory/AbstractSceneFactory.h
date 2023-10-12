#pragma once

#include "BaseScene.h"
#include <string>


/*

*	AbstractSceneFactory.h (cpp X)

*	���ۃV�[���t�@�N�g���[

*/
class AbstractSceneFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractSceneFactory() = default;

	//�V�[������
	virtual BaseScene* CreateScene(const std::string& sceneName, int stagenum = 0) = 0;
private:

};
