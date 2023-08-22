#include "EnemyManager.h"
#include <cassert>

EnemyManager* EnemyManager::GetInstance()
{
	static EnemyManager instance;
	return &instance;
}

void EnemyManager::CreateEnemy(const std::string& enemyName,
	std::unique_ptr<BaseEnemy>const& enemy,
	Model* model, Player* player, GamePlayScene* gamescene)
{
	assert(enemyFactory_);
	assert(nextEnemy_ == nullptr);

	//次のシーンを生成
	nextEnemy_ = enemyFactory_->CreateEnemy(enemyName,model, player, gamescene);
}

void EnemyManager::Update()
{
	//シーン切り替え機構
	//次のシーンの予約が入ったら
	if (nextEnemy_)
	{
		
		//シーン切り替え
		nextEnemy_ = nullptr;

		//SceneManagerのセット
		//enemy_->SetEnemyManager(this);

		//切り替えたシーンを初期化
		enemy_->Initialize();
	}

	//実行中のシーンを更新
	enemy_->Update();
}

void EnemyManager::Draw()
{
	enemy_->Draw();
}

void EnemyManager::Finalize()
{
	//enemy_->Finalize();

}
