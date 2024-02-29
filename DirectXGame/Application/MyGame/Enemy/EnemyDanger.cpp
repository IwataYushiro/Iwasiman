#include "EnemyDanger.h"
#include <cassert>
#include "SphereCollider.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;
using namespace IwasiEngine;
/*

*	EnemyDanger.cpp

*	危険な敵
*/

EnemyDanger::~EnemyDanger()
{
}

std::unique_ptr<EnemyDanger> EnemyDanger::Create(const Model* model,
	const Player* player, GamePlayScene* gamescene, [[maybe_unused]] unsigned short level)
{
	std::unique_ptr<EnemyDanger> ins = std::make_unique<EnemyDanger>();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	//自機のセット
	if (player)ins->SetPlayer(player);
	//ゲームシーンのセット
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

bool EnemyDanger::Initialize()
{
	//初期化
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//敵属性で能力は即死型
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);
	//各種パラメータ設定
	Parameter();
	//炎
	particleFire_ = Particle::LoadFromParticleTexture("kaze.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmFire_->SetParticleModel(particleFire_.get());
	return true;
}

void EnemyDanger::Reset()
{
	Parameter();//各種パラメータだけ
}

void EnemyDanger::Parameter()
{
	//ライフ初期値
	const int32_t startLife = 5;
	life_ = startLife;
	//反転したか
	isReverse_ = false;
	//死亡フラグ
	isDead_ = false;

}

void EnemyDanger::Update(const bool isStart)
{
	//パーティクルマネージャーにカメラをセット
	pmFire_->SetCamera(camera_);
	//常時パーティクル
	UpdateParticleSkin();

	if (!isStart)
	{
		//速度
		XMFLOAT3 velocity;

		const XMFLOAT3 velDefault = { 0.55f, 0.0f, 0.0f };//通常時スピード
		const XMFLOAT3 velReverse = { -0.55f, 0.0f, 0.0f };//反転時スピード

		//移動
		if (!isReverse_)velocity = velDefault;
		else velocity = velReverse;

		position_.x += velocity.x;
		position_.y += velocity.y;
		position_.z += velocity.z;

		//X軸が指定の位置に到達したら反転
		const float reversePosX = 200.0f;

		if (position_.x >= startPos_.x+reversePosX) {
			//→から←
			isReverse_ = true;
		}
		if (position_.x <= startPos_.x) {
			//←から→
			isReverse_ = false;
		}
		//常時回転
		const float rot = 7.0f;
		rotation_.y += rot;
	}
	//座標を転送
	Trans();
	//更新
	camera_->Update();		//カメラ
	Object3d::Update();		//3Dオブジェクト
	collider_->Update();	//コライダー
	pmFire_->Update();		//パーティクル
}

void EnemyDanger::Draw()
{
	//モデルの描画
	Object3d::Draw();
}

void EnemyDanger::DrawParticle()
{
	pmFire_->Draw();
}

void EnemyDanger::OnCollision([[maybe_unused]] const CollisionInfo& info, [[maybe_unused]] const unsigned short attribute, [[maybe_unused]] const unsigned short subAttribute)
{
}

void EnemyDanger::UpdateParticleSkin()
{
	//炎プリセット
	const ParticleManager::Preset fire =
	{
		particleFire_.get(),
		position_,
		{radius_ * 2.0f,radius_ * 2.0f,radius_ * 2.0f},
		{ -3.0f,0.2f,0.0f },
		{ 0.0f,0.001f,0.0f },
		MyMath::RandomMTInt(1,5),
		{MyMath::RandomMTFloat(3.0f,10.0f), 0.0f },
		{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.3f,0.5f),0.0f,1.0f },
		{ 1.0f,MyMath::RandomMTFloat(0.2f,0.5f),MyMath::RandomMTFloat(0.0f,0.2f),1.0f }

	};
	//マイナス座標用
	struct MinusPreset
	{
		XMFLOAT3 minusStartPos;			//初期座標
		XMFLOAT3 minusVel;			//速度
	};
	//座標マイナスプリセット
	const MinusPreset minus =
	{
		{position_.x-radius_,position_.y,position_.z},
		{-fire.vel.x,-fire.vel.y,-fire.vel.z}
	};

	//体を覆う炎エフェクト
	if (!isReverse_)//通常時
	{
		pmFire_->ActiveX(fire);
	}
	else//反転時
	{
		pmFire_->ActiveX(fire.particle, minus.minusStartPos, fire.pos, minus.minusVel,
			fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
	}
	//色合いも炎っぽく
	const XMFLOAT4 objFireColor = {0.0f,0.0f,MyMath::RandomMTFloat(0.2f,0.3f),1.0f };
	SetColor(objFireColor);
}
