#include "Goal.h"
#include <cassert>

using namespace DirectX;

void Goal::Initialize(Model* model, Object3d* obj, Camera* camera)
{
	// NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;
	obj_ = obj;

	//ワールド変換の初期化
	pos = { 120.0f,-10.0f,0.0f };
	obj_->SetPosition(pos);
	scale = { 10.0f,10.0f,10.0f };
	obj_->SetScale(scale);
	
}

void Goal::Update()
{
	Trans();
	obj_->Update();
}

void Goal::Trans()
{
	XMMATRIX world;
	//行列更新
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(obj_->GetScale().x, obj_->GetScale().y, obj_->GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(obj_->GetRotation().z)
		* XMMatrixRotationX(obj_->GetRotation().x) * XMMatrixRotationY(obj_->GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(obj_->GetPosition().x,
		obj_->GetPosition().y, obj_->GetPosition().z);

	//合成
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	obj_->SetWorld(world);
}

XMFLOAT3 Goal::GetWorldPosition()
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = obj_->GetPosition().x;
	worldPos.y = obj_->GetPosition().y;
	worldPos.z = obj_->GetPosition().z;

	return worldPos;
}

void Goal::Draw()
{
	obj_->Draw();
}

void Goal::OnCollision()
{
	isGoal_ = true;
}
