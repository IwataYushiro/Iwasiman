#include "PlayerBullet.h"
#include <cassert>
#include "MyMath.h"
#include "SphereCollider.h"
#include "CollisionAttribute.h"

using namespace DirectX;
using namespace IwasiEngine;
using namespace MyMath;
/*

*	PlayerBullet.cpp

*	自機の弾

*/

std::unique_ptr<PlayerBullet> PlayerBullet::Create(const XMFLOAT3& position, const XMFLOAT3& velocity,
	const Model* model, Particle* particle, ParticleManager* pm)
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
	//弾を撃つときの炎
	if (particle)ins->particleFire_ = particle;
	if (pm)ins->pmFire_ = pm;
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

	//パーティクルマネージャーにカメラをセット
	pmFire_->SetCamera(camera_);
	//座標を移動させる
	XMFLOAT3 pos = Object3d::GetPosition();

	pos.x += velocity_.x;
	pos.y += velocity_.y;
	pos.z += velocity_.z;

	
	//座標のセット
	Object3d::SetPosition(pos);
	//行列転送
	Trans();
	
	UpdateParticle();//パーティクル更新
	//更新
	camera_->Update();	//カメラ
	Object3d::Update();	//3Dオブジェクト
	pmFire_->Update();	//パーティクル
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

void PlayerBullet::DrawParticle()
{
	pmFire_->Draw();
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

void PlayerBullet::UpdateParticle()
{
	//パーティクル
	const ParticleManager::Preset fire =	//炎プリセット
	{
		particleFire_,
		position_,
		{ 0.05f ,2.0f,0.05f },
		{-velocity_.x,velocity_.y,velocity_.z},//弾の速度と同じ
		{ 0.0f,0.001f,0.0f },
		3,
		{ 2.0f, 0.0f },
		{0.0f,MyMath::RandomMTFloat(0.0f,0.2f),MyMath::RandomMTFloat(0.9f,1.0f),1.0f },
		{0.0f,0.0f,0.0f,1.0f}
	};
	
	pmFire_->ActiveX(fire);
}
