#pragma once
#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

class CollisionManager;

class Earth :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	const int MUTEKI_COUNT = 60;
public:

	static std::unique_ptr<Earth> Create(Model* model = nullptr);
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
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isHit_ = false;
	bool isDead_ = false;
	int mutekiCount = 0;
	int life_ = 3;
	
	float radius_ = 50.0f;
public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }
	//���C�t
	void SetLife(int life) { this->life_ = life; }
	const int& GetLife()const { return life_; }
};
#pragma once
