#include "GimmickFactory.h"

#include "Spike.h"

std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, Model* model, Player* player)
{
	//トゲ(ギミックの1つだが一旦ここでテスト)
	if (gimmickName.find("SPIKE") == 0)
	{
		return Spike::Create(model, player);
	}

	return nullptr;
}
