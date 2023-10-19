#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Spike.h

*	������ƒɂ��g�Q

*/
class Spike :public BaseGimmick
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//����(�g�p���f���A�v���C���[)
	static std::unique_ptr<Spike> Create(Model* model = nullptr, Player* player = nullptr);
	//������
	bool Initialize()override;
	//�X�V
	void Update()override;
	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw()override;

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;
	//���a
	float radius_ = 5.0f;

	//�v���C���[
	Player* player_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	//�v���C���[�Z�b�g
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once
