#include "EnemyFactory.h"
#include "MyMath.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

/*
Create(model, bullet, player, gamescene, level)��level�ɂ���;
	
	level = 1 -> �ʏ펞
	level = 2 -> �U���͋��߁@���x�x��
	level = 3 -> ���C�t���߁@�U���͎��
	level = 4 -> ���x���߁@�@���C�t���Ȃ�
	level = 5 -> �U���͂قڑ����@�����
*/

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	Model* model, Model* bullet, Player* player, GamePlayScene* gamescene)
{
	if (enemyName.find("ENEMY1") == 0)
	{
		//�O�ɐi�ގG��
		return Enemy1::Create(model, bullet, player, gamescene, 1);
	}
	else if (enemyName.find("ENEMY2") == 0)
	{
		//���ɂ������G��
		return Enemy2::Create(model, bullet, player, gamescene, 1);
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
