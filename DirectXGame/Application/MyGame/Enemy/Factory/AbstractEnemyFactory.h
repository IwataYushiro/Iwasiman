#pragma once

#include "BaseEnemy.h"

#include <string>
#include <list>
#include <memory>

/*

*	AbstractEnemyFactory.h (cpp X)

*	���ۃG�l�~�t�@�N�g���[

*/
class AbstractEnemyFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractEnemyFactory() = default;

	//�G����(�G�̖��O�A�g�p���f���A�g�p���f��(�e)�A�v���C���[�A�Q�[���v���C�V�[��)
	virtual std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName,
		const Model* model, const Model* bullet, const Player* player,GamePlayScene* gamescene) = 0;
private:

};
