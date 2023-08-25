#include "EnemyFactory.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//�N���{�[���G��
		return Enemy1::Create(model, player, gamescene);
	}
	else if (enemyName.find("ENEMY2") == 0)
	{
		//�h�b�X�����G��
		return Enemy2::Create(model, player, gamescene);
	}
	else if (enemyName.find("BOSS1") == 0)
	{
		//�x�W�F�Ȑ��œ����{�X
		return EnemyBoss::Create(model, player, gamescene, SUBCOLLISION_ATTR_NONE);
	}
	else if (enemyName.find("CORE1") == 0)
	{
		//�R�A
		return EnemyBoss::Create(model, player, gamescene, SUBCOLLISION_ATTR_ENEMYCORE);
	}
	return nullptr;
}
