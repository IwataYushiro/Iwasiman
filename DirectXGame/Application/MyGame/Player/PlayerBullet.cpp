#include "PlayerBullet.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;

/*

*	PlayerBullet.cpp

*	自機の弾

*/

//静的メンバ変数の実体
CollisionManager* PlayerBullet::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<PlayerBullet> PlayerBullet::Create(const XMFLOAT3& position, const XMFLOAT3& velocity, Model* model)
{
	//インスタンス生成
	std::unique_ptr<PlayerBullet> ins = std::make_unique<PlayerBullet>();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize(position, velocity))
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	return ins;
}

//初期化
bool PlayerBullet::Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity)
{
	if (!Object3d::Initialize()) return false;

	//引数で受け取った初期座標をセット
	SetPosition(position);
	velocity_ = velocity;

	//コライダー追加
	const XMVECTOR colliderOffset = { 0.0f,radius_,0.0f,0.0f };
	SetCollider(new SphereCollider(colliderOffset, radius_));
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);

	return true;
}

void PlayerBullet::Reset() { isDead_ = true; }
//更新
void PlayerBullet::Update() {
	//座標を移動させる
	XMFLOAT3 pos = Object3d::GetPosition();

	pos.x += velocity_.x;
	pos.y += velocity_.y;
	pos.z += velocity_.z;

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
	camera_->Update();
	Object3d::Update();
	//時間経過で死亡
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

//描画
void PlayerBullet::Draw() {
	//モデルの描画
	Object3d::Draw();
}

//衝突を検出したら呼び出されるコールバック関数
void PlayerBullet::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute) {

	if (attribute == COLLISION_ATTR_LANDSHAPE)isDead_ = true;
	else if (attribute == COLLISION_ATTR_ENEMYS) 
	{
		if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
		else isDead_ = true;
	}
	
}
//ワールド座標を取得
XMFLOAT3 PlayerBullet::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}