#pragma once
///<summary>
///�����蔻�葮��
///</summary>
const unsigned short COLLISION_ATTR_LANDSHAPE = 0b1 << 0; //�n�`
const unsigned short COLLISION_ATTR_PLAYERS= 0b1 << 1;//�����w�c
const unsigned short COLLISION_ATTR_ENEMYS = 0b1 << 2;//�G�w�c
const unsigned short COLLISION_ATTR_GOAL = 0b1 << 3;//�S�[��
const unsigned short COLLISION_ATTR_ITEM = 0b1 << 4;//�A�C�e��

//�T�u����
const unsigned short SUBCOLLISION_ATTR_NONE = (0b1 << 15) + 1;//�T�u�����Ȃ�
const unsigned short SUBCOLLISION_ATTR_BULLET = (0b1 << 15) + 2;//�e
