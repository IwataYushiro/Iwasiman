#pragma once

#include "BaseGimmick.h"
#include <string>
#include <list>
#include <memory>

//抽象ギミックファクトリー

class AbstractGimmickFactory
{
public:
	//仮想デストラクタ
	virtual ~AbstractGimmickFactory() = default;

	//ギミック生成
	virtual std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName,
		Model* model, Player* player) = 0;
private:

};
