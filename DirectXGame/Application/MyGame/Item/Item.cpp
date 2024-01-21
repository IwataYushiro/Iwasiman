#include "Item.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "EnumList.h"
#include "MyMath.h"

using namespace DirectX;
using namespace IwasiEngine;
using namespace MyMath;

/*

*	Item.cpp

*	�A�C�e���Ǘ�

*/

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
	//���@�̃Z�b�g
	if (player)ins->SetPlayer(player);
	if (subAttribute)ins->collider_->SetSubAttribute(subAttribute);//�ǂ̃A�C�e�����͈����Ō��߂�

	return ins;
}

bool Item::Initialize()
{
	//������
	if (!Object3d::Initialize()) return false;
	//�V���O���g���C���X�^���X
	spCommon_ = SpriteCommon::GetInstance();

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//�A�C�e��
	collider_->SetAttribute(COLLISION_ATTR_ITEM);

	//�擾������
	isGet_ = false;
	isGetJump_ = false;

	//�p�[�e�B�N��
	p_ = Particle::LoadFromParticleTexture("particle3.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(p_.get());
	//�X�v���C�g
	LoadSprite();

	return true;

}

void Item::Reset()
{
	//�擾�������ǂ�������
	isGet_ = false;
	isGetJump_ = false;
}

void Item::Update()
{
	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pm_->SetCamera(camera_);
	//�����ɂ���Č��ʂ�ς���
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP) UpdateJumpPowerup();//�W�����v�͋����A�C�e���̏ꍇ
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_HEAL)//���C�t�񕜃A�C�e���̏ꍇ
	{
		//�擾������
		if (isGetHeal_)
		{
			//�������擾�ς݃A�C�e���ɂ��Ď��Ȃ��悤�ɐݒ�
			collider_->SetSubAttribute(SUBCOLLISION_ATTR_ITEM_GET_ONCE);
			isGetHeal_ = false;
		}
	}
	//�A�C�e���͏펞����Ă���
	const float rotSpeedY = 2.0f;
	rotation_.y += rotSpeedY;
	//���W��]��
	Trans();
	//�X�V
	camera_->Update();		//�J����
	pm_->Update();			//�p�[�e�B�N���}�l�[�W���[
	Object3d::Update();		//3D�I�u�W�F�N�g
	
	//�W�����v�o�[�X�V
	spriteItemJumpBar_->Update();
}

void Item::UpdateJumpPowerup()
{
	
	if (isGetJump_)//�擾������
	{
		//�C�[�W���O�ŏ������T�C�Y������������
		ease_.ease_in_out_sine();
		spriteItemJumpBar_->SetTextureSize({ ease_.num_X, spriteItemJumpBar_->GetSize().y});
		spriteItemJumpBar_->SetSize({ ease_.num_X, spriteItemJumpBar_->GetSize().y });
		//���ʎ��Ԃ�i�߂�
		count_++;
	}
	else
	{
		//�A���t�@�l��߂�
		spriteItemJumpBar_->SetTextureSize({ ease_.start, spriteItemJumpBar_->GetSize().y });
		spriteItemJumpBar_->SetSize({ ease_.start, spriteItemJumpBar_->GetSize().y });
	}
	//���ʎ��Ԃ����̎��ԂɒB�����狭������
	if (count_ >= MAX_TIME)
	{
		const float countReset = 0.0f;
		count_ = countReset;
		isGet_ = false;
		isGetJump_ = false;

	}


}

void Item::Draw()
{
	if (!isGet_)Object3d::Draw();//�Q�b�g�����������
}

void Item::DrawParticle()
{
	pm_->Draw();//�p�[�e�B�N���`��
}

void Item::DrawSprite()
{
	if (isGetJump_)spriteItemJumpBar_->Draw();//�W�����v�A�C�e�����Q�b�g������\�������
}

void Item::OnCollision([[maybe_unused]] const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)
{
	if (isGet_)return;//���d�q�b�g��h�~
	//�p�[�e�B�N���v���Z�b�g
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

	if (attribute == COLLISION_ATTR_PLAYERS)//���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)//���@�{�̂��G�ꂽ����ʔ���
		{
			//�Q�b�g�ł������Ƃ�m�点��p�[�e�B�N��
			pm_->ActiveY(itemGet);
			//�A�C�e�����ƂɌ��ʂ͈Ⴄ
			if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP)//�W�����v�͋����A�C�e���̏ꍇ
			{
				//�C�[�W���O���X�^���o�C
				ease_.Standby(false);
				//�Q�b�g
				isGetJump_ = true;
			}
			else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_HEAL)//���C�t�񕜃A�C�e���̏ꍇ
			{
				//�Q�b�g
				isGetHeal_ = true;
			}
			//�A�C�e�����Q�b�g�������Ƃ�`����t���O
			isGet_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;//�e���G��Ă������N����Ȃ�
	}
}

void Item::LoadSprite()
{
	//�A�C�e���֌W�̃X�v���C�g
	//�W�����v�o�[
	spCommon_->LoadTexture(GPSITI_ItemJumpBar, "itemtex/itemjumpbar.png");
	spriteItemJumpBar_->Initialize(spCommon_, GPSITI_ItemJumpBar);
	const XMFLOAT2 jumpBarPos = {50.0f,660.0f };
	spriteItemJumpBar_->SetPosition(jumpBarPos);
	const Vector2 settingEase_ = {spriteItemJumpBar_->GetSize().x, 0.0f};//�C�[�W���O�̃X�^�[�g�A�G���h�n�_
	ease_.SetEasing(settingEase_.x, settingEase_.y, timer_);//�C�[�W���O�̃Z�b�g

	spriteItemJumpBar_->Update();
}
