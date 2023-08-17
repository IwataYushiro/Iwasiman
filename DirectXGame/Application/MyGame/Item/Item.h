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

class Player;
class CollisionManager;

//�A�C�e���Ǘ�
class Item :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�萔
	const float MAX_TIME = 200.0f;
public:
	~Item();
	
	static std::unique_ptr<Item> Create(Model* model = nullptr, Player* player = nullptr
		, unsigned short subAttribute = 0b1000000000000000);
	//������
	bool Initialize(unsigned short subAttribute);
	//�X�V
	void Update()override;
	//�W�����v����
	void UpdateJumpPowerup();
	//���C�t��(�͕ʂ�update�����Ă��悳����)

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

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute,unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//�|�W�V����
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isGet_ = false;
	//�W�����v
	bool isGetJump_ = false;

	float radius_ = 3.0f;

	Player* player_ = nullptr;

	//�C�[�W���O
	float count = 0.0f;
	const float timer = MAX_TIME / 60.0f;
	Easing ease = Easing(1.0f, 0.0f, timer);

	//�p�[�e�B�N��
	Particle* p = nullptr;
	ParticleManager* pm_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsGetJump() const { return isGetJump_; }

	void SetPlayer(Player* player) { player_ = player; }
	Easing GetEasing()const { return ease; }

private:
	SpriteCommon* spCommon_ = SpriteCommon::GetInstance();
	Sprite* spriteItemJumpBar_ = new Sprite();

private:
	void LoadSprite();
};

