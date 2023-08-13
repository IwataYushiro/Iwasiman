#pragma once
#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class Player;
class CollisionManager;

class ItemJump :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	static std::unique_ptr<ItemJump> Create(Model* model = nullptr, Player* player = nullptr);
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
	void OnCollision(const CollisionInfo& info, unsigned short attribute)override;

private:
	static CollisionManager* colManager_;
	//�|�W�V����
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	float count = 200.0f;

	bool isGet_ = false;

	float radius_ =3.0f;

	Player* player_ = nullptr;
public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsGet() const { return isGet_; }
	void SetPlayer(Player* player) { player_ = player; }
};
#pragma once
