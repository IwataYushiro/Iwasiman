#include "EnemyBoss.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"

using namespace DirectX;

/*

*	EnemyBoss.cpp

*	左右に動くボス敵

*/

//静的メンバ変数の実体
CollisionManager* EnemyBoss::colManager_ = CollisionManager::GetInstance();

EnemyBoss::~EnemyBoss() {
	
}

std::unique_ptr<EnemyBoss> EnemyBoss::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene)
{
	//インスタンス生成
	std::unique_ptr<EnemyBoss> ins = std::make_unique<EnemyBoss>();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	if (bullet) ins->modelBullet_ = bullet;
	if (player)ins->SetPlayer(player);
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// 初期化
bool EnemyBoss::Initialize() {

	if (!Object3d::Initialize()) return false;

	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	Parameter();

	//パーティクル
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmSmoke_->SetParticleModel(particleSmoke_.get());

	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmFire_->SetParticleModel(particleFire_.get());

	return true;
}

//パラメータ
void EnemyBoss::Parameter() {

	//初期フェーズ
	phase_ = Phase::ApproachStage1;

	const int32_t startLife = 2;
	life_ = startLife;

	isReverse_ = false;
	enum MinMax
	{
		MM_min = 0,
		MM_max=1,
		MM_num=2,
	};
	const std::array<int, MM_num>randomMinMax = { 40,75 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);

	//発射タイマー初期化
	fireTimer_ = fireInterval_;

	isDead_ = false;
	bossDead_ = false;

}

//リセット
void EnemyBoss::Reset() { }

//更新
void EnemyBoss::Update(const bool isStart) {

	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//座標を移動させる
		switch (phase_) {
		case EnemyBoss::Phase::ApproachStage1:

			UpdateApproach();
			break;

		case EnemyBoss::Phase::AttackStage1:

			UpdateAttack();

			break;

		case EnemyBoss::Phase::Leave:
			UpdateLeave();
			break;
		}
	}
	//行列更新
	Trans();
	camera_->Update();
	Object3d::Update();
	//パーティクル更新
	pmFire_->Update();
	pmSmoke_->Update();
}

//転送
void EnemyBoss::Trans() {

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
//弾発射
void EnemyBoss::Fire() {
	assert(player_);

	//弾の速度
	const float kBulletSpeed = 0.5f;
	XMFLOAT3 velocity = {};

	//自機のワールド座標を取得
	player_->GetWorldPosition();
	//敵のワールド座標を取得
	GetWorldPosition();
	//敵→自機の差分ベクトルを求める
	velocity.x = player_->GetWorldPosition().x - GetWorldPosition().x;
	velocity.y = player_->GetWorldPosition().y - GetWorldPosition().y;
	velocity.z = player_->GetWorldPosition().z - GetWorldPosition().z;
	// ベクトルの正規化
	float len;
	len = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
	if (len != 0)
	{
		velocity.x /= len;
		velocity.y /= len;
		velocity.z /= len;
	}

	// ベクトルの長さを速さに合わせる

	velocity.z -= kBulletSpeed;

	//座標をコピー
	XMFLOAT3 pos = GetPosition();

	//弾を生成し初期化
	std::unique_ptr<EnemyBullet> newBullet;
	newBullet = EnemyBullet::Create(pos, velocity, modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//弾を登録
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

//描画
void EnemyBoss::Draw() {
	//モデルの描画
	if (phase_ != Phase::Leave)Object3d::Draw();
}

void EnemyBoss::DrawParticle()
{
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//状態変化用の更新関数
//接近
void EnemyBoss::UpdateApproach() {
	//速度
	XMFLOAT3 velocity;

	const XMFLOAT3 approachSpeed = { 0.0f,0.0f,-0.5f };
	//移動
	velocity = approachSpeed;
	position_.x += velocity.x;
	position_.y += velocity.y;
	position_.z += velocity.z;

	//発射タイマーカウントダウン
	fireTimer_--;
	//指定時間に達した
	if (fireTimer_ <= endFireTime_) {
		//弾発射
		Fire();
		//発射タイマー初期化
		const int32_t minInterval_ = fireInterval_;
		const int32_t maxInterval_ = fireInterval_ * 2;
		fireTimer_ = MyMath::RandomMTInt(minInterval_, maxInterval_);
	}
	//ここまで来たら攻撃フェーズへ
	const float moveAttackPhasePosZ = 100.0f;
	//指定の位置に到達したら攻撃
	if (position_.z < moveAttackPhasePosZ) {

		//ベジェ曲線の値
		const XMFLOAT3 startBezier3Pos = { position_.x ,10.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point1Bezier3Pos = { -10.0f ,-20.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point2Bezier3Pos = { 10.0f ,40.0f,moveAttackPhasePosZ };
		const XMFLOAT3 endBezier3Pos = { 30.0f ,10.0f,moveAttackPhasePosZ };

		//制御点
		start_ = startBezier3Pos;
		point1_ = point1Bezier3Pos;
		point2_ = point2Bezier3Pos;
		end_ = endBezier3Pos;

		phase_ = Phase::AttackStage1;
	}
}
//攻撃
void EnemyBoss::UpdateAttack() {

	//時間
	//現在時間を取得する
	nowCount_ = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

	const float micro = 1'000'000.0f;//マイクロ秒
	float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//マイクロ秒を秒に単位変換

	const float timeRateMax = 1.0f;
	timeRate_ = min(elapsed / maxTime_, timeRateMax);

	if (isReverse_) {
		position_ = Bezier3(end_, point2_, point1_, start_, timeRate_);
	}
	else {
		position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);
	}

	//ベジェ曲線の値
	const XMFLOAT3 startBezier3Pos = { -30.0f ,10.0f,100.0f };
	const XMFLOAT3 point1Bezier3Pos = { -10.0f ,-20.0f,100.0f };
	const XMFLOAT3 point2Bezier3Pos = { 10.0f ,40.0f,100.0f };
	const XMFLOAT3 endBezier3Pos = { 30.0f ,10.0f,100.0f };
	//指定の位置に到達したら反転
	if (position_.x >= end_.x) {
		//制御点
		start_ = startBezier3Pos;
		point1_ = point1Bezier3Pos;
		point2_ = point2Bezier3Pos;
		end_ = endBezier3Pos;

		isReverse_ = true;
		startCount_ = std::chrono::steady_clock::now();
	}
	if (position_.x <= start_.x) {
		//制御点
		start_ = startBezier3Pos;
		point1_ = point1Bezier3Pos;
		point2_ = point2Bezier3Pos;
		end_ = endBezier3Pos;

		isReverse_ = false;
		startCount_ = std::chrono::steady_clock::now();
	}

	//発射タイマーカウントダウン
	fireTimer_--;
	//指定時間に達した
	if (fireTimer_ <= endFireTime_) {
		//弾発射
		Fire();
		//発射タイマー初期化
		const int32_t minInterval_ = fireInterval_;
		const int32_t maxInterval_ = fireInterval_ * 2;
		fireTimer_ = MyMath::RandomMTInt(minInterval_, maxInterval_);
	}
	//死んだら
	if (life_ <= deathLife_) {
		phase_ = Phase::Leave;
	}

}

//離脱
void EnemyBoss::UpdateLeave() {

	deathTimer_++;

	if (deathTimer_ >= DEATH_TIME)
	{
		isDead_ = true;
		bossDead_ = true;
	}
}

const XMFLOAT3 EnemyBoss::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	//三点ベジェ曲線の式
	//B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t)*t^2 * P2 + t^3 * P3 0<=t<=1
	
	XMFLOAT3 ans;
	ans.x = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.x + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.x + 3.0f * (1.0f - t) * t * t * p2.x + t * t * t * p3.x;
	ans.y = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.y + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.y + 3.0f * (1.0f - t) * t * t * p2.y + t * t * t * p3.y;
	ans.z = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.z + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.z + 3.0f * (1.0f - t) * t * t * p2.z + t * t * t * p3.z;
	
	return ans;
}


//ワールド座標を取得
const XMFLOAT3 EnemyBoss::GetWorldPosition() const{

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void EnemyBoss::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;
	const int hitLife = deathLife_ + 1;
	//煙プリセット
	const ParticleManager::Preset smoke =
	{
		particleSmoke_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		30,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};
	//爆発プリセット
	const ParticleManager::Preset fire =
	{
		particleFire_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		30,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)
		{
			if (life_ > hitLife)
			{

				pmSmoke_->ActiveZ(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
					smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

				pmSmoke_->Update();
				life_--;
			}
			else
			{

				pmFire_->ActiveZ(fire.particle, fire.startPos, fire.pos, fire.vel,
					fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);

				pmFire_->Update();
				life_--;
			}
		}

	}

}
