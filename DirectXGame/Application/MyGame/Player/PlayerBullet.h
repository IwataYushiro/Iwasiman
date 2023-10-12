#pragma once
#include "Object3d.h"
#include "Model.h"
#include <DirectXMath.h>

//�O���錾
//�R���W�����}�l�[�W���[
class CollisionManager;

/*

*	PlayerBullet.h

*	���@�̒e

*/
class PlayerBullet:public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//����(�������W�A���x�A�g�p���f��)
	static std::unique_ptr<PlayerBullet> Create
	(const XMFLOAT3& position, const XMFLOAT3& velocity, Model* model = nullptr);
	//������(�������W�A���x)
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
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;

	//���x
	XMFLOAT3 velocity_;
	//����<frm>
	static const int32_t kLifeTime_ = 60;
	//���S����
	int32_t deathTimer_ = kLifeTime_;
	//���S�t���O
	bool isDead_ = false;
	//���a
	float radius_ = 4.0f;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }
};