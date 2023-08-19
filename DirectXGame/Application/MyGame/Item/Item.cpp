#include "Item.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* Item::colManager_ = CollisionManager::GetInstance();

Item::~Item()
{
	delete p;
	delete pm_;

	delete spriteItemJumpBar_;
}

std::unique_ptr<Item> Item::Create(Model* model, Player* player, unsigned short subAttribute)
{
	//インスタンス生成
	std::unique_ptr<Item> ins = std::make_unique<Item>();

	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize(subAttribute))
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	if (player)ins->SetPlayer(player);

	return ins;
}

bool Item::Initialize(unsigned short subAttribute)
{
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider->SetAttribute(COLLISION_ATTR_ITEM);
	collider->SetSubAttribute(subAttribute);

	isGet_ = false;
	isGetJump_ = false;

	//パーティクル
	p = Particle::LoadFromParticleTexture("particle6.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(p);

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
	if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP) UpdateJumpPowerup();


	rotation.y += 2.0f;
	Trans();
	camera_->Update();
	pm_->Update();
	Object3d::Update();
	
	spriteItemJumpBar_->Update();
}

void Item::UpdateJumpPowerup()
{
	if (isGetJump_)
	{
		ease.ease_out_cubic();
		if (player_->OnGround())player_->SetJumpVYFist(4.0f);
		spriteItemJumpBar_->SetColor({ 1.0f, 1.0f,1.0f, ease.num_X });
		count++;
	}
	else
	{
		if (player_->OnGround())player_->SetJumpVYFist(2.0f);
		spriteItemJumpBar_->SetColor({ 1.0f, 1.0f, 1.0f,ease.start });
	}

	if (count >= MAX_TIME)
	{
		count = 0.0f;
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

void Item::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (isGet_)return;//多重ヒットを防止

	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			
			if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP)
			{
				ease.Standby(false);
				isGetJump_ = true;
			}
			else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_HEAL)
			{
				pm_->ActiveY(p, position, { 8.0f ,8.0f,0.0f }, { 0.1f,4.0f,0.1f }, { 0.0f,0.001f,0.0f }, 30, { 2.0f, 0.0f });

				player_->SetLife(player_->GetLife() + 1);

			}
			isGet_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}

void Item::LoadSprite()
{
	//アイテム関係
	spCommon_->LoadTexture(1000, "itemtex/itemjumpbar.png");
	spriteItemJumpBar_->Initialize(spCommon_, 1000);
	spriteItemJumpBar_->SetPosition({ 0.0f,100.0f });

	spriteItemJumpBar_->Update();
}
