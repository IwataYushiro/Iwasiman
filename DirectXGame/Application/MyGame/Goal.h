#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>

class Goal
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	
	//������
	void Initialize(Model* model, Object3d* obj, Camera* camera);
	//�X�V
	void Update();

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision();

private:
	
	//���f��
	Model* model_ = nullptr;

	Object3d* obj_ = nullptr;
	//�J����
	Camera* camera_ = nullptr;

	//�|�W�V����
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isGoal_ = false;
public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isGoal_; }
};
#pragma once
