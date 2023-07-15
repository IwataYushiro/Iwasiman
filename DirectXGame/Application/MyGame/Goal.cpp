#include "Goal.h"
#include "SphereCollider.h"
#include <cassert>

using namespace DirectX;

Goal* Goal::Create(Model* model)
{
	//インスタンス生成
	Goal* ins = new Goal();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		delete ins;
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	return ins;
}

bool Goal::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//ワールド変換の初期化
	pos = { 120.0f,-10.0f,0.0f };
	Object3d::SetPosition(pos);
	scale = { 10.0f,10.0f,10.0f };
	Object3d::SetScale(scale);

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));

	return true;
	
}

void Goal::Update()
{
	Trans();
	camera_->Update();
	Object3d::Update();
}

void Goal::Trans()
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

XMFLOAT3 Goal::GetWorldPosition()
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
	Object3d::Draw();
}

void Goal::OnCollision(const CollisionInfo& info)
{
	//isGoal_ = true;
}
