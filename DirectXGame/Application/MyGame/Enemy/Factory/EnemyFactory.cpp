#include "EnemyFactory.h"
#include "MyMath.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

/*
Create(model, bullet, player, gamescene, level)のlevelについて;
	
	level = 1 -> 通常時
	level = 2 -> 攻撃力強め　速度遅め
	level = 3 -> ライフ多め　攻撃力弱め
	level = 4 -> 速度速め　　ライフ少なめ
	level = 5 -> 攻撃力ほぼ即死　他弱め
*/

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Model* bullet, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//前に進む雑魚
		return Enemy1::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("ENEMY2") == 0)
	{
		//横にも動く雑魚
		return Enemy2::Create(model, bullet, player, gamescene, 1);
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
