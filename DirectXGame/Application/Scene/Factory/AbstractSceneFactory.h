#pragma once
#include "BaseScene.h"
#include <string>
#include <memory>

/*

*	AbstractSceneFactory.h (cpp X)

*	抽象シーンファクトリー

*/
class AbstractSceneFactory
{
public:
	//仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;

	//シーン生成(シーン名、ステージ情報)
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName, const int stagenum = 0) const = 0;

};
