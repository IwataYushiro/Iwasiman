#pragma once
#include "BaseEnemy.h"
#include "AbstractEnemyFactory.h"
#include <list>
#include <memory>

class EnemyManager final
{
public://シングルトンインスタンス
	static EnemyManager* GetInstance();

public://メンバ関数
	//次の敵を予約
	void CreateEnemy(const std::string& enemyName,
		std::unique_ptr<BaseEnemy>const& enemy,
		Model* model, Player* player, GamePlayScene* gamescene);
	//更新
	void Update();

	//描画
	void Draw();

	//終了
	void Finalize();

private://メンバ変数
	//現在敵
	std::unique_ptr<BaseEnemy> enemy_ = nullptr;
	//次の敵
	std::unique_ptr<BaseEnemy> nextEnemy_ = nullptr;
	//借りてくる敵ファクトリー
	AbstractEnemyFactory* enemyFactory_ = nullptr;

public://アクセッサ置き場
	//敵ファクトリーのセッター
	void SetEnemyFactory(AbstractEnemyFactory* enemyFactory) { this->enemyFactory_ = enemyFactory; }

private:
	EnemyManager() = default;
	~EnemyManager() = default;
public:
	EnemyManager(const EnemyManager& obj) = delete;
	EnemyManager& operator=(const EnemyManager& obj) = delete;
};
