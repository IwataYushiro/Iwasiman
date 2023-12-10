#include "Item.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "EnumList.h"
#include "MyMath.h"

using namespace DirectX;
using namespace MyMath;

/*

*	Item.cpp

*	�A�C�e���Ǘ�

*/

//�ÓI�����o�ϐ��̎���
CollisionManager* Item::colManager_ = CollisionManager::GetInstance();

Item::~Item()
{
}

std::unique_ptr<Item> Item::Create(const Model* model, const Player* player, const unsigned short subAttribute)
{
	//�C���X�^���X����
	std::unique_ptr<Item> ins = std::make_unique<Item>();

	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	if (player)ins->SetPlayer(player);
	if (subAttribute)ins->collider_->SetSubAttribute(subAttribute);

	return ins;
}

bool Item::Initialize()
{
	if (!Object3d::Initialize()) return false;
	//�V���O���g���C���X�^���X
	spCommon_ = SpriteCommon::GetInstance();

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider_->SetAttribute(COLLISION_ATTR_ITEM);

	isGet_ = false;
	isGetJump_ = false;

	//�p�[�e�B�N��
	p_ = Particle::LoadFromParticleTexture("particle3.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(p_.get());

	LoadSprite();

	return true;

}

void Item::Reset()
{
	isGet_ = false;
	isGetJump_ = false;
}

void Item::Update()
{
	pm_->SetCamera(camera_);
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP) UpdateJumpPowerup();
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_HEAL)
	{
		if (isGetHeal_)
		{
			collider_->SetSubAttribute(SUBCOLLISION_ATTR_ITEM_GET_ONCE);
			isGetHeal_ = false;
		}
	}
	const float rotSpeedY = 2.0f;
	rotation_.y += rotSpeedY;
	Trans();
	camera_->Update();
	pm_->Update();
	Object3d::Update();

	spriteItemJumpBar_->Update();
}

void Item::UpdateJumpPowerup()
{
	
	const XMFLOAT3 asIsColor = { 1.0f,1.0f,1.0f };//�f�ނ��̂܂܂̐F
	if (isGetJump_)
	{
		ease_.ease_out_cubic();
		spriteItemJumpBar_->SetColor({ asIsColor.x, asIsColor.y,asIsColor.z, ease_.num_X });
		count_++;
	}
	else
	{
		spriteItemJumpBar_->SetColor({ asIsColor.x, asIsColor.y, asIsColor.z,ease_.start });
	}

	if (count_ >= MAX_TIME)
	{
		const float countReset = 0.0f;
		count_ = countReset;
		isGet_ = false;
		isGetJump_ = false;

	}


}

void Item::Trans()
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

const XMFLOAT3 Item::GetWorldPosition()const
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void Item::Draw()
{
	if (!isGet_)Object3d::Draw();
}

void Item::DrawParticle()
{
	pm_->Draw();
}

void Item::DrawSprite()
{
	if (isGetJump_)spriteItemJumpBar_->Draw();
}

void Item::OnCollision([[maybe_unused]] const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)
{
	if (isGet_)return;//���d�q�b�g��h�~
	//�v���Z�b�g
	const ParticleManager::Preset itemGet =
	{
		p_.get(),
		position_,
		{ 8.0f ,8.0f,0.0f },
		{ 0.5f,4.0f,0.1f },
		{ 0.0f,0.001f,0.0f },
		30,
		{3.0f, 0.0f },
		{0.3f,1.0f,0.3f,1.0f},
		{0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			pm_->ActiveY(itemGet.particle, itemGet.startPos, itemGet.pos, itemGet.vel,
				itemGet.acc, itemGet.num, itemGet.scale, itemGet.startColor, itemGet.endColor);
			if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP)
			{
				ease_.Standby(false);
				isGetJump_ = true;
			}
			else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_HEAL)
			{
				isGetHeal_ = true;
			}

			isGet_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}

void Item::LoadSprite()
{
	//�A�C�e���֌W
	spCommon_->LoadTexture(GPSITI_ItemJumpBar, "itemtex/itemjumpbar.png");
	spriteItemJumpBar_->Initialize(spCommon_, GPSITI_ItemJumpBar);
	const XMFLOAT2 jumpBarPos = {800.0f,65.0f };
	spriteItemJumpBar_->SetPosition(jumpBarPos);

	spriteItemJumpBar_->Update();
}
