#pragma once

#include "AbstractSceneFactory.h"

//具体的(このゲーム用)なシーンファクトリー
class SceneFactory :public AbstractSceneFactory
{
public:
	//シーン生成
	BaseScene* CreateScene(const std::string& sceneName) override;

};
