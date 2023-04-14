#pragma once

#include "BaseScene.h"
#include <string>

//���ۃV�[���t�@�N�g���[

class AbstractSceneFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractSceneFactory() = default;

	//�V�[������
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
private:

};
