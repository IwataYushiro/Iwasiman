#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	FallAndRiseSphere.h

*	�����鋅�Ə��鋅

*/
class FallAndRiseSphere :public BaseGimmick
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	//����(�g�p���f���A�v���C���[�A�T�u����)
	static std::unique_ptr<FallAndRiseSphere> Create(Model* model = nullptr, Player* player = nullptr, unsigned short subAttribute = 0b1000000000000001);
	//������
	bool Initialize()override;
	//�X�V
	void Update()override;
	//�X�V(��������)
	void UpdateFallSphere();
	//�߂�
	void UpdateFallSphereReturn();
	//�X�V(�̂ڂ����)
	void UpdateRiseSphere();
	//�߂�
	void UpdateRiseSphereReturn();

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw()override;

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//�|�W�V����
	XMFLOAT3 pos_;
	//�X�P�[��
	XMFLOAT3 scale_;

	//���a
	float radius_ = 9.0f;

	//�v���C���[
	Player* player_ = nullptr;

	//����true
	bool isRide_ = false;
	//����ė�����true
	bool isReturn_ = false;

	//�J�n�n�_
	XMFLOAT3 startPos_ = {};

public: //�A�N�Z�b�T�A�C�����C���֐�
	
	//�v���C���[�Z�b�g
	void SetPlayer(Player* player) { player_ = player; }
};