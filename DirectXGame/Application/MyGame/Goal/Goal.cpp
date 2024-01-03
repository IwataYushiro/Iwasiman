#include "Goal.h"
#include "SphereCollider.h"
#include "MyMath.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
using namespace MyMath;
/*

*	Goal.cpp

*	ゴール

*/
std::unique_ptr<Goal> Goal::Create(const Model* model, const bool isLandShape)
{
	//インスタンス生成
	std::unique_ptr<Goal> ins = std::make_unique<Goal>();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize(isLandShape))
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	return ins;
}

bool Goal::Initialize(const bool isLandShape)
{
	//初期化
	if (!Object3d::Initialize()) return false;
	//ゴールしたか
	isGoal_ = false;
	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//ゴール本体
	//地形化フラグがオンなら地形に
	if(isLandShape)collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	else collider_->SetAttribute(COLLISION_ATTR_GOAL);//オフならゴール扱い
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	
	//パーティクル
	particle_ = Particle::LoadFromParticleTexture("particle1.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(particle_.get());

	return true;
	
}

void Goal::Reset() { isGoal_ = false;/*ゴールしたかだけ*/ }

void Goal::Update()
{
	//パーティクルマネージャーにカメラをセット
	pm_->SetCamera(camera_);
	//パーティクルプリセット
	const ParticleManager::Preset goalEffect =
	{
		particle_.get(),
		position_,
		{ radius_*2.0f ,radius_ * 2.0f,radius_ * 2.0f },
		{ 0.1f,4.0f,0.1f },
		{ 0.0f,0.001f,0.0f },
		1,
		{3.0f, 0.0f },
		{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f},
		{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f}
	};
	//ゴールの位置を知らせるパーティクル
	pm_->ActiveY(goalEffect.particle, goalEffect.startPos, goalEffect.pos, goalEffect.vel,
		goalEffect.acc, goalEffect.num, goalEffect.scale, goalEffect.startColor, goalEffect.endColor);

	//ゴールは常時回っている
	const float rotSpeedY = 1.0f;
	rotation_.y += rotSpeedY;
	//ワールド座標を転送
	Trans();
	//更新
	camera_->Update();	//カメラ
	pm_->Update();		//パーティクル
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

void Goal::Draw()
{
	Object3d::Draw();//モデルの描画
}

void Goal::DrawParticle()
{
	pm_->Draw();//パーティクル描画
}

void Goal::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_PLAYERS)//自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isGoal_ = true;	//自機本体が触れたらゴール
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;	//自機の弾じゃ何も起こらない
	}
}
