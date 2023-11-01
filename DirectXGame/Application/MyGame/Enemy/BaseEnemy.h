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
	//�p�[�e�B�N���`��
	virtual void DrawParticle() = 0;

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

	//�G����
	enum EnemyType
	{
		ET_Normal = 1,	//�ʏ�
		ET_Power = 2,	//�U���^
		ET_Guard = 3,	//�h��^
		ET_Speed = 4,	//���x�^
		ET_Death = 5,	//�댯�^
	};
	//����
	const int endFireTime_ = 0;
	//���C�t0�ȉ��Ŏ��S
	const int deathLife_ = 0;
	
	//�e���ˎ��Ԃ̓����_��

	//���S����
	int32_t deathTimer_ = 0;
	//�^�C�}�[�����̈ʒu�ɒB�����玀�S
	const int32_t DEATH_TIME = 70;

public://�A�N�Z�b�T
	//���񂾂��ǂ���
	virtual bool IsDead() const { return isDead_; }
	//�{�X�����񂾂��ǂ���
	virtual bool BossDead()const { return bossDead_; }
};

