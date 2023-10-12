#pragma once
#include "AbstractEnemyFactory.h"

/*

*	EnemyFactory.h

*	��̓I(���̃Q�[���p)�ȓG�t�@�N�g���[

*/
class EnemyFactory :public AbstractEnemyFactory
{
public:
	//�G����
	std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName
		,Model* model, Model* bullet, Player* player, GamePlayScene* gamescene) override;

};
