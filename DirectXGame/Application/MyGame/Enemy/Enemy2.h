#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "Model.h"
#include "ParticleManager.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Enemy2.h

*	�����Ă͗�����G���G

*/
class Enemy2 :public BaseEnemy {
public:
	//�n�ʂɏ���Ă鎞��
	const int MAX_GROUND = 60;

public:
	//�f�X�g���N�^
	~Enemy2();
	//����(�g�p���f���A�g�p�e���f���A�v���C���[�A�Q�[���v���C�V�[���A�p�����[�^���x��)
	static std::unique_ptr<Enemy2> Create(const Model* model = nullptr,const Model* bullet = nullptr,
		const Player* player = nullptr, GamePlayScene* gamescene = nullptr, int level = 1);

	//������
	bool Initialize(const int level);
	//�T�u����������
	void InitSubATTR(const int level);
	//�T�u�����ʑ��x������
	void InitSpeed();
	//�T�u�����ʃ��C�t������
	void InitLife();

	//���Z�b�g����
	void Reset();

	//�p�����[�^
	void Parameter();
	//�X�V
	void Update(const bool isStart = false)override;
	//�]���@
	void Trans();
	//�e����
	void Fire();
	//���n����
	void Landing();
	//�`��
	void Draw()override;
	//�p�[�e�B�N���`��
	void DrawParticle()override;

	//��ԕω��p�̍X�V�֐�
	//�ڋ�
	void UpdateApproach();
	//�߂�
	void UpdateBack();
	//���E
	void UpdateLeave();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute,
		const unsigned short subAttribute)override;

private:
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;

	//���f��	
	const Model* modelBullet_ = nullptr;

	//�s���t�F�[�Y
	enum class Phase {
		//��������X�e�[�W1
		Approach,	//�o��
		Back,		//�߂�
		Leave,		//���E

	};

	//�t�F�[�Y
	Phase phase_;
	//�e���ˊԊu
	int fireInterval_;
	//�e���˃^�C�}�[
	int32_t fireTimer_;
	//�n�ʂɓ���������
	XMFLOAT3 upPos_;
	//�A���O��
	XMFLOAT3 angle_;
	//���a
	const float radius_ = 4.0f;
	//�P�ރX�s�[�h
	XMFLOAT3 backSpeed_;
	//�n�ʂɏ���Ă邩
	bool onGround_ = true;
	//�������x�N�g��
	XMFLOAT3 fallVec_;

	//���]�t���O
	bool isReverse_ = false;
	//�n�ʂɏ���Ă鎞�̃J�E���g
	int count_;

	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;
	//Y�������̒n�_�ɒB������
	const float backFallPosY = -20.0f;//���
	const float backUpPosY = 20.0f;//����

public:

	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }
	//�Q�[���V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};