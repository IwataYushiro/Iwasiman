#include "FallSphere.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* FallSphere::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<FallSphere> FallSphere::Create(Model* model, Player* player, unsigned short subAttribute)
{
	//インスタンス生成
	std::unique_ptr<FallSphere> ins = std::make_unique<FallSphere>();

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
	if (subAttribute)ins->collider->SetSubAttribute(subAttribute);
	

	return ins;
}

bool FallSphere::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	
	return true;

}

void FallSphere::Update()
{
	
	if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE)UpdateRiseSphereReturn();
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN)UpdateFallSphereReturn();

	Trans();
	camera_->Update();
	Object3d::Update();
}

void FallSphere::UpdateFallSphere()
{
	if (isRide)
	{
		const float downSpeed = 0.1f;
		position.y -= downSpeed;

		if (position.y <= startPos.y - 50.0f)
		{
			position = startPos;
			isRide = false;
		}
	}
}

void FallSphere::UpdateFallSphereReturn()
{
	const float speed = 0.1f;
	
	if (isRide)
	{
		if (player_->OnGround())position.y -= speed;
		else
		{
			isRide = false;
			isReturn = true;
		}
	}
	else if (isReturn)
	{
		position.y += speed;
		if (position.y >= startPos.y)
		{
			position = startPos;
			isReturn = false;
		}
	}
	
}

void FallSphere::UpdateRiseSphere()
{
	if (isRide)
	{
		const float riseSpeed = 0.1f;
		position.y += riseSpeed;

		if (position.y >= startPos.y + 50.0f)
		{
			position = startPos;
			isRide = false;
		}
	}
}

void FallSphere::UpdateRiseSphereReturn()
{
	const float speed = 0.1f;

	if (isRide)
	{
		if (player_->OnGround())position.y += speed;
		else
		{
			isRide = false;
			isReturn = true;
		}
	}
	else if (isReturn)
	{
		position.y -= speed;
		if (position.y <= startPos.y)
		{
			position = startPos;
			isReturn = false;
		}
	}
}

void FallSphere::Trans()
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

XMFLOAT3 FallSphere::GetWorldPosition()
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void FallSphere::Draw()
{
	Object3d::Draw();
}

void FallSphere::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (isRide)return;

	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			if (!isReturn)startPos = position;
			isRide = true;
			isReturn = false;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;

	}
}