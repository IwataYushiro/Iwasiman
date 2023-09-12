#include "EnemyFactory.h"
#include "MyMath.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Enemy3.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

/*
Create(model, bullet, player, gamescene, level)のlevelについて;
	
	level = 1 -> 通常時
	level = 2 -> 攻撃力強め　速度遅め		E2P
	level = 3 -> ライフ多め　攻撃力弱め		E2G
	level = 4 -> 速度速め　　ライフ少なめ	E2S
	level = 5 -> 攻撃力ほぼ即死　他弱め		E2D
*/

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Model* bullet, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//前に進む雑魚
		return Enemy1::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E1P") == 0)
	{
		//前に進む雑魚(攻撃力強め　速度遅め)
		return Enemy1::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E1G") == 0)
	{
		//前に進む雑魚(ライフ多め　攻撃力弱め)
		return Enemy1::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E1S") == 0)
	{
		//前に進む雑魚(速度速め　ライフ少なめ)
		return Enemy1::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E1D") == 0)
	{
		//前に進む雑魚(攻撃力ほぼ即死　他弱め)
		return Enemy1::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("ENEMY2") == 0)
	{
		//横にも動く雑魚
		return Enemy2::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E2P") == 0)
	{
		//横にも動く雑魚(攻撃力強め　速度遅め)
		return Enemy2::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E2G") == 0)
	{
		//横にも動く雑魚(ライフ多め　攻撃力弱め)
		return Enemy2::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E2S") == 0)
	{
		//横にも動く雑魚(速度速め　ライフ少なめ)
		return Enemy2::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E2D") == 0)
	{
		//横にも動く雑魚(攻撃力ほぼ即死　他弱め)
		return Enemy2::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("ENEMY3") == 0)
	{
		//ワープする雑魚
		return Enemy3::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E3P") == 0)
	{
		//ワープする雑魚(攻撃力強め　速度遅め)
		return Enemy3::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E3G") == 0)
	{
		//ワープする雑魚(ライフ多め　攻撃力弱め)
		return Enemy3::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E3S") == 0)
	{
		//ワープする雑魚(速度速め　ライフ少なめ)
		return Enemy3::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E3D") == 0)
	{
		//ワープする雑魚(攻撃力ほぼ即死　他弱め)
		return Enemy3::Create(model, bullet, player, gamescene, 5);
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
