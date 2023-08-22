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

	//���̃V�[���𐶐�
	nextEnemy_ = enemyFactory_->CreateEnemy(enemyName,model, player, gamescene);
}

void EnemyManager::Update()
{
	//�V�[���؂�ւ��@�\
	//���̃V�[���̗\�񂪓�������
	if (nextEnemy_)
	{
		
		//�V�[���؂�ւ�
		nextEnemy_ = nullptr;

		//SceneManager�̃Z�b�g
		//enemy_->SetEnemyManager(this);

		//�؂�ւ����V�[����������
		enemy_->Initialize();
	}

	//���s���̃V�[�����X�V
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
