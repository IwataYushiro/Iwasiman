#pragma once
#include "Camera.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "Object3d.h"
#include <chrono>
#include <DirectXMath.h>
#include <list>
#include <memory>

//���@�N���X�̑O���錾
class Player;
class CollisionManager;
class GamePlayScene;

//�G
class Enemy:public Object3d {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~Enemy();
	static std::unique_ptr<Enemy> Create(XMFLOAT3 pos,Model* model = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr);
	//�e���ˊԊu
	static const int kFireIntervalStage1 = 40;
	//������
	bool Initialize(XMFLOAT3 pos);

	//���Z�b�g����
	void Reset(XMFLOAT3 pos);
	
	//�p�����[�^
	void Stage1Parameter(XMFLOAT3 pos);
	//�X�V
	void Update()override;
	//�]���@
	void Trans();
	//�e����
	void Fire();
	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();
	//�`��
	void Draw();

	//��ԕω��p�̍X�V�֐�
	//�ڋ�
	void UpdateApproachStage1();
	//�U��
	void UpdateAttackStage1();
	//���E
	void UpdateLeave();

	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute)override;
	void OnCollisionPlayer();
	
private:
	static CollisionManager* colManager_;
	
	//���f��	
	Model* modelBullet_ = nullptr;

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
	//�e���˃^�C�}�[
	int32_t fireTimer = 0;
	//�X�P�[��
	XMFLOAT3 scale;
	//�|�W�V����
	XMFLOAT3 pos;
	//�A���O��
	XMFLOAT3 angle;
	//���a
	float radius_ = 5.0f;
	//���@
	Player* player_ = nullptr;

	//�Q�[���V�[��
	GamePlayScene* gameScene_ = nullptr;

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


public:
	bool IsDead() const { return isDead_; }
	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};