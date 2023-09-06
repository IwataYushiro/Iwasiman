#pragma once
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include "ParticleManager.h"

#include <DirectXMath.h>
#include <list>
#include <memory>
#include <chrono>

class CollisionManager;
class GamePlayScene;

class Player:public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	const int MUTEKI_COUNT = 60;
public:
	~Player();

	static std::unique_ptr<Player> Create(Model* model = nullptr,
		GamePlayScene* gamescene = nullptr);
	//������
	bool Initialize() override;
	//���Z�b�g����
	void Reset();
	
	//�X�V
	void Update() override;
	//�v���C���[�̈ړ�����
	void Move();
	void CameraMove();
	//�v���C���[�̍U������
	void Attack();

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();
	void DrawParticle();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

	//�x�W�F�Ȑ�
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//�e���X�g���擾
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	static CollisionManager* colManager_;
	//�e
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	bool isRight_ = true;
	//���f��
	Model* modelBullet_ = nullptr;
	
	//�C���v�b�g
	Input* input_ = nullptr;
	
	//�|�W�V����
	XMFLOAT3 pos;
	//�A���O��
	XMFLOAT3 angle;

	//�W�����v���Ă邩
	bool onGround = true;
	float jumpVYFist = 2.0f;
	XMFLOAT3 fallVec;

	//�����Ɉړ�
	bool isJumpBack;
	bool isBack;

	//���Ԍv��
	std::chrono::steady_clock::time_point startCount;	//�J�n����
	std::chrono::steady_clock::time_point nowCount;		//���ݎ���
	std::chrono::microseconds elapsedCount;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	float	maxTime = 1.0f;					//�S�̎���
	float	timeRate;
	//����_
	XMFLOAT3 start;
	XMFLOAT3 p1;
	XMFLOAT3 p2;
	XMFLOAT3 end;

	//���a
	float radius_ = 1.0f;
	
	//���S�t���O�ƃ��C�t
	bool isDead_ = false;
	int mutekiCount = 0;
	int life_ = 5;
	bool ishit = false;
	bool cameramove = true;
	//�p�[�e�B�N��
	Particle* particleDash_ = nullptr;
	ParticleManager* pmDash_ = nullptr;

	//�Q�[���V�[��
	GamePlayScene* gameScene_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }
	bool OnGround()const { return onGround; }
	void SetOnGround(bool og) { this->onGround = og; }

	//�W�����v��
	void SetJumpVYFist(float jumpFist) { this->jumpVYFist = jumpFist; }
	//���C�t
	void SetLife(int life) { this->life_ = life; }
	const int& GetLife()const { return life_; }
	//�Q�[���V�[��
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};
