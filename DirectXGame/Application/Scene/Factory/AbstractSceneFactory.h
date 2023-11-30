#pragma once
#include "BaseScene.h"
#include <string>
#include <memory>

/*

*	AbstractSceneFactory.h (cpp X)

*	���ۃV�[���t�@�N�g���[

*/
class AbstractSceneFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractSceneFactory() = default;

	//�V�[������(�V�[�����A�X�e�[�W���)
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName, const int stagenum = 0) const = 0;

};
