#pragma once
#include "Camera.h"
#include "ParticleManager.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include "Sprite.h"
#include <list>
#include <memory>

class Player;
class CollisionManager;

/*

*	ItemHeal.h

*	���@���C�t�񕜃A�C�e��

*/
class ItemHeal :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~ItemHeal();

	static std::unique_ptr<ItemHeal> Create(Model* model = nullptr, Player* player = nullptr);
	//������
	bool Initialize()override;
	//�X�V
	void Update()override;

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();
	//�p�[�e�B�N���`��
	void DrawParticle();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//�|�W�V����
	XMFLOAT3 pos_;
	XMFLOAT3 scale_;

	bool isGet_ = false;

	float radius_ = 3.0f;

	Player* player_ = nullptr;

	Particle* p_ = nullptr;
	ParticleManager* pm_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsGet() const { return isGet_; }
	void SetPlayer(Player* player) { player_ = player; }
};


