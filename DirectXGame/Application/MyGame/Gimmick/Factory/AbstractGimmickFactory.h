#pragma once

#include "BaseGimmick.h"
#include <string>
#include <list>
#include <memory>

//���ۃM�~�b�N�t�@�N�g���[

class AbstractGimmickFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractGimmickFactory() = default;

	//�M�~�b�N����
	virtual std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName,
		Model* model, Player* player) = 0;
private:

};
