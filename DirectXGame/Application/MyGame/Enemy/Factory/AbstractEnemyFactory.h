#pragma once

#include "BaseEnemy.h"

#include <string>
#include <list>
#include <memory>

/*

*	AbstractEnemyFactory.h (cpp X)

*	抽象エネミファクトリー

*/
class AbstractEnemyFactory
{
public:
	//仮想デストラクタ
	virtual ~AbstractEnemyFactory() = default;

	//敵生成(敵の名前、使用モデル、使用モデル(弾)、プレイヤー、ゲームプレイシーン)
	virtual std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName,
		const Model* model, const Model* bullet, const Player* player,GamePlayScene* gamescene) = 0;
private:

};
