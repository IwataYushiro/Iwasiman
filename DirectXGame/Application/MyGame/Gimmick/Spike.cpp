#include "Spike.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	Spike.cpp

*	当たると痛いトゲ

*/

std::unique_ptr<Spike> Spike::Create(const Model* model, const Player* player)
{
	//インスタンス生成
	std::unique_ptr<Spike> ins = std::make_unique<Spike>();

	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	//自機のセット
	if (player)ins->SetPlayer(player);
	return ins;
}

bool Spike::Initialize()
{
	//初期化
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//トゲのギミック
	collider_->SetAttribute(COLLISION_ATTR_GIMMICK);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_GIMMICK_SPIKE);


	return true;

}

void Spike::Update()
{
	//座標を転送
	Trans();
	//更新
	camera_->Update();	//カメラ
	Object3d::Update();	//3Dオブジェクト
}

void Spike::Trans()
{
	//ワールド座標
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
void Spike::Draw()
{
	Object3d::Draw();//トゲを描画
}

void Spike::OnCollision([[maybe_unused]] const CollisionInfo& info, [[maybe_unused]] const unsigned short attribute, [[maybe_unused]] const unsigned short subAttribute)
{
	
}