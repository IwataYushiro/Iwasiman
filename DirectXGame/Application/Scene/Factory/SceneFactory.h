#pragma once

#include "AbstractSceneFactory.h"

//��̓I(���̃Q�[���p)�ȃV�[���t�@�N�g���[
class SceneFactory :public AbstractSceneFactory
{
public:
	//�V�[������
	BaseScene* CreateScene(const std::string& sceneName) override;

};
