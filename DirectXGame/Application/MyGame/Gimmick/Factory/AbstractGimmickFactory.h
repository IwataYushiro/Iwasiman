#pragma once

#include "BaseGimmick.h"
#include <string>
#include <list>
#include <memory>

/*

*	AbstractGimmickFactory.h (cpp X)

*	抽象ギミックファクトリー

*/
class AbstractGimmickFactory
{
public:
	//仮想デストラクタ
	virtual ~AbstractGimmickFactory() = default;

	//ギミック生成(ギミック名、使用モデル、プレイヤー)
	virtual std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName,
		Model* model, Player* player) = 0;
private:

};
