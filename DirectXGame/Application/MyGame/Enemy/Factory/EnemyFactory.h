#pragma once
#include "AbstractEnemyFactory.h"

/*

*	EnemyFactory.h

*	��̓I(���̃Q�[���p)�ȓG�t�@�N�g���[

*/
class EnemyFactory :public AbstractEnemyFactory
{
public:
	//�G����(�G�̖��O�A�g�p���f���A�g�p���f��(�e)�A�v���C���[�A�Q�[���v���C�V�[��)
	std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName
		,Model* model, Model* bullet, Player* player, GamePlayScene* gamescene) override;

};
