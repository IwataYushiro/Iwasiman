#include "PlayerBullet.h"
#include <cassert>

using namespace DirectX;

//初期化
void PlayerBullet::Initialize(Model* model, Object3d* obj, const XMFLOAT3& position, const XMFLOAT3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	obj_ = obj;
	
	//引数で受け取った初期座標をセット
	obj_->SetPosition(position);
	velocity_ = velocity;
}

void PlayerBullet::Reset() { isDead_ = true; }
//更新
void PlayerBullet::Update() {
	//座標を移動させる
	XMFLOAT3 pos = obj_->GetPosition();

	pos.x += velocity_.x;
	pos.y += velocity_.y;
	pos.z += velocity_.z;

	obj_->SetPosition(pos);
	//行列更新
	XMMATRIX world;

	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(obj_->GetScale().x, obj_->GetScale().y, obj_->GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(obj_->GetRotation().z)
		* XMMatrixRotationX(obj_->GetRotation().x) * XMMatrixRotationY(obj_->GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(obj_->GetPosition().x,
		obj_->GetPosition().y, obj_->GetPosition().z);

	//合成
	matWorld = matScale * matRot * matTrans;

	obj_->SetWorld(matWorld);
	obj_->Update();

	//時間経過で死亡
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

//描画
void PlayerBullet::Draw() {
	//モデルの描画
	obj_->Draw();
}

//衝突を検出したら呼び出されるコールバック関数
void PlayerBullet::OnCollision() { isDead_ = true; }

//ワールド座標を取得
XMFLOAT3 PlayerBullet::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = obj_->GetPosition().x;
	worldPos.y = obj_->GetPosition().y;
	worldPos.z = obj_->GetPosition().z;

	return worldPos;
}