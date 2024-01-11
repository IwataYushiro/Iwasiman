#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "ParticleManager.h"
#include "Input.h"

#include <chrono>
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	EnemyBoss.h

*	���E�ɓ����{�X�G

*/
class EnemyBoss :public BaseEnemy {
public:
	//�f�X�g���N�^
	~EnemyBoss();
	//����(�g�p���f���A�g�p�e���f���A�v���C���[�A�Q�[���v���C�V�[��,�{�X�X�e�[�W���ǂ���)
	static std::unique_ptr<EnemyBoss> Create(const Model* model = nullptr, const Model* bullet = nullptr,
		const Player* player = nullptr, GamePlayScene* gamescene = nullptr, const bool isNotBossStage = false);
	
	//������
	bool Initialize()override;

	//���Z�b�g����
	void Reset();
	
	//�p�����[�^
	void Parameter();
	//�X�V
	void Update(const bool isStart = false)override;
	//�e����
	void Fire();
	//�`��
	void Draw()override;
	//�p�[�e�B�N���`��
	void DrawParticle()override;

	//�x�W�F�Ȑ�(�ŏ��_�A���ԓ_1�A���ԓ_2�A�ŏI�_�A���Ԃ̐i�݋)
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, const unsigned short attribute,
		const unsigned short subAttribute)override;
	
private:
	//���f��	
	const Model* modelBullet_ = nullptr;

	//�s���t�F�[�Y
	enum class Phase {
		//��������X�e�[�W1
		ApproachStage1, //�ڋ�
		AttackStage1,   //�U��
		//�S��
		Leave, //���E
		
	};

	//�t�F�[�Y
	Phase phase_;
	//�e���ˊԊu
	int fireInterval_;
	//�e���˃^�C�}�[
	int32_t fireTimer_;
	
	//���a
	const float radius_ = 5.0f;
	//�{�X���S
	bool bossDead_ = false;

	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

//���Ԍv��
	std::chrono::steady_clock::time_point startCount_;	//�J�n����
	std::chrono::steady_clock::time_point nowCount_;	//���ݎ���
	std::chrono::microseconds elapsedCount_;			//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	const float	maxTime_ = 5.0f;						//�S�̎���
	float	timeRate_;									//�ǂꂭ�炢���Ԃ��i�񂾂�
	//����_
	XMFLOAT3 start_;	//�ŏ��_
	XMFLOAT3 point1_;	//���ԓ_1
	XMFLOAT3 point2_;	//���ԓ_2
	XMFLOAT3 end_;		//�ŏI�_
	
	//���S�t���O
	bool isDead_;
	//���C�t
	int life_;

	//���]�t���O
	bool isReverse_ = false;
	//����
	IwasiEngine::Input* input_ = nullptr;

public:
	//���񂾂��ǂ���
	bool IsDead() const override { return isDead_; }
	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }
	//�Q�[���V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
	//�{�X�����񂾂��ǂ���
	bool BossDead()const override{ return bossDead_; }
private:
	//�x�W�F�Ȑ��̓��������ʉ�(�f�t�H���g�����̓{�X�X�e�[�W�̏ꍇ)
	void UpdateBezierMove(const bool notStageBoss = false);
	//X���W���v���C���[�̓����ɘA�������邽�߂̊֐�(�ʏ�X�e�[�W����)
	void UpdateSynchronizePlayerMove(const bool notStageBoss);

public://��ԕω��p�̍X�V
	//��ԕω��p�̍X�V�֐�
	//�ڋ�
	void UpdateApproach();
	//�U��
	void UpdateAttack();
	//���E
	void UpdateLeave();
	
private:
	//�����o�֐��|�C���^�̃e�[�u��
	static void (EnemyBoss::* updateTable_[])();
};