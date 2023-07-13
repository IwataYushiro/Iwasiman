#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>

class Goal :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	
	static Goal* Create(Model* model = nullptr);
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

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info)override;

private:
	
	//�|�W�V����
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isGoal_ = false;

	float radius_ = 10.0f;
public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isGoal_; }
};
#pragma once
