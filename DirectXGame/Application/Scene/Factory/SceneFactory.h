#pragma once

#include "AbstractSceneFactory.h"

/*

*	SceneFactory.h

*	具体的(このゲーム用)なシーンファクトリー

*/
class SceneFactory :public AbstractSceneFactory
{
public:
	//シーン生成(シーン名、ステージ情報)
	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName, const int stagenum = 0) const override;

};
