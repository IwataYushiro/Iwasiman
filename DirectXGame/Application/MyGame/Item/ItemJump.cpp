#include "ItemJump.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* ItemJump::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<ItemJump> ItemJump::Create(Model* model)
{
	//インスタンス生成
	std::unique_ptr<ItemJump> ins = std::make_unique<ItemJump>();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	return ins;
}

bool ItemJump::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider->SetAttribute(COLLISION_ATTR_ITEM);

	return true;

}

void ItemJump::Update()
{
	if (isGet_)	count--;
	
	if (count <= 0.0f)
	{
		isGet_ = false;
		count = 200.0f;
	}
	rotation.y += 2.0f;

	Trans();
	camera_->Update();
	Object3d::Update();
}

void ItemJump::Trans()
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

XMFLOAT3 ItemJump::GetWorldPosition()
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void ItemJump::Draw()
{
	if (!isGet_)Object3d::Draw();
}

void ItemJump::OnCollision(const CollisionInfo& info, unsigned short attribute)
{
	if (attribute == COLLISION_ATTR_ALLIES)isGet_ = true;

}
