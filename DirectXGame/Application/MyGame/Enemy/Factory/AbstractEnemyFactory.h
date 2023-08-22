#pragma once

#include "BaseEnemy.h"
#include <string>
#include <list>
#include <memory>

//���ۃV�[���t�@�N�g���[

class AbstractEnemyFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractEnemyFactory() = default;

	//�V�[������
	virtual std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName,
		Model* model, Player* player, GamePlayScene* gamescene) = 0;
private:

};
