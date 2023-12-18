#include "Goal.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	Goal.cpp

*	ゴール

*/
std::unique_ptr<Goal> Goal::Create(const Model* model)
{
	//インスタンス生成
	std::unique_ptr<Goal> ins = std::make_unique<Goal>();
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

bool Goal::Initialize()
{
	//初期化
	if (!Object3d::Initialize()) return false;
	//ゴールしたか
	isGoal_ = false;
	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//ゴール本体
	collider_->SetAttribute(COLLISION_ATTR_GOAL);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	return true;
	
}

void Goal::Reset() { isGoal_ = false;/*ゴールしたかだけ*/ }

void Goal::Update()
{
	//ワールド座標を転送
	Trans();
	//更新
	camera_->Update();	//カメラ
	Object3d::Update();	//3Dオブジェクト
}

void Goal::Trans()
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

const XMFLOAT3 Goal::GetWorldPosition()const
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void Goal::Draw()
{
	Object3d::Draw();//モデルの描画
}

void Goal::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_PLAYERS)//自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isGoal_ = true;	//自機本体が触れたらゴール
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;	//自機の弾じゃ何も起こらない
	}
}
