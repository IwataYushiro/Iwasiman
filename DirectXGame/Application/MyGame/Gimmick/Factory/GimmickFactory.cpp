#include "GimmickFactory.h"

#include "Spike.h"
#include "FallSphere.h"

#include "CollisionAttribute.h"

//具体的(このゲーム用)なギミックファクトリー
std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, Model* model, Player* player)
{
	//トゲ
	if (gimmickName.find("SPIKE") == 0)
	{
		return Spike::Create(model, player);
	}
	//落ちる球
	else if(gimmickName.find("FALLSPHERE") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE);
	}
	//落ちて戻ってくる球
	else if (gimmickName.find("FALLSPHERERETURN") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN);
	}
	//昇る球
	else if (gimmickName.find("RISESPHERE") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE);
	}

	return nullptr;
}
