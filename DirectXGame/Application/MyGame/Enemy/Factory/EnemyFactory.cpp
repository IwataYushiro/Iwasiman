#include "EnemyFactory.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

using namespace IwasiEngine;
/*

*	EnemyFactory.cpp

*	具体的(このゲーム用)な敵ファクトリー

*/

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	const Model* model, const Model* bullet, const Player* player, GamePlayScene* gamescene)
{
	//ローカル変数
	const int8_t findSuccess = 0;//合ってるとき
	//敵属性
	enum EnemyType
	{
		ET_Normal = 1,	//通常
		ET_Power = 2,	//攻撃型
		ET_Guard = 3,	//防御型
		ET_Speed = 4,	//速度型
		ET_Death = 5,	//危険型
	};

	if (enemyName.find("ENEMY1") == findSuccess)
	{
		//クリボー風雑魚
		return Enemy1::Create(model, bullet, player, gamescene, ET_Normal);
	}
	else if (enemyName.find("E1P") == findSuccess)
	{
		//クリボー風雑魚(攻撃力強め　速度遅め)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Power);
	}
	else if (enemyName.find("E1G") == findSuccess)
	{
		//クリボー風雑魚(ライフ多め　攻撃力弱め)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Guard);
	}
	else if (enemyName.find("E1S") == findSuccess)
	{
		//クリボー風雑魚(速度速め　ライフ少なめ)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Speed);
	}
	else if (enemyName.find("E1D") == findSuccess)
	{
		//クリボー風雑魚(攻撃力ほぼ即死　他弱め)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Death);
	}

	else if (enemyName.find("ENEMY2") == findSuccess)
	{
		//ドッスン風雑魚
		return Enemy2::Create(model, bullet, player, gamescene, ET_Normal);
	}
	else if (enemyName.find("E2P") == findSuccess)
	{
		//ドッスン風雑魚(攻撃力強め　速度遅め)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Power);
	}
	else if (enemyName.find("E2G") == findSuccess)
	{
		//ドッスン風雑魚(ライフ多め　攻撃力弱め)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Guard);
	}
	else if (enemyName.find("E2S") == findSuccess)
	{
		//ドッスン風雑魚(速度速め　ライフ少なめ)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Speed);
	}
	else if (enemyName.find("E2D") == findSuccess)
	{
		//ドッスン風雑魚(攻撃力ほぼ即死　他弱め)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Death);
	}

	else if (enemyName.find("BOSS1") == findSuccess)
	{
		//ベジェ曲線で動くボス
		return EnemyBoss::Create(model, bullet, player, gamescene);
	}
	else if (enemyName.find("CORE1") == findSuccess)
	{
		//コア
		return EnemyCore::Create(model,bullet, player, gamescene);
	}

	return nullptr;
}
