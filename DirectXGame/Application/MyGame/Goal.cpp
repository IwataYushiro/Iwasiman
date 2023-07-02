#include "Goal.h"
#include <cassert>

using namespace DirectX;

void Goal::Initialize(Model* model, Object3d* obj, Camera* camera)
{
	// NULL�|�C���^�`�F�b�N
	assert(model);

	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	model_ = model;
	camera_ = camera;
	obj_ = obj;

	//���[���h�ϊ��̏�����
	pos = { 120.0f,-10.0f,0.0f };
	obj_->SetPosition(pos);
	scale = { 10.0f,10.0f,10.0f };
	obj_->SetScale(scale);
	
}

void Goal::Update()
{
	Trans();
	obj_->Update();
}

void Goal::Trans()
{
	XMMATRIX world;
	//�s��X�V
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(obj_->GetScale().x, obj_->GetScale().y, obj_->GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(obj_->GetRotation().z)
		* XMMatrixRotationX(obj_->GetRotation().x) * XMMatrixRotationY(obj_->GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(obj_->GetPosition().x,
		obj_->GetPosition().y, obj_->GetPosition().z);

	//����
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	obj_->SetWorld(world);
}

XMFLOAT3 Goal::GetWorldPosition()
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = obj_->GetPosition().x;
	worldPos.y = obj_->GetPosition().y;
	worldPos.z = obj_->GetPosition().z;

	return worldPos;
}

void Goal::Draw()
{
	obj_->Draw();
}

void Goal::OnCollision()
{
	isGoal_ = true;
}
