#include "EnemyFactory.h"
#include "MyMath.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Enemy3.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

/*
Create(model, bullet, player, gamescene, level)��level�ɂ���;
	
	level = 1 -> �ʏ펞
	level = 2 -> �U���͋��߁@���x�x��		E2P
	level = 3 -> ���C�t���߁@�U���͎��		E2G
	level = 4 -> ���x���߁@�@���C�t���Ȃ�	E2S
	level = 5 -> �U���͂قڑ����@�����		E2D
*/

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Model* bullet, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//�O�ɐi�ގG��
		return Enemy1::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E1P") == 0)
	{
		//�O�ɐi�ގG��(�U���͋��߁@���x�x��)
		return Enemy1::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E1G") == 0)
	{
		//�O�ɐi�ގG��(���C�t���߁@�U���͎��)
		return Enemy1::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E1S") == 0)
	{
		//�O�ɐi�ގG��(���x���߁@���C�t���Ȃ�)
		return Enemy1::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E1D") == 0)
	{
		//�O�ɐi�ގG��(�U���͂قڑ����@�����)
		return Enemy1::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("ENEMY2") == 0)
	{
		//���ɂ������G��
		return Enemy2::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E2P") == 0)
	{
		//���ɂ������G��(�U���͋��߁@���x�x��)
		return Enemy2::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E2G") == 0)
	{
		//���ɂ������G��(���C�t���߁@�U���͎��)
		return Enemy2::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E2S") == 0)
	{
		//���ɂ������G��(���x���߁@���C�t���Ȃ�)
		return Enemy2::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E2D") == 0)
	{
		//���ɂ������G��(�U���͂قڑ����@�����)
		return Enemy2::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("ENEMY3") == 0)
	{
		//���[�v����G��
		return Enemy3::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("E3P") == 0)
	{
		//���[�v����G��(�U���͋��߁@���x�x��)
		return Enemy3::Create(model, bullet, player, gamescene, 2);
	}
	else if (enemyName.find("E3G") == 0)
	{
		//���[�v����G��(���C�t���߁@�U���͎��)
		return Enemy3::Create(model, bullet, player, gamescene, 3);
	}
	else if (enemyName.find("E3S") == 0)
	{
		//���[�v����G��(���x���߁@���C�t���Ȃ�)
		return Enemy3::Create(model, bullet, player, gamescene, 4);
	}
	else if (enemyName.find("E3D") == 0)
	{
		//���[�v����G��(�U���͂قڑ����@�����)
		return Enemy3::Create(model, bullet, player, gamescene, 5);
	}

	else if (enemyName.find("BOSS1") == 0)
	{
		//�x�W�F�Ȑ��œ����{�X
		return EnemyBoss::Create(model, player, gamescene);
	}
	else if (enemyName.find("CORE1") == 0)
	{
		//�R�A
		return EnemyCore::Create(model, player, gamescene);
	}

	return nullptr;
}
