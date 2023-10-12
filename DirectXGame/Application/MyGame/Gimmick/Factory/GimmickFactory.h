#pragma once
#include "AbstractGimmickFactory.h"

/*

*	GimmickFactory.h

*	具体的(このゲーム用)なギミックファクトリー

*/
class GimmickFactory :public AbstractGimmickFactory
{
public:
	//ギミック生成
	std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName
		, Model* model, Player* player) override;

};
