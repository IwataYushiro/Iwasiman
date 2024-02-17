#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	FallAndRiseGround.h

*	�����鋅�Ə��鋅

*/
class FallAndRiseGround :public BaseGimmick
{
public:

	//����(�g�p���f���A�v���C���[�A�T�u����)
	static std::unique_ptr<FallAndRiseGround> Create(Model* model = nullptr, const Player* player = nullptr,
		const unsigned short subAttribute = 0b1000000000000001);
	//������
	bool Initialize(Model* model);
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

	//�`��
	void Draw()override;

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	//�|�W�V����
	XMFLOAT3 pos_;
	//�X�P�[��
	XMFLOAT3 scale_;

	//���a
	const float radius_ = 8.0f;

	//�v���C���[
	const Player* player_ = nullptr;

	//����true
	bool isRide_ = false;
	//����ė�����true
	bool isReturn_ = false;

	//�J�n�n�_
	XMFLOAT3 startPos_ = {};
	//�ŏ��̒l���Z�b�g���邽�߂̃t���O(�؂�ւ��͈��L��)
	bool isSetStartPositionOnce_ = false;
	//���Z�b�g�|�W�V����
	const float resetPosY_ = 50.0f;

public: //�A�N�Z�b�T�A�C�����C���֐�
	
	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }
};