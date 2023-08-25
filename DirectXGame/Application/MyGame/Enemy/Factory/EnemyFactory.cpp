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
		//クリボー風雑魚
		return Enemy1::Create(model, player, gamescene);
	}
	else if (enemyName.find("ENEMY2") == 0)
	{
		//ドッスン風雑魚
		return Enemy2::Create(model, player, gamescene);
	}
	else if (enemyName.find("BOSS1") == 0)
	{
		//ベジェ曲線で動くボス
		return EnemyBoss::Create(model, player, gamescene, SUBCOLLISION_ATTR_NONE);
	}
	else if (enemyName.find("CORE1") == 0)
	{
		//コア
		return EnemyBoss::Create(model, player, gamescene, SUBCOLLISION_ATTR_ENEMYCORE);
	}
	return nullptr;
}
