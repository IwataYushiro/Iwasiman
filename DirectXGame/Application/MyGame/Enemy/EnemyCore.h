#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "ParticleManager.h"
#include <chrono>
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	EnemyCore.h

*	�{�X�G�̃R�A

*/
class EnemyCore :public BaseEnemy {
public:
	//�f�X�g���N�^
	~EnemyCore();
	////����(�g�p���f���A�g�p�e���f���A�v���C���[�A�Q�[���v���C�V�[���A�p�����[�^���x��)
	static std::unique_ptr<EnemyCore> Create(const Model* model = nullptr, const Model* bullet = nullptr,
		const Player* player = nullptr,GamePlayScene* gamescene = nullptr, unsigned short stage = 1);

	
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

	//��ԕω��p�̍X�V�֐�
	//�R�A�X�V
	void UpdateCore();
	//�R�A���j
	void UpdateBreakCore();

	//���E
	void UpdateLeave();

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
		CoreStage1,
		CoreBreak,
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
	const float radius_ = 10.0f;
	//�{�X���S
	bool bossDead_ = false;

	//���Ԍv��
	std::chrono::steady_clock::time_point startCount_;	//�J�n����
	std::chrono::steady_clock::time_point nowCount_;	//���ݎ���
	std::chrono::microseconds elapsedCount_;			//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	const float	maxTime_ = 2.0f;							//�S�̎���
	float	timeRate_;									//�ǂꂭ�炢���Ԃ��i�񂾂�
	//����_
	XMFLOAT3 start_;		//�ŏ��_
	XMFLOAT3 point1_;	//���ԓ_1
	XMFLOAT3 point2_;	//���ԓ_2
	XMFLOAT3 end_;		//�ŏI�_

	//���S�t���O
	bool isDead_;
	//���C�t
	int32_t life_;
	//�������������
	int32_t hit_ = 0;
	//���]�t���O
	bool isReverse_ = false;
	//���݈ʒu�擾
	XMFLOAT3 nowPos_ = {};

	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

public:
	//���񂾂��ǂ���
	bool IsDead() const override{ return isDead_; }
	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }
	//�Q�[���V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }

};