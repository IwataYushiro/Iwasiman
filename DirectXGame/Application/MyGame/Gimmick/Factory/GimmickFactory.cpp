#include "GimmickFactory.h"

#include "Spike.h"
#include "FallSphere.h"

#include "CollisionAttribute.h"

//��̓I(���̃Q�[���p)�ȃM�~�b�N�t�@�N�g���[
std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, Model* model, Player* player)
{
	//�g�Q
	if (gimmickName.find("SPIKE") == 0)
	{
		return Spike::Create(model, player);
	}
	//�����鋅
	else if(gimmickName.find("FALLSPHERE") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE);
	}
	//�����Ė߂��Ă��鋅
	else if (gimmickName.find("FALLSPHERERETURN") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN);
	}
	//���鋅
	else if (gimmickName.find("RISESPHERE") == 0)
	{
		return FallSphere::Create(model, player, SUBCOLLISION_ATTR_GIMMICK_UPSPHERE);
	}

	return nullptr;
}
