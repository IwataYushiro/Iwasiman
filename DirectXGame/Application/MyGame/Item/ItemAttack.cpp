#include "ItemAttack.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* ItemAttack::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<ItemAttack> ItemAttack::Create(Model* model, Player* player)
{
	//インスタンス生成
	std::unique_ptr<ItemAttack> ins = std::make_unique<ItemAttack>();

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
	return ins;
}

bool ItemAttack::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider_->SetAttribute(COLLISION_ATTR_ITEM);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	return true;

}

void ItemAttack::Update()
{
	rotation_.y += 2.0f;

	Trans();
	camera_->Update();
	Object3d::Update();
}

void ItemAttack::Trans()
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

XMFLOAT3 ItemAttack::GetWorldPosition()
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void ItemAttack::Draw()
{
	if (!isGet_)Object3d::Draw();
}

void ItemAttack::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (isGet_)return;//多重ヒットを防止
	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isGet_ = true;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}
