#pragma once
#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Enemy2.h

*	�����Ă͗�����G���G

*/
class Enemy2 :public BaseEnemy {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	int MAX_GROUND = 60;

public:
	~Enemy2();
	static std::unique_ptr<Enemy2> Create(Model* model = nullptr, Model* bullet = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr, int level = 1);

	//�e���ˊԊu
	int kFireInterval;

	//������
	bool Initialize(int level);
	void InitSubATTR(int level);
	void InitSpeed();
	void InitLIfe();

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
	void Draw()override;

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
	int32_t fireTimer_ = 0;
	//�X�P�[��
	XMFLOAT3 scale_;
	//�|�W�V����
	XMFLOAT3 pos_;
	//�n�ʂɓ���������
	XMFLOAT3 upPos_;
	//�A���O��
	XMFLOAT3 angle_;
	//���a
	float radius_ = 3.0f;
	//�P�ރX�s�[�h
	XMFLOAT3 backSpeed_;

	bool onGround_ = true;
	XMFLOAT3 fallVec_;

	//���]�t���O
	bool isReverse_ = false;

	int count_ = 0;

public:

	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};