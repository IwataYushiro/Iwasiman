#pragma once
#include "AbstractGimmickFactory.h"

/*

*	GimmickFactory.h

*	��̓I(���̃Q�[���p)�ȃM�~�b�N�t�@�N�g���[

*/
class GimmickFactory :public AbstractGimmickFactory
{
public:
	//�M�~�b�N����
	std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName
		, Model* model, Player* player) override;

};
