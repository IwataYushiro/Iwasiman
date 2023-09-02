#pragma once
#include "AbstractGimmickFactory.h"

//��̓I(���̃Q�[���p)�ȓG�t�@�N�g���[
class GimmickFactory :public AbstractGimmickFactory
{
public:
	//�G����
	std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName
		, Model* model, Player* player) override;

};
