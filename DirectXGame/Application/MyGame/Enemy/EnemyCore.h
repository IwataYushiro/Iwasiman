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
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�f�X�g���N�^
	~EnemyCore();
	////����(�g�p���f���A�g�p�e���f���A�v���C���[�A�Q�[���v���C�V�[���A�p�����[�^���x��)
	static std::unique_ptr<EnemyCore> Create(Model* model = nullptr, Model* bullet = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr, unsigned short stage = 1);

	
	//������
	bool Initialize()override;

	//���Z�b�g����
	void Reset();

	//�p�����[�^
	void Parameter();
	//�X�V
	void Update(bool isStart = false)override;
	//�]���@
	void Trans();
	//�e����
	void Fire();
	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();
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
	void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;

	//���f��	
	Model* modelBullet_ = nullptr;

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
	int32_t fireTimer_ = 0;

	//���a
	float radius_ = 10.0f;
	//�{�X���S
	bool bossDead_ = false;

	//���Ԍv��
	std::chrono::steady_clock::time_point startCount_;	//�J�n����
	std::chrono::steady_clock::time_point nowCount_;	//���ݎ���
	std::chrono::microseconds elapsedCount_;			//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	float	maxTime_ = 2.0f;							//�S�̎���
	float	timeRate_;									//�ǂꂭ�炢���Ԃ��i�񂾂�
	//����_
	XMFLOAT3 start_;		//�ŏ��_
	XMFLOAT3 point1_;	//���ԓ_1
	XMFLOAT3 point2_;	//���ԓ_2
	XMFLOAT3 end_;		//�ŏI�_

	//���S�t���O
	bool isDead_;
	//���C�t
	int life_;

	//���]�t���O
	bool isReverse_ = false;
	//���݈ʒu�擾
	XMFLOAT3 nowPos_ = {};

	//�p�[�e�B�N��
	Particle* particleFire_ = nullptr;
	Particle* particleSmoke_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pmSmoke_ = nullptr;
	ParticleManager* pmFire_ = nullptr;

public:
	//���񂾂��ǂ���
	bool IsDead() const override{ return isDead_; }
	//�v���C���[�Z�b�g
	void SetPlayer(Player* player) { player_ = player; }
	//�Q�[���V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};