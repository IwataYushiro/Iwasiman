#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class CollisionManager;

//�S�[��
class Goal :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	
	static std::unique_ptr<Goal> Create(Model* model = nullptr);
	//������
	bool Initialize()override;
	void Reset();
	//�X�V
	void Update()override;

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//�|�W�V����
	XMFLOAT3 pos_;
	XMFLOAT3 scale_;

	bool isGoal_ = false;

	float radius_ = 10.0f;
public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsGoal() const { return isGoal_; }
};
#pragma once
