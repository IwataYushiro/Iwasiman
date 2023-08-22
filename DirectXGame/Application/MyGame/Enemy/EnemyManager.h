#pragma once
#include "BaseEnemy.h"
#include "AbstractEnemyFactory.h"
#include <list>
#include <memory>

class EnemyManager final
{
public://�V���O���g���C���X�^���X
	static EnemyManager* GetInstance();

public://�����o�֐�
	//���̓G��\��
	void CreateEnemy(const std::string& enemyName,
		std::unique_ptr<BaseEnemy>const& enemy,
		Model* model, Player* player, GamePlayScene* gamescene);
	//�X�V
	void Update();

	//�`��
	void Draw();

	//�I��
	void Finalize();

private://�����o�ϐ�
	//���ݓG
	std::unique_ptr<BaseEnemy> enemy_ = nullptr;
	//���̓G
	std::unique_ptr<BaseEnemy> nextEnemy_ = nullptr;
	//�؂�Ă���G�t�@�N�g���[
	AbstractEnemyFactory* enemyFactory_ = nullptr;

public://�A�N�Z�b�T�u����
	//�G�t�@�N�g���[�̃Z�b�^�[
	void SetEnemyFactory(AbstractEnemyFactory* enemyFactory) { this->enemyFactory_ = enemyFactory; }

private:
	EnemyManager() = default;
	~EnemyManager() = default;
public:
	EnemyManager(const EnemyManager& obj) = delete;
	EnemyManager& operator=(const EnemyManager& obj) = delete;
};
