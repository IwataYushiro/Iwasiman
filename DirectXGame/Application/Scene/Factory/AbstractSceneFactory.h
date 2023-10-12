#pragma once

#include "BaseScene.h"
#include <string>


/*

*	AbstractSceneFactory.h (cpp X)

*	抽象シーンファクトリー

*/
class AbstractSceneFactory
{
public:
	//仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;

	//シーン生成
	virtual BaseScene* CreateScene(const std::string& sceneName, int stagenum = 0) = 0;
private:

};
