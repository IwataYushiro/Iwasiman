#pragma once
#include "Object3d.h"

//�O���錾
//���@�N���X
class Player;
//�R���W�����}�l�[�W���[
class CollisionManager;

/*

*	BaseGimmick.h (cpp X)

*	�M�~�b�N��ՃN���X

*/
class BaseGimmick :public Object3d
{
public:
	//�f�X�g���N�^
	virtual ~BaseGimmick() = default;

	//������
	virtual bool Initialize() = 0;
	
	//�X�V
	virtual void Update() = 0; //{ Object3d::Update(); }
	//�`��
	virtual void Draw() = 0; //{ Object3d::Draw(); }

};

