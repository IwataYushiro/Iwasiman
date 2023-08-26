#pragma once
#include "BaseEnemy.h"

#include "Camera.h"
#include "EnemyBullet.h"
#include "Model.h"
#include <chrono>
#include <DirectXMath.h>
#include <list>
#include <memory>

//�G
class EnemyBoss :public BaseEnemy {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~EnemyBoss();
	static std::unique_ptr<EnemyBoss> Create(Model* model = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr
		, unsigned short subAttribute = 0b1000000000000001);
	//�e���ˊԊu
	static const int kFireInterval = 40;
	//������
	bool Initialize(unsigned short subAttribute)override;

	//���Z�b�g����
	void Reset();
	
	//�p�����[�^
	void Parameter(unsigned short subAttribute);
	//�X�V
	void Update()override;
	//�]���@
	void Trans();
	//�e����
	void Fire();
	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();
	//�`��
	void Draw()override;

	//��ԕω��p�̍X�V�֐�
	//�ڋ�
	void UpdateApproach();
	//�U��
	void UpdateAttack();

	//�R�A�X�V
	void UpdateCore();
	//�R�A���j
	void UpdateBreakCore();

	//���E
	void UpdateLeave();

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
		ApproachStage1, //�ڋ�
		AttackStage1,   //�U��
		//�R�A
		CoreStage1,
		CoreBreak,
		//�S��
		Leave, //���E
		
	};

	//�t�F�[�Y
	Phase phase_;
	//�e���˃^�C�}�[
	int32_t fireTimer = 0;
	
	//���a
	float radius_ = 5.0f;
	//�{�X���S
	bool bossDead = false;

//���Ԍv��
	std::chrono::steady_clock::time_point startCount;	//�J�n����
	std::chrono::steady_clock::time_point nowCount;		//���ݎ���
	std::chrono::microseconds elapsedCount;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	float	maxTime = 5.0f;					//�S�̎���
	float	timeRate;
	//����_
	XMFLOAT3 start;
	XMFLOAT3 p1;
	XMFLOAT3 p2;
	XMFLOAT3 end;
	
	//���S�t���O�ƃ��C�t
	bool isDead_;
	int life_;

	//���]�t���O
	bool isReverse_ = false;
	//���݈ʒu�擾
	XMFLOAT3 nowPos = {};

public:
	bool IsDead() const { return isDead_; }
	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
	bool BossDead()const { return bossDead; }
};