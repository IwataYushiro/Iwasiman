#pragma once
#include "Camera.h"
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class Player
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	~Player();

	//������
	void Initialize(Model* model, Object3d* obj, Input* input, Camera* camera);
	//���Z�b�g����
	void Reset();
	
	//�X�V
	void Update();

	//�v���C���[�̈ړ�����
	void Move();
	void CameraMove();

	//�v���C���[�̍U������
	void Attack();

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision();

	//�e���X�g���擾
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	//�e
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	
	//���f��
	Model* model_ = nullptr;
	Model* modelBullet_ = nullptr;

	Object3d* obj_ = nullptr;
	Object3d* objBullet_ = nullptr;
	//�J����
	Camera* camera_ = nullptr;

	//�C���v�b�g
	Input* input_ = nullptr;
	
	//�|�W�V����
	XMFLOAT3 pos;
	//�A���O��
	XMFLOAT3 angle;

	//���S�t���O�ƃ��C�t
	bool isDead_ = false;
	int life_ = 5;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }
};
