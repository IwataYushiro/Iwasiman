#include "EnemyFactory.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

//敵の工場
std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Model* bullet, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//クリボー風雑魚
		return Enemy1::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E1P") == 0)
	{
		//クリボー風雑魚(攻撃力強め　速度遅め)
		return Enemy1::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E1G") == 0)
	{
		//クリボー風雑魚(ライフ多め　攻撃力弱め)
		return Enemy1::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E1S") == 0)
	{
		//クリボー風雑魚(速度速め　ライフ少なめ)
		return Enemy1::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E1D") == 0)
	{
		//クリボー風雑魚(攻撃力ほぼ即死　他弱め)
		return Enemy1::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("ENEMY2") == 0)
	{
		//ドッスン風雑魚
		return Enemy2::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E2P") == 0)
	{
		//ドッスン風雑魚(攻撃力強め　速度遅め)
		return Enemy2::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E2G") == 0)
	{
		//ドッスン風雑魚(ライフ多め　攻撃力弱め)
		return Enemy2::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E2S") == 0)
	{
		//ドッスン風雑魚(速度速め　ライフ少なめ)
		return Enemy2::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E2D") == 0)
	{
		//ドッスン風雑魚(攻撃力ほぼ即死　他弱め)
		return Enemy2::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("BOSS1") == 0)
	{
		//ベジェ曲線で動くボス
		return EnemyBoss::Create(model,bullet, player, gamescene);
	}
	else if (enemyName.find("CORE1") == 0)
	{
		//コア
		return EnemyCore::Create(model,bullet, player, gamescene);
	}

	return nullptr;
}
