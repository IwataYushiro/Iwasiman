#pragma once
#include "AbstractGimmickFactory.h"

/*

*	GimmickFactory.h

*	具体的(このゲーム用)なギミックファクトリー

*/
class GimmickFactory :public AbstractGimmickFactory
{
public:
	//ギミック生成(ギミック名、使用モデル、プレイヤー)
	std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName
		, const Model* model, const Player* player) override;

	//動く床生成(ギミック名、使用モデル、プレイヤー)
	std::unique_ptr<BaseGimmick> CreateMoveGround(const std::string& gimmickName,
		Model* model, const Player* player)override;
};
