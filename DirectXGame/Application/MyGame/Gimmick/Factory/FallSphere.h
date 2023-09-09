#pragma once

#include "BaseGimmick.h"

#include "Camera.h"
#include "Model.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class Player;
class CollisionManager;

class FallSphere :public BaseGimmick
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	static std::unique_ptr<FallSphere> Create(Model* model = nullptr, Player* player = nullptr, unsigned short subAttribute = 0b1000000000000001);
	//������
	bool Initialize()override;
	//�X�V
	void Update()override;
	//�X�V(��������)
	void UpdateFallSphere();
	//�߂�
	void UpdateFallSphereReturn();

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

	bool isFallSphere_ = false;

	float radius_ = 7.0f;

	Player* player_ = nullptr;

	//����true
	bool isRide = false;
	//����ė�����true
	bool isReturn = false;

	XMFLOAT3 startPos = {};

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsFallSphere() const { return isFallSphere_; }
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once