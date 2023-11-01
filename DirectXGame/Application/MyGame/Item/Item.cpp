#include "Item.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "EnumList.h"

using namespace DirectX;

/*

*	Item.cpp

*	アイテム管理

*/

//静的メンバ変数の実体
CollisionManager* Item::colManager_ = CollisionManager::GetInstance();

Item::~Item()
{
	delete p_;
	delete pm_;

	delete spriteItemJumpBar_;
}

std::unique_ptr<Item> Item::Create(Model* model, Player* player, unsigned short subAttribute)
{
	//インスタンス生成
	std::unique_ptr<Item> ins = std::make_unique<Item>();

	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	if (player)ins->SetPlayer(player);
	if (subAttribute)ins->collider_->SetSubAttribute(subAttribute);

	return ins;
}

bool Item::Initialize()
{
	if (!Object3d::Initialize()) return false;
	//シングルトンインスタンス
	spCommon_ = SpriteCommon::GetInstance();

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider_->SetAttribute(COLLISION_ATTR_ITEM);
	
	isGet_ = false;
	isGetJump_ = false;

	//パーティクル
	p_ = Particle::LoadFromParticleTexture("particle6.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(p_);

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
	const float jumpPowerUp = 3.0f;
	const float jumpPowerReset = 2.0f;
	const XMFLOAT3 asIsColor = { 1.0f,1.0f,1.0f };//素材そのままの色
	if (isGetJump_)
	{
		ease_.ease_out_cubic();	
		if (player_->OnGround())player_->SetJumpVYFist(jumpPowerUp);
		spriteItemJumpBar_->SetColor({ asIsColor.x, asIsColor.y,asIsColor.z, ease_.num_X });
		count_++;
	}
	else
	{
		if (player_->OnGround())player_->SetJumpVYFist(jumpPowerReset);
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
	//行列更新
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//合成
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	Object3d::SetWorld(world);
}

XMFLOAT3 Item::GetWorldPosition()
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
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

void Item::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (isGet_)return;//多重ヒットを防止
	//プリセット
	const ParticleManager::Preset itemGet =
	{
		p_,
		position_,
		{ 8.0f ,8.0f,0.0f },
		{ 0.1f,4.0f,0.1f },
		{ 0.0f,0.001f,0.0f },
		30,
		{ 2.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
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
				const int healLife = player_->GetLife() + 1;
				player_->SetLife(healLife);
			}

			isGet_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}

void Item::LoadSprite()
{
	//アイテム関係
	spCommon_->LoadTexture(GPSITI_ItemJumpBar, "itemtex/itemjumpbar.png");
	spriteItemJumpBar_->Initialize(spCommon_, GPSITI_ItemJumpBar);
	spriteItemJumpBar_->SetPosition({ 0.0f,100.0f });

	spriteItemJumpBar_->Update();
}
