#pragma once

#include "AbstractSceneFactory.h"

/*

*	SceneFactory.h

*	具体的(このゲーム用)なシーンファクトリー

*/
class SceneFactory :public AbstractSceneFactory
{
public:
	//シーン生成
	BaseScene* CreateScene(const std::string& sceneName, int stagenum = 0) override;

};
