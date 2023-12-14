#include "EnemyFactory.h"
#include "CollisionAttribute.h"
#include "Enemy1.h"
#include "Enemy2.h"

#include "EnemyBoss.h"
#include "EnemyCore.h"

using namespace IwasiEngine;
/*

*	EnemyFactory.cpp

*	��̓I(���̃Q�[���p)�ȓG�t�@�N�g���[

*/

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyName,
	const Model* model, const Model* bullet, const Player* player, GamePlayScene* gamescene)
{
	//���[�J���ϐ�
	const int8_t findSuccess = 0;//�����Ă�Ƃ�
	//�G����
	enum EnemyType
	{
		ET_Normal = 1,	//�ʏ�
		ET_Power = 2,	//�U���^
		ET_Guard = 3,	//�h��^
		ET_Speed = 4,	//���x�^
		ET_Death = 5,	//�댯�^
	};

	if (enemyName.find("ENEMY1") == findSuccess)
	{
		//�N���{�[���G��
		return Enemy1::Create(model, bullet, player, gamescene, ET_Normal);
	}
	else if (enemyName.find("E1P") == findSuccess)
	{
		//�N���{�[���G��(�U���͋��߁@���x�x��)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Power);
	}
	else if (enemyName.find("E1G") == findSuccess)
	{
		//�N���{�[���G��(���C�t���߁@�U���͎��)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Guard);
	}
	else if (enemyName.find("E1S") == findSuccess)
	{
		//�N���{�[���G��(���x���߁@���C�t���Ȃ�)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Speed);
	}
	else if (enemyName.find("E1D") == findSuccess)
	{
		//�N���{�[���G��(�U���͂قڑ����@�����)
		return Enemy1::Create(model, bullet, player, gamescene, ET_Death);
	}

	else if (enemyName.find("ENEMY2") == findSuccess)
	{
		//�h�b�X�����G��
		return Enemy2::Create(model, bullet, player, gamescene, ET_Normal);
	}
	else if (enemyName.find("E2P") == findSuccess)
	{
		//�h�b�X�����G��(�U���͋��߁@���x�x��)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Power);
	}
	else if (enemyName.find("E2G") == findSuccess)
	{
		//�h�b�X�����G��(���C�t���߁@�U���͎��)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Guard);
	}
	else if (enemyName.find("E2S") == findSuccess)
	{
		//�h�b�X�����G��(���x���߁@���C�t���Ȃ�)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Speed);
	}
	else if (enemyName.find("E2D") == findSuccess)
	{
		//�h�b�X�����G��(�U���͂قڑ����@�����)
		return Enemy2::Create(model, bullet, player, gamescene, ET_Death);
	}

	else if (enemyName.find("BOSS1") == findSuccess)
	{
		//�x�W�F�Ȑ��œ����{�X
		return EnemyBoss::Create(model, bullet, player, gamescene);
	}
	else if (enemyName.find("CORE1") == findSuccess)
	{
		//�R�A
		return EnemyCore::Create(model,bullet, player, gamescene);
	}

	return nullptr;
}
