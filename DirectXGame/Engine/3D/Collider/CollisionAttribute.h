#pragma once

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	CollisionAttribute.h (cpp X)

	*	�����蔻�葮��

	*/
	const unsigned short COLLISION_ATTR_LANDSHAPE = 0b1 << 0;								//�n�`
	const unsigned short COLLISION_ATTR_PLAYERS = 0b1 << 1;									//�����w�c
	const unsigned short COLLISION_ATTR_ENEMYS = 0b1 << 2;									//�G�w�c
	const unsigned short COLLISION_ATTR_GOAL = 0b1 << 3;									//�S�[��
	const unsigned short COLLISION_ATTR_ITEM = 0b1 << 4;									//�A�C�e��
	const unsigned short COLLISION_ATTR_GIMMICK = 0b1 << 5;									//�d�|��


	//�T�u����
	const unsigned short SUBCOLLISION_ATTR_NONE = (0b1 << 15) + 1;							//�T�u�����Ȃ�(�m�[�}���^�C�v)
	//�e
	const unsigned short SUBCOLLISION_ATTR_BULLET = (0b1 << 15) + 2;						//�e

	//�A�C�e������
	const unsigned short SUBCOLLISION_ATTR_ITEM_JUMP = (0b1 << 15) + 11;					//�W�����v�A�C�e��
	const unsigned short SUBCOLLISION_ATTR_ITEM_HEAL = (0b1 << 15) + 12;					//�񕜃A�C�e��
	const unsigned short SUBCOLLISION_ATTR_ITEM_ATTACK = (0b1 << 15) + 13;					//�U���͋����A�C�e��
	const unsigned short SUBCOLLISION_ATTR_ITEM_GET_ONCE = (0b1 << 15) + 99;				//�擾�ς݃A�C�e��(���ʂ�1�x����)

	//�d�|���̎��
	const unsigned short SUBCOLLISION_ATTR_GIMMICK_SPIKE = (0b1 << 15) + 100;				//�g�Q
	const unsigned short SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE = (0b1 << 15) + 101;			//���Ɨ����鋅(�~��Ă��������Â���)
	const unsigned short SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN = (0b1 << 15) + 102;	//���Ɨ����鋅(�~���ƌ��̈ʒu�ɖ߂�)
	const unsigned short SUBCOLLISION_ATTR_GIMMICK_UPSPHERE = (0b1 << 15) + 103;			//���Əオ�鋅(�~��Ă��㏸��������)
	const unsigned short SUBCOLLISION_ATTR_GIMMICK_UPSPHERE_RETURN = (0b1 << 15) + 104;		//���Əオ�鋅(�~���ƌ��̈ʒu�ɖ߂�)

	//�G�̌X��
	const unsigned short SUBCOLLISION_ATTR_ENEMY_POWER = (0b1 << 15) + 200;					//�U���^
	const unsigned short SUBCOLLISION_ATTR_ENEMY_GUARD = (0b1 << 15) + 201;					//�h��^
	const unsigned short SUBCOLLISION_ATTR_ENEMY_SPEED = (0b1 << 15) + 202;					//�X�s�[�h�^
	const unsigned short SUBCOLLISION_ATTR_ENEMY_DEATH = (0b1 << 15) + 203;					//������Ƃقڎ��S
	const unsigned short SUBCOLLISION_ATTR_ENEMY_NOTSTAGEBOSS = (0b1 << 15) + 203;			//�{�X�G�����ǃX�e�[�W�ɏo��Ƃ�

	const unsigned short SUBCOLLISION_ATTR_ENEMY_ISDEAD = (0b1 << 15) + 299;				//���Ɏ���

}