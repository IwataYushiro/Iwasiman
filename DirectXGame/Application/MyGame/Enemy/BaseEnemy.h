#pragma once
#include "Object3d.h"
#include "EnemyBullet.h"
#include <DirectXMath.h>

//���@�N���X�̑O���錾
class Player;
class CollisionManager;
class GamePlayScene;

//�G��ՃN���X
class BaseEnemy :public Object3d
{
private:
	using XMFLOAT3 = DirectX::XMFLOAT3;
public:
	virtual ~BaseEnemy() = default;

	virtual bool Initialize() { return Object3d::Initialize(); }
	virtual bool Initialize(unsigned short subAttribute) { return Object3d::Initialize(); }

	//�X�V
	virtual void Update() { Object3d::Update(); }
	//�`��
	virtual void Draw() { Object3d::Draw(); }

protected:
	static CollisionManager* colManager_;

	//���@
	Player* player_ = nullptr;

	//�Q�[���V�[��
	GamePlayScene* gameScene_ = nullptr;

	//���S�t���O�ƃ��C�t
	bool isDead_;
	int life_;
	//���x
	XMFLOAT3 speed;

	bool bossDead;
public://�A�N�Z�b�T
	virtual bool IsDead() const { return isDead_; }
	virtual bool BossDead()const { return bossDead; }
};

