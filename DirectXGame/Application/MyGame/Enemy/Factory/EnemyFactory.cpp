#include "EnemyFactory.h"
#include "Enemy1.h"
#include "EnemyBoss.h"

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,Model* model, Player* player, GamePlayScene* gamescene)
{
	//次のシーンを生成
	std::unique_ptr<BaseEnemy> newenemy;

	if (enemyName == "ENEMY1")
	{
		//クリボー風雑魚
		newenemy = Enemy1::Create(model, player, gamescene);
	}
	
	else if (enemyName == "BOSS1")
	{
		//ベジェ曲線で動くボス
		newenemy = EnemyBoss::Create(model, player, gamescene);
	}
	
	return newenemy;
}
