#pragma once
#include "Object3d.h"
#include "EnemyBullet.h"

//���@�N���X�̑O���錾
class Player;
class CollisionManager;
class GamePlayScene;

//�G��ՃN���X
class BaseEnemy :public Object3d
{
public:
	virtual ~BaseEnemy() = default;

	virtual bool Initialize() = 0;

	//�X�V
	virtual void Update() = 0;
	//�`��
	virtual void Draw() = 0;

protected:
	static CollisionManager* colManager_;

	//���@
	Player* player_ = nullptr;

	//�Q�[���V�[��
	GamePlayScene* gameScene_ = nullptr;

	//���S�t���O�ƃ��C�t
	bool isDead_;
	int life_;
public://�A�N�Z�b�T
	bool IsDead() const { return isDead_; }
};

