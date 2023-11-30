#include "FallAndRiseSphere.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;

/*

*	FallAndRiseSphere.cpp

*	落ちる球と昇る球

*/

//静的メンバ変数の実体
CollisionManager* FallAndRiseSphere::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<FallAndRiseSphere> FallAndRiseSphere::Create(const Model* model, const Player* player, unsigned short subAttribute)
{
	//インスタンス生成
	std::unique_ptr<FallAndRiseSphere> ins = std::make_unique<FallAndRiseSphere>();

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

bool FallAndRiseSphere::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	
	return true;

}

void FallAndRiseSphere::Update()
{
	
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE)UpdateFallSphereReturn();
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN)UpdateFallSphereReturn();
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_UPSPHERE)UpdateRiseSphere();

	Trans();
	camera_->Update();
	Object3d::Update();
}

void FallAndRiseSphere::UpdateFallSphere()
{
	if (isRide_)
	{
		const float downSpeed = 0.1f;
		position_.y -= downSpeed;

		if (position_.y <= startPos_.y - resetPosY_)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallAndRiseSphere::UpdateFallSphereReturn()
{
	const float speed = 0.1f;
	
	if (isRide_)
	{
		if (player_->OnGround())position_.y -= speed;
		else
		{
			isRide_ = false;
			isReturn_ = true;
		}
	}
	else if (isReturn_)
	{
		position_.y += speed;
		if (position_.y >= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
	
}

void FallAndRiseSphere::UpdateRiseSphere()
{
	if (isRide_)
	{
		const float Speed = 0.1f;
		position_.y += Speed;

		if (position_.y >= startPos_.y + resetPosY_)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallAndRiseSphere::UpdateRiseSphereReturn()
{
	const float speed = 0.1f;

	if (isRide_)
	{
		if (player_->OnGround())position_.y += speed;
		else
		{
			isRide_ = false;
			isReturn_ = true;
		}
	}
	else if (isReturn_)
	{
		position_.y -= speed;
		if (position_.y <= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
}

void FallAndRiseSphere::Trans()
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

XMFLOAT3 FallAndRiseSphere::GetWorldPosition()
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void FallAndRiseSphere::Draw()
{
	Object3d::Draw();
}

void FallAndRiseSphere::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (isRide_)return;

	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			if (!isReturn_)startPos_ = position_;
			isRide_ = true;
			
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;

	}
}