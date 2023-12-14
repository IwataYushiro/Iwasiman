#pragma once

#include "BaseGimmick.h"

#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Spike.h

*	������ƒɂ��g�Q

*/
class Spike :public BaseGimmick
{
public:
	//����(�g�p���f���A�v���C���[)
	static std::unique_ptr<Spike> Create(const Model* model = nullptr, const Player* player = nullptr);
	//������
	bool Initialize()override;
	//�X�V
	void Update()override;
	//�]��
	void Trans();

	//���[���h���W���擾
	const XMFLOAT3 GetWorldPosition()const;

	//�`��
	void Draw()override;

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	
	//���a
	const float radius_ = 1.0f;

	//�v���C���[
	const Player* player_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	//�v���C���[�Z�b�g
	void SetPlayer(const Player* player) { player_ = player; }
};
#pragma once
