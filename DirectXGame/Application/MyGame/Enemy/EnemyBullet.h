#pragma once
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>

class CollisionManager;

/*

*	EnemyBullet.h

*	�G�̒e

*/
class EnemyBullet: public Object3d {
private:
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	static std::unique_ptr<EnemyBullet> Create(const XMFLOAT3& position, const XMFLOAT3& velocity, Model* model = nullptr);
	//������
	bool Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity);
	//���Z�b�g����
	void Reset();

	//�X�V
	void Update()override;
	//�`��
	void Draw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

private:
	static CollisionManager* colManager_;

	//���x
	XMFLOAT3 velocity_;

	//����
	static const int32_t kLifeTime_ = 60 * 5;
	//���S����
	int32_t deathTimer_ = kLifeTime_;
	//���S�t���O
	bool isDead_ = false;
	//���a
	float radius_ = 4.0f;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }
};