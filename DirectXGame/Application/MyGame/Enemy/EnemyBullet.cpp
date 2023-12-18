#include "EnemyBullet.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;

/*

*	EnemyBullet.cpp

*	敵の弾

*/

std::unique_ptr<EnemyBullet> EnemyBullet::Create(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model)
{
	//インスタンス生成
	std::unique_ptr<EnemyBullet> ins = std::make_unique<EnemyBullet>();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize(position,velocity))
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	return ins;
}

//初期化
bool EnemyBullet::Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity) {
	if (!Object3d::Initialize()) return false;

	//引数で受け取った初期座標をセット
	SetPosition(position);
	velocity_ = velocity;

	//コライダー追加
	const XMVECTOR colliderOffset = { 0.0f,this->radius_,0.0f,0.0f };
	SetCollider(new SphereCollider(colliderOffset, this->radius_));
	//敵の弾
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);

	return true;
}

void EnemyBullet::Reset() { isDead_ = true; }

//更新
void EnemyBullet::Update() {
	//座標を移動させる
	XMFLOAT3 pos = Object3d::GetPosition();

	pos.x += velocity_.x;
	pos.y += velocity_.y;
	pos.z += velocity_.z;
	//座標のセット
	Object3d::SetPosition(pos);

	//行列更新
	XMMATRIX world;

	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//合成
	matWorld = matScale * matRot * matTrans;

	Object3d::SetWorld(matWorld);
	//更新
	camera_->Update();	//カメラ
	Object3d::Update();	//3Dオブジェクト
	
	//時間経過で死亡
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

//描画
void EnemyBullet::Draw() {
	//モデルの描画
	Object3d::Draw();
}

//衝突を検出したら呼び出されるコールバック関数
void EnemyBullet::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute) {
	
	if (attribute == COLLISION_ATTR_PLAYERS)//自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)isDead_ = true;	//自機本体に当たったら消滅
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;	//自機の弾だと何も起こらない
	}
}

//ワールド座標を取得
const XMFLOAT3 EnemyBullet::GetWorldPosition() const{

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}