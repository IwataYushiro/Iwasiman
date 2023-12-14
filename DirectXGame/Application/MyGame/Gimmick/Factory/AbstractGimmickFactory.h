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
protected:
	using Model = IwasiEngine::Model;

public:
	//仮想デストラクタ
	virtual ~AbstractGimmickFactory() = default;

	//ギミック生成(ギミック名、使用モデル、プレイヤー)
	virtual std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName,
		const Model* model, const Player* player) = 0;

	//動く床生成(ギミック名、使用モデル、プレイヤー)
	virtual std::unique_ptr<BaseGimmick> CreateMoveGround(const std::string& gimmickName,
		Model* model, const Player* player) = 0;
private:

};
