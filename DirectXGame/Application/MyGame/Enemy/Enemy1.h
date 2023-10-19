#pragma once
#include "BaseEnemy.h"

#include "Camera.h"

#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Enemy1.h

*	���̂܂ܕ���������G

*/
class Enemy1 :public BaseEnemy {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�f�X�g���N�^
	~Enemy1();

	//����(�g�p���f���A�g�p�e���f���A�v���C���[�A�Q�[���v���C�V�[���A�p�����[�^���x��)
	static std::unique_ptr<Enemy1> Create(Model* model = nullptr, Model* bullet = nullptr,
		Player* player = nullptr, GamePlayScene* gamescene = nullptr, int level = 1);

	//������
	bool Initialize(int level);
	//�T�u����������
	void InitSubATTR(int level);
	//�T�u�����ʑ��x������
	void InitSpeed();
	//�T�u�����ʃ��C�t������
	void InitLife();

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

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, unsigned short attribute,
		unsigned short subAttribute)override;

private:
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;

	//���f��	
	Model* modelBullet_ = nullptr;

	//�s���t�F�[�Y
	enum class Phase {
		//��������X�e�[�W1
		Approach,//�o��
		Leave, //���E

	};

	//�t�F�[�Y
	Phase phase_;

	//�e���ˊԊu
	int fireInterval_;
	//�e���˃^�C�}�[
	int32_t fireTimer_ = 0;
	//�A���O��
	XMFLOAT3 angle_;
	//���a
	float radius_ = 1.0f;
	
	//�n�ʂɏ���Ă邩
	bool onGround_ = true;
	//�������x�N�g��
	XMFLOAT3 fallVec_;

	//���]�t���O
	bool isReverse_ = false;

public:
	
	//�v���C���[�Z�b�g
	void SetPlayer(Player* player) { player_ = player; }
	//�Q�[���v���C�V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};