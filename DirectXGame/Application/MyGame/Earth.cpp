#include "Earth.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* Earth::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<Earth> Earth::Create(Model* model)
{
	//�C���X�^���X����
	std::unique_ptr<Earth> ins = std::make_unique<Earth>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	return ins;
}

bool Earth::Initialize()
{
	if (!Object3d::Initialize()) return false;
	life_ = 5;
	isDead_ = false;
	isHit_ = false;
	mutekiCount = 0;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider->SetAttribute(COLLISION_ATTR_EARTH);
	collider->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	return true;
}

void Earth::Reset()
{
	life_ = 5;
	isDead_ = false;
	isHit_ = false;
	mutekiCount = 0;
}

void Earth::Update()
{
	if (!isDead_)
	{
		if (life_ <= 0)
		{
			isDead_ = true;
		}
		
		if (isHit_) mutekiCount++;
		if (mutekiCount == MUTEKI_COUNT)
		{
			isHit_ = false;
			mutekiCount = 0;
		}

	}
	rotation.y += 2.0f;
	camera_->Update();
	UpdateWorldMatrix();
	collider->Update();

	//�s��X�V��
	Object3d::Update();
}

void Earth::Trans()
{
	XMMATRIX world;
	//�s��X�V
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//����
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	Object3d::SetWorld(world);
}

XMFLOAT3 Earth::GetWorldPosition()
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void Earth::Draw()
{
	Object3d::Draw();
}

void Earth::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (isHit_)return;

		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			life_--;
			isHit_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)
		{
			life_--;
			 isHit_ = true;
		}
	}

}
