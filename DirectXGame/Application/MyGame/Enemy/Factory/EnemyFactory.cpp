#include "EnemyFactory.h"
#include "Enemy1.h"
#include "EnemyBoss.h"

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,Model* model, Player* player, GamePlayScene* gamescene)
{
	//���̃V�[���𐶐�
	std::unique_ptr<BaseEnemy> newenemy;

	if (enemyName == "ENEMY1")
	{
		//�N���{�[���G��
		newenemy = Enemy1::Create(model, player, gamescene);
	}
	
	else if (enemyName == "BOSS1")
	{
		//�x�W�F�Ȑ��œ����{�X
		newenemy = EnemyBoss::Create(model, player, gamescene);
	}
	
	return newenemy;
}
