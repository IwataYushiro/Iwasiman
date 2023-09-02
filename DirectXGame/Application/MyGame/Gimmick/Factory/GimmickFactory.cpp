#include "GimmickFactory.h"

#include "Spike.h"
#include "FallSphere.h"

#include "CollisionAttribute.h"

std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, Model* model, Player* player)
{
	//トゲ(ギミックの1つだが一旦ここでテスト)
	if (gimmickName.find("SPIKE") == 0)
	{
		return Spike::Create(model, player);
	}
	else if(gimmickName.find("FALLSPHERE") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE);
	}
	else if (gimmickName.find("FALLSPHERERETURN") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN);
	}

	return nullptr;
}
