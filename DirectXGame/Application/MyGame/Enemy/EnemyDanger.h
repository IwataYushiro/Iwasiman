#pragma once
#include "BaseEnemy.h"
#include "Camera.h"
#include "Model.h"
#include "ParticleManager.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	EnemyDanger.h

*	�댯�ȓG

*/
class EnemyDanger :public BaseEnemy {
public:
	//�f�X�g���N�^
	~EnemyDanger();
	//����(�g�p���f���A�g�p�e���f���A�v���C���[�A�Q�[���v���C�V�[���A�ǂ̕����ɐi�ނ�)
	static std::unique_ptr<EnemyDanger> Create(const Model* model = nullptr,const Player* player = nullptr,
		GamePlayScene* gamescene = nullptr, unsigned short xyz = XYZ_X);

	//������
	bool Initialize()override;

	//���Z�b�g����
	void Reset();

	//�p�����[�^
	void Parameter();
	//�X�V
	void Update(const bool isStart = false)override;
	//�`��
	void Draw()override;
	//�p�[�e�B�N���`��
	void DrawParticle()override;

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, const unsigned short attribute,
		const unsigned short subAttribute)override;

private:

	//���a
	const float radius_ = 10.0f;
	//�{�X���S
	bool bossDead_ = false;

	//���S�t���O
	bool isDead_;
	//���C�t
	int life_;

	//���]�t���O
	bool isReverse_ = false;
	
	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

public:
	//���񂾂��ǂ���
	bool IsDead() const override { return isDead_; }
	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }
	//�Q�[���V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }
	//�����ڂ��p�[�e�B�N���ŕ�������
	void UpdateParticleSkin();
};