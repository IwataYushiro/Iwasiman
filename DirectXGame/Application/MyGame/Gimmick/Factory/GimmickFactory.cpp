#include "GimmickFactory.h"

#include "Spike.h"

std::unique_ptr<BaseGimmick> GimmickFactory::CreateGimmick(const std::string& gimmickName, Model* model, Player* player)
{
	//�g�Q(�M�~�b�N��1������U�����Ńe�X�g)
	if (gimmickName.find("SPIKE") == 0)
	{
		return Spike::Create(model, player);
	}

	return nullptr;
}
