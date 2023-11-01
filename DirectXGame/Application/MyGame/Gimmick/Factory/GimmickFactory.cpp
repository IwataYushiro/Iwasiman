#include "GimmickFactory.h"

#include "Spike.h"
#include "FallAndRiseSphere.h"

#include "CollisionAttribute.h"


/*

*	GimmickFactory.cpp

*	��̓I(���̃Q�[���p)�ȃM�~�b�N�t�@�N�g���[

*/

std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, Model* model, Player* player)
{
	//���[�J���ϐ�
	const bool findSuccess = 0;//�����Ă�Ƃ�
	//�g�Q
	if (gimmickName.find("SPIKE") == findSuccess)
	{
		return Spike::Create(model, player);
	}
	//�����鋅
	else if (gimmickName.find("FALLSPHERE") == findSuccess)
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE);
	}
	//�����Ė߂��Ă��鋅
	else if (gimmickName.find("FALLSPHERERETURN") == findSuccess)
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN);
	}
	//���鋅
	else if (gimmickName.find("RISESPHERE") == findSuccess)
	{
		return FallAndRiseSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE);
	}

	return nullptr;
}
