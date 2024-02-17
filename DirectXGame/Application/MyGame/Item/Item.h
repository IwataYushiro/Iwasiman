#pragma once
#include "Camera.h"
#include "Easing.h"
#include "ParticleManager.h"
#include "Model.h"
#include "Object3d.h"
#include "Vector2.h"
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
class Item :public IwasiEngine::Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::���ȗ�
	using Sprite = IwasiEngine::Sprite;
	using SpriteCommon = IwasiEngine::SpriteCommon;
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using Easing = IwasiEngine::Easing;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;
	using CollisionInfo = IwasiEngine::CollisionInfo;
	using Vector2 = IwasiEngine::Vector2;

public://�萔
	//�ő���ʎ���
	const float MAX_TIME = 210.0f;
public:
	~Item();
	
	//����(�g�p���f���A�v���C���[�A�T�u����)
	static std::unique_ptr<Item> Create(const Model* model = nullptr, const Player* player = nullptr
		,const unsigned short subAttribute = 0b1000000000000001);
	//������
	bool Initialize()override;
	//���Z�b�g
	void Reset();
	//�X�V
	void Update()override;
	//�X�V(�W�����v�����A�C�e��)
	void UpdateJumpPowerup();
	//�`��
	void Draw();
	//�p�[�e�B�N���`��
	void DrawParticle();
	//�X�v���C�g�`��
	void DrawSprite();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute,const unsigned short subAttribute)override;

private:
	//�擾��
	bool isGet_ = false;
	//�W�����v�A�C�e�����擾������
	bool isGetJump_ = false;
	//�񕜃A�C�e�����擾������
	bool isGetHeal_ = false;
	//���a
	const float radius_ = 3.0f;
	//�v���C���[
	const Player* player_ = nullptr;

	//�C�[�W���O
	//�A�C�e���擾�������̌��ݎ���
	float count_;
	//�^�C�}�[�̒l�܂ŗ�������ʏI��
	const float timer_ = MAX_TIME / 60.0f;
	//�擾UI�Ɏg���C�[�W���O
	Easing ease_;

	//�p�[�e�B�N��
	std::unique_ptr<Particle> p_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pm_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	//�W�����v�A�C�e�����擾�������ǂ���
	bool IsGetJump() const { return isGetJump_; }

	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }

private:
	//�X�v���C�g��ՃN���X
	SpriteCommon* spCommon_ = nullptr;
	//�W�����v�A�C�e���擾����UI�X�v���C�g
	std::unique_ptr<Sprite> spriteItemJumpBar_ = std::make_unique<Sprite>();

private:
	//�X�v���C�g�̃��[�h
	void LoadSprite();
};

