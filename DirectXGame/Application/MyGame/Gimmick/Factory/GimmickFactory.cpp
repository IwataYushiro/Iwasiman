#include "GimmickFactory.h"

#include "Spike.h"
#include "FallAndRiseSphere.h"

#include "CollisionAttribute.h"

using namespace IwasiEngine;
/*

*	GimmickFactory.cpp

*	具体的(このゲーム用)なギミックファクトリー

*/

std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, const Model* model, const Player* player)
{
	//ローカル変数
	const bool findSuccess = 0;//合ってるとき
	//トゲ
	if (gimmickName.find("SPIKE") == findSuccess)
	{
		return Spike::Create(model, player);
	}

	return nullptr;
}

std::unique_ptr<BaseGimmick> GimmickFactory::CreateMoveGround(const std::string& gimmickName, Model* model, const Player* player)
{
	//ローカル変数
	const bool findSuccess = 0;//合ってるとき
	//落ちる球
	if (gimmickName.find("FALLSPHERE") == findSuccess)
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE);
	}
	//落ちて戻ってくる球
	else if (gimmickName.find("FALLSPHERERETURN") == findSuccess)
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN);
	}
	//昇る球
	else if (gimmickName.find("RISESPHERE") == findSuccess)
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE);
	}

	return nullptr;
}
