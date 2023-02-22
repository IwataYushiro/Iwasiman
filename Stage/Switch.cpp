#include "Switch.h"

using namespace DirectX;

void Switch::Initialize(Model* model, Object3d* obj) {
	// モデル読み込み
	model_ = model;
	obj_ = obj;

	isFlag_ = false;
}

void Switch::Update() {
	isFlag_ = false;
}

void Switch::Trans()
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

void Switch::Draw() {
	obj_->Draw();
}

void Switch::OnCollisionSwitch() {
	isFlag_ = true;
}

void Switch::SetPosition(XMFLOAT3 pos)
{
	// スケール設定
	scale_ = { 2.0f, 2.0f, 2.0f };
	// 座標設定
	pos_ = pos;
	// 行列更新
	Trans();

}
