#pragma once
#include "AbstractEnemyFactory.h"

//具体的(このゲーム用)な敵ファクトリー
class EnemyFactory :public AbstractEnemyFactory
{
public:
	//敵生成
	std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName
		,Model* model, Player* player, GamePlayScene* gamescene) override;

};
