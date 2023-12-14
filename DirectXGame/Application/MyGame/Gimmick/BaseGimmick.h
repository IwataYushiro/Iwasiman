#pragma once
#include "Object3d.h"
#include "Camera.h"
#include "Model.h"

//�O���錾
//���@�N���X
class Player;
//�R���W�����}�l�[�W���[
class CollisionManager;

/*

*	BaseGimmick.h (cpp X)

*	�M�~�b�N��ՃN���X

*/
class BaseGimmick :public IwasiEngine::Object3d
{
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::���ȗ�
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using CollisionInfo = IwasiEngine::CollisionInfo;

public:
	//�f�X�g���N�^
	virtual ~BaseGimmick() = default;

	//������
	virtual bool Initialize() { return Object3d::Initialize(); }
	//virtual bool Initialize([[maybe_unused]] Model* model) { return Object3d::Initialize(); }
	//�X�V
	virtual void Update() = 0; //{ Object3d::Update(); }
	//�`��
	virtual void Draw() = 0; //{ Object3d::Draw(); }

};

