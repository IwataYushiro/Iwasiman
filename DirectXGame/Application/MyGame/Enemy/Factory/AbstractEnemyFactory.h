#pragma once

#include "BaseEnemy.h"
#include <string>
#include <list>
#include <memory>

//抽象シーンファクトリー

class AbstractEnemyFactory
{
public:
	//仮想デストラクタ
	virtual ~AbstractEnemyFactory() = default;

	//敵生成
	virtual std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName,
		Model* model, Model* bullet, Player* player, GamePlayScene* gamescene) = 0;
private:

};
