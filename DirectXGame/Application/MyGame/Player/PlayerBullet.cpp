#include "PlayerBullet.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	PlayerBullet.cpp

*	自機の弾

*/

std::unique_ptr<PlayerBullet> PlayerBullet::Create(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model)
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
	//自機の弾
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
void PlayerBullet::Draw() {
	//モデルの描画
	Object3d::Draw();
}

//衝突を検出したら呼び出されるコールバック関数
void PlayerBullet::OnCollision([[maybe_unused]] const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute) {
	
	if (attribute == COLLISION_ATTR_LANDSHAPE)isDead_ = true;//地形の場合ぶつかったら消滅
	else if (attribute == COLLISION_ATTR_ENEMYS) //敵の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;			//敵の弾だと何も起こらない
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_ISDEAD)return;	//敵が死んでいる扱いでも何も起こらない
		else isDead_ = true;											//敵本体は消滅
	}
	
}
//ワールド座標を取得
const XMFLOAT3 PlayerBullet::GetWorldPosition() const{

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}