#pragma once
#include "AbstractGimmickFactory.h"

/*

*	GimmickFactory.h

*	��̓I(���̃Q�[���p)�ȃM�~�b�N�t�@�N�g���[

*/
class GimmickFactory :public AbstractGimmickFactory
{
public:
	//�M�~�b�N����(�M�~�b�N���A�g�p���f���A�v���C���[)
	std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName
		, const Model* model, const Player* player) override;

	//����������(�M�~�b�N���A�g�p���f���A�v���C���[)
	std::unique_ptr<BaseGimmick> CreateMoveGround(const std::string& gimmickName,
		Model* model, const Player* player)override;
};
