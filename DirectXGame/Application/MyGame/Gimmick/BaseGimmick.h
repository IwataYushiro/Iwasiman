#pragma once
#include "Object3d.h"

//���@�N���X�̑O���錾
class Player;
class CollisionManager;

/*

*	BaseGimmick.h (cpp X)

*	�M�~�b�N��ՃN���X

*/
class BaseGimmick :public Object3d
{
public:
	virtual ~BaseGimmick() = default;

	virtual bool Initialize() = 0; //{ return Object3d::Initialize(); }

	//�X�V
	virtual void Update() = 0; //{ Object3d::Update(); }
	//�`��
	virtual void Draw() = 0; //{ Object3d::Draw(); }

};

