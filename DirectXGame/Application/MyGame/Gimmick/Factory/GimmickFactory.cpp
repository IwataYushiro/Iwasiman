#include "GimmickFactory.h"

#include "Spike.h"
#include "FallAndRiseSphere.h"

#include "CollisionAttribute.h"

using namespace IwasiEngine;
/*

*	GimmickFactory.cpp

*	��̓I(���̃Q�[���p)�ȃM�~�b�N�t�@�N�g���[

*/

std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, const Model* model, const Player* player)
{
	//���[�J���ϐ�
	const bool findSuccess = 0;//�����Ă�Ƃ�
	//�g�Q
	if (gimmickName.find("SPIKE") == findSuccess)						//�g�Q
	{
		return Spike::Create(model, player);
	}

	return nullptr;
}

std::unique_ptr<BaseGimmick> GimmickFactory::CreateMoveGround(const std::string& gimmickName, Model* model, const Player* player)
{
	//���[�J���ϐ�
	const bool findSuccess = 0;//�����Ă�Ƃ�
	//�����鋅
	if (gimmickName.find("FALLSPHERE") == findSuccess)					//�����鏰
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE);
	}
	//�����Ė߂��Ă��鋅
	else if (gimmickName.find("FSPHERERETURN") == findSuccess)		//�����Ė߂鏰
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN);
	}
	//���鋅
	else if (gimmickName.find("RISESPHERE") == findSuccess)				//���鏰
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE);
	}
	//�����ċ�
	else if (gimmickName.find("RSPHERERETURN") == findSuccess)				//���鏰
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE_RETURN);
	}

	return nullptr;
}
