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
protected:
	using Model = IwasiEngine::Model;

public:
	//���z�f�X�g���N�^
	virtual ~AbstractGimmickFactory() = default;

	//�M�~�b�N����(�M�~�b�N���A�g�p���f���A�v���C���[)
	virtual std::unique_ptr<BaseGimmick> CreateGimmick(const std::string& gimmickName,
		const Model* model, const Player* player) = 0;

	//����������(�M�~�b�N���A�g�p���f���A�v���C���[)
	virtual std::unique_ptr<BaseGimmick> CreateMoveGround(const std::string& gimmickName,
		Model* model, const Player* player) = 0;
private:

};
