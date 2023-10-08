#include "EnemyFactory.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

//�G�̍H��
std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Model* bullet, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//�N���{�[���G��
		return Enemy1::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E1P") == 0)
	{
		//�N���{�[���G��(�U���͋��߁@���x�x��)
		return Enemy1::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E1G") == 0)
	{
		//�N���{�[���G��(���C�t���߁@�U���͎��)
		return Enemy1::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E1S") == 0)
	{
		//�N���{�[���G��(���x���߁@���C�t���Ȃ�)
		return Enemy1::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E1D") == 0)
	{
		//�N���{�[���G��(�U���͂قڑ����@�����)
		return Enemy1::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("ENEMY2") == 0)
	{
		//�h�b�X�����G��
		return Enemy2::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E2P") == 0)
	{
		//�h�b�X�����G��(�U���͋��߁@���x�x��)
		return Enemy2::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E2G") == 0)
	{
		//�h�b�X�����G��(���C�t���߁@�U���͎��)
		return Enemy2::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E2S") == 0)
	{
		//�h�b�X�����G��(���x���߁@���C�t���Ȃ�)
		return Enemy2::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E2D") == 0)
	{
		//�h�b�X�����G��(�U���͂قڑ����@�����)
		return Enemy2::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("BOSS1") == 0)
	{
		//�x�W�F�Ȑ��œ����{�X
		return EnemyBoss::Create(model,bullet, player, gamescene);
	}
	else if (enemyName.find("CORE1") == 0)
	{
		//�R�A
		return EnemyCore::Create(model,bullet, player, gamescene);
	}

	return nullptr;
}
