#pragma once
#include "Object3d.h"
#include "Model.h"
#include <DirectXMath.h>

//���L�����̒e
class PlayerBullet {
private:
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	//������
	void Initialize(Model* model, Object3d* obj, const XMFLOAT3& position, const XMFLOAT3& velocity);
	//���Z�b�g����
	void Reset();

	//�X�V
	void Update();

	//�`��
	void Draw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

private:
	
	//���f��
	Model* model_ = nullptr;
	Object3d* obj_ = nullptr;
	//���x
	XMFLOAT3 velocity_;
	//����<frm>
	static const int32_t kLifeTime = 60 * 5;
	//���S����
	int32_t deathTimer_ = kLifeTime;
	//���S�t���O
	bool isDead_ = false;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }
};