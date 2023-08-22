#pragma once

#include "AbstractEnemyFactory.h"

//具体的(このゲーム用)なシーンファクトリー
class EnemyFactory :public AbstractEnemyFactory
{
public:
	//シーン生成
	std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName
		,Model* model, Player* player, GamePlayScene* gamescene) override;

};
