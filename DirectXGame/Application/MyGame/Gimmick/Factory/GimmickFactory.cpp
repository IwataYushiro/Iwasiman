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
	if (gimmickName.find("SPIKE") == findSuccess)						//トゲ
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
	if (gimmickName.find("FALLSPHERE") == findSuccess)					//落ちる床
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE);
	}
	//落ちて戻ってくる球
	else if (gimmickName.find("FSPHERERETURN") == findSuccess)		//落ちて戻る床
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN);
	}
	//昇る球
	else if (gimmickName.find("RISESPHERE") == findSuccess)				//昇る床
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE);
	}
	//昇って球
	else if (gimmickName.find("RSPHERERETURN") == findSuccess)				//昇る床
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE_RETURN);
	}

	return nullptr;
}
