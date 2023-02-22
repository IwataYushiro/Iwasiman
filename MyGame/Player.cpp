#include "Player.h"
#include <cassert>

using namespace DirectX;

bool Player::isDeads_ = false;

void Player::Initialize(Model* model, Object3d* obj, Input* input, XMFLOAT3 pos) {
	// NULLポインタチェック
	assert(model);

	//シングルトンインスタンスを取得
	input_ = input;

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	obj_ = obj;

	pos_ = pos;

	isDead_ = false;
	isDeads_ = false;
}

void Player::Update() {
	if (isDeads_) {
		isDead_ = true;
		return;
	}
	if (isDead_) {
		return;
	}

	XMFLOAT3 move = obj_->GetPosition();
	float moveSpeed = 0.4f;

	// キーボード入力による移動処理
	XMMATRIX matTrans = XMMatrixIdentity();
	if (input_->PushKey(DIK_A)) {
		move.x = -moveSpeed;
	}
	if (input_->PushKey(DIK_D)) {
		move.x = moveSpeed;
	}
	if (input_->PushKey(DIK_W)) {
		move.z = moveSpeed;
	}
	if (input_->PushKey(DIK_S)) {
		move.z = -moveSpeed;
	}

	obj_->SetPosition(move);
	//転送
	Trans();
}

void Player::Trans()
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

void Player::Draw() {
	if (!isDead_) {
		obj_->Draw();
	}
}

void Player::Reset() {

}

void Player::OnCollisionStage(bool collisionFlag) {
	if (collisionFlag) {
		pos_ = prePos_;
		Trans();
	}
	// 前フレーム座標
	prePos_ = pos_;
}

void Player::OnCollisionPlayer(bool collisionFlag) {
	if (collisionFlag) {
		isDeads_ = true;
	}
}

void Player::Move() {

}