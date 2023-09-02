#pragma once
#include "AbstractGimmickFactory.h"

//具体的(このゲーム用)な敵ファクトリー
class GimmickFactory :public AbstractGimmickFactory
{
public:
	//敵生成
	std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName
		, Model* model, Player* player) override;

};
