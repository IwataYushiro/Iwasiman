#pragma once

#include "AbstractEnemyFactory.h"

//��̓I(���̃Q�[���p)�ȃV�[���t�@�N�g���[
class EnemyFactory :public AbstractEnemyFactory
{
public:
	//�V�[������
	std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName
		,Model* model, Player* player, GamePlayScene* gamescene) override;

};
