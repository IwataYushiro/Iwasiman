#pragma once
#include "Object3d.h"
#include "EnemyBullet.h"
#include <DirectXMath.h>

//�O���錾
//���@�N���X
class Player;
//�R���W�����}�l�[�W���[
class CollisionManager;
//�Q�[���v���C�V�[��
class GamePlayScene;

/*

*	BaseEnemy.h (cpp X)

*	�G��ՃN���X

*/
class BaseEnemy :public Object3d
{
private:
	using XMFLOAT3 = DirectX::XMFLOAT3;
public:
	//�f�X�g���N�^
	virtual ~BaseEnemy() = default;

	//������
	virtual bool Initialize() { return Object3d::Initialize(); }
	//������(�T�u�����w���)
	virtual bool Initialize([[maybe_unused]] unsigned short subAttribute ) { return Object3d::Initialize(); }

	//�X�V
	virtual void Update(bool isStart = false) = 0;
	//�`��
	virtual void Draw() { Object3d::Draw(); }

protected:
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;

	//���@
	Player* player_ = nullptr;

	//�Q�[���v���C�V�[��
	GamePlayScene* gameScene_ = nullptr;

	//���S�t���O
	bool isDead_;
	//���C�t
	int life_;
	//���x
	XMFLOAT3 speed_;

	//�{�X�̎��S�t���O
	bool bossDead_;

public://�A�N�Z�b�T
	//���񂾂��ǂ���
	virtual bool IsDead() const { return isDead_; }
	//�{�X�����񂾂��ǂ���
	virtual bool BossDead()const { return bossDead_; }
};

