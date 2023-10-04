#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class Player;
class CollisionManager;

//������ƒɂ��g�Q
class Spike :public BaseGimmick
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

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

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//�|�W�V����
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isSpike_ = false;

	float radius_ = 5.0f;

	Player* player_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsSpike() const { return isSpike_; }
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once
