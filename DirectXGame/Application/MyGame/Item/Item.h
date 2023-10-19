#pragma once
#include "Camera.h"
#include "Easing.h"
#include "ParticleManager.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include "Sprite.h"
#include <list>
#include <memory>

//�O���錾
//���@�N���X
class Player;
//�R���W�����}�l�[�W���[
class CollisionManager;

/*

*	Item.h

*	�A�C�e���Ǘ�

*/
class Item :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�萔
	//�ő���ʎ���
	const float MAX_TIME = 200.0f;
public:
	~Item();
	
	//����(�g�p���f���A�v���C���[�A�T�u����)
	static std::unique_ptr<Item> Create(Model* model = nullptr, Player* player = nullptr
		, unsigned short subAttribute = 0b1000000000000001);
	//������
	bool Initialize()override;
	//���Z�b�g
	void Reset();
	//�X�V
	void Update()override;
	//�X�V(�W�����v�����A�C�e��)
	void UpdateJumpPowerup();

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();
	//�p�[�e�B�N���`��
	void DrawParticle();
	//�X�v���C�g�`��
	void DrawSprite();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, unsigned short attribute,unsigned short subAttribute)override;

private:
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;
	//�擾��
	bool isGet_ = false;
	//�W�����v�A�C�e�����擾������
	bool isGetJump_ = false;
	//���a
	float radius_ = 3.0f;
	//�v���C���[
	Player* player_ = nullptr;

	//�C�[�W���O
	//�A�C�e���擾�������̌��ݎ���
	float count_ = 0.0f;
	//�^�C�}�[�̒l�܂ŗ�������ʏI��
	const float timer_ = MAX_TIME / 60.0f;
	//�擾UI�Ɏg���C�[�W���O
	Easing ease_ = Easing(1.0f, 0.0f, timer_);

	//�p�[�e�B�N��
	Particle* p_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pm_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	//�W�����v�A�C�e�����擾�������ǂ���
	bool IsGetJump() const { return isGetJump_; }

	//�v���C���[�Z�b�g
	void SetPlayer(Player* player) { player_ = player; }

private:
	//�X�v���C�g��ՃN���X
	SpriteCommon* spCommon_ = SpriteCommon::GetInstance();
	//�W�����v�A�C�e���擾����UI�X�v���C�g
	Sprite* spriteItemJumpBar_ = new Sprite();

private:
	//�X�v���C�g�̃��[�h
	void LoadSprite();
};

