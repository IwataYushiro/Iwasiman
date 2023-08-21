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
class Enemy1 :public Object3d {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~Enemy1();
	static std::unique_ptr<Enemy1> Create(Model* model = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr);
	//�e���ˊԊu
	static const int kFireInterval = 150;
	//������
	bool Initialize()override;

	//���Z�b�g����
	void Reset();

	//�p�����[�^
	void Parameter();
	//�X�V
	void Update()override;
	//�]���@
	void Trans();
	//�e����
	void Fire();
	//���n����
	void Landing();
	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();
	//�`��
	void Draw();

	//��ԕω��p�̍X�V�֐�
	//�ڋ�
	void UpdateApproach();
	//���E
	void UpdateLeave();

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
		Approach,
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
	float radius_ = 1.0f;
	//���@
	Player* player_ = nullptr;

	//�Q�[���V�[��
	GamePlayScene* gameScene_ = nullptr;

	//���S�t���O�ƃ��C�t
	bool isDead_;
	int life_;

	bool onGround = true;
	XMFLOAT3 fallVec;

	//���]�t���O
	bool isReverse_ = false;


public:
	bool IsDead() const { return isDead_; }
	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};