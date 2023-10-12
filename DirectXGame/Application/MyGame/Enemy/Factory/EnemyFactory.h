#pragma once
#include "AbstractEnemyFactory.h"

/*

*	EnemyFactory.h

*	具体的(このゲーム用)な敵ファクトリー

*/
class EnemyFactory :public AbstractEnemyFactory
{
public:
	//敵生成(敵の名前、使用モデル、使用モデル(弾)、プレイヤー、ゲームプレイシーン)
	std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyName
		,Model* model, Model* bullet, Player* player, GamePlayScene* gamescene) override;

};
