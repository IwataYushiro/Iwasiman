#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

//�O���錾
//�R���W�����}�l�[�W���[
class CollisionManager;

/*

*	Goal.h

*	�S�[��

*/
class Goal :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	
	//����(�g�p���f��)
	static std::unique_ptr<Goal> Create(Model* model = nullptr);
	//������
	bool Initialize()override;
	//���Z�b�g
	void Reset();
	//�X�V
	void Update()override;

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;
	//�S�[��������
	bool isGoal_ = false;
	//���a
	const float radius_ = 10.0f;

public: //�A�N�Z�b�T�A�C�����C���֐�
	//�S�[��������
	bool IsGoal() const { return isGoal_; }
};
#pragma once
