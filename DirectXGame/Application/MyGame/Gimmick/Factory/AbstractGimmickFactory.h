#pragma once

#include "BaseGimmick.h"
#include <string>
#include <list>
#include <memory>

/*

*	AbstractGimmickFactory.h (cpp X)

*	���ۃM�~�b�N�t�@�N�g���[

*/
class AbstractGimmickFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractGimmickFactory() = default;

	//�M�~�b�N����(�M�~�b�N���A�g�p���f���A�v���C���[)
	virtual std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName,
		Model* model, Player* player) = 0;
private:

};
