#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "Model.h"
#include "ParticleManager.h"

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
	static std::unique_ptr<Enemy1> Create(const Model* model = nullptr, const Model* bullet = nullptr,
		const Player* player = nullptr,GamePlayScene* gamescene = nullptr, int level = 1);

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
	//���[���h���W���擾
	const XMFLOAT3 GetWorldPosition() const;
	//�`��
	void Draw()override;
	//�p�[�e�B�N���`��
	void DrawParticle()override;

	//��ԕω��p�̍X�V�֐�
	//�ڋ�
	void UpdateApproach();
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
		Approach,//�o��
		Leave, //���E

	};

	//�t�F�[�Y
	Phase phase_;

	//�e���ˊԊu
	int fireInterval_;
	//�e���˃^�C�}�[
	int32_t fireTimer_;
	//�A���O��
	XMFLOAT3 angle_;
	//���a
	const float radius_ = 3.0f;
	
	//�n�ʂɏ���Ă邩
	bool onGround_ = true;
	//�������x�N�g��
	XMFLOAT3 fallVec_;

	//���]�t���O
	bool isReverse_ = false;

	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;
public:
	
	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }
	//�Q�[���v���C�V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
};