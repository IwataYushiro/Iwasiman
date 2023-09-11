#include "EnemyFactory.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Model* bullet, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//前に進む雑魚
		return Enemy1::Create(model, bullet, player, gamescene);
	}
	else if (enemyName.find("ENEMY2") == 0)
	{
		//横にも動く雑魚
		return Enemy2::Create(model, bullet, player, gamescene);
	}
	else if (enemyName.find("BOSS1") == 0)
	{
		//ベジェ曲線で動くボス
		return EnemyBoss::Create(model, player, gamescene);
	}
	else if (enemyName.find("CORE1") == 0)
	{
		//コア
		return EnemyCore::Create(model, player, gamescene);
	}

	return nullptr;
}
