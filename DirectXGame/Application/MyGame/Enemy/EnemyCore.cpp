#include "EnemyCore.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;

/*

*	EnemyCore.cpp

*	ボス敵のコア

*/

//静的メンバ変数の実体
CollisionManager* EnemyCore::colManager_ = CollisionManager::GetInstance();

EnemyCore::~EnemyCore() {
	//パーティクルモデルの解放
	delete particleSmoke_;
	delete pmSmoke_;

	delete particleFire_;
	delete pmFire_;
}

std::unique_ptr<EnemyCore> EnemyCore::Create(Model* model, Model* bullet, Player* player, GamePlayScene* gamescene, [[maybe_unused]] unsigned short stage)
{
	//インスタンス生成
	std::unique_ptr<EnemyCore> ins = std::make_unique<EnemyCore>();
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
bool EnemyCore::Initialize() {

	if (!Object3d::Initialize()) return false;

	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);

	Parameter();

	//パーティクル
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_);

	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_);

	return true;
}

//パラメータ
void EnemyCore::Parameter() {
	phase_ = Phase::CoreStage1;
	const int32_t startLife = 5;
	life_ = startLife;
	isReverse_ = false;
	//弾初期値
	std::array<int, 2>RandomMinMax = { 50,100 };
	fireInterval_ = MyMath::RandomMTInt(RandomMinMax[0], RandomMinMax[1]);
	//発射タイマー初期化
	fireTimer_ = fireInterval_;

	isDead_ = false;


}

//リセット
void EnemyCore::Reset() { Parameter(); }

//更新
void EnemyCore::Update(bool isStart) {

	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//座標を移動させる
		switch (phase_) {
		case EnemyCore::Phase::CoreStage1:
			UpdateCore();
			break;

		case EnemyCore::Phase::CoreBreak:
			UpdateBreakCore();

			break;
		case EnemyCore::Phase::Leave:
			UpdateLeave();
			break;
		}
	}
	//行列更新
	Trans();
	camera_->Update();
	Object3d::Update();
	collider_->Update();

	//パーティクル更新
	pmFire_->Update();
	pmSmoke_->Update();
}

//転送
void EnemyCore::Trans() {

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
void EnemyCore::Fire() {
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
void EnemyCore::Draw() {

	//モデルの描画
	if (phase_!=Phase::Leave)Object3d::Draw();
}

void EnemyCore::DrawParticle()
{
	//パーティクル更新
	pmFire_->Draw();
	pmSmoke_->Draw();
}

void EnemyCore::UpdateCore()
{
	//速度
	float cameraMove = camera_->GetEye().x;

	//速度
	XMFLOAT3 velocity;
	//移動
	if (!isReverse_)velocity = { 0.3f, 0.0f, 0.0f };
	else velocity = { -0.3f, 0.0f, 0.0f };

	position_.x += velocity.x;
	position_.y += velocity.y;
	position_.z += velocity.z;

	//X軸が指定の位置に到達したら反転
	const float reversePosX = 65.0f;

	if (position_.x >= reversePosX) {
		//→から←
		isReverse_ = true;
	}
	if (position_.x <= -reversePosX) {
		isReverse_ = false;
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
	//死んだら自機の弾みたいに
	if (life_ <= deathLife_) {
		nowPos_ = Object3d::GetPosition();

		life_ = deathLife_;

		//ベジェ曲線の値
		const XMFLOAT3 startBezier3Pos = nowPos_;
		const XMFLOAT3 point1Bezier3Pos = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,40.0f,70.0f };
		const XMFLOAT3 point2Bezier3Pos = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,25.0f,85.0f };
		const XMFLOAT3 endBezier3Pos = { MyMath::RandomMTFloat(-15.0f,15.0f),10.0f,100.0f };
		//制御点
		start_ = startBezier3Pos;
		point1_ = point1Bezier3Pos;
		point2_ = point2Bezier3Pos;
		end_ = endBezier3Pos;

		startCount_ = std::chrono::steady_clock::now();	//開始時間
		//敵にぶつける

		phase_ = Phase::CoreBreak;
	}
}

void EnemyCore::UpdateBreakCore()
{
	if (position_.z >= point2_.z)
	{
		collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
		collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);
	}
	//時間
	//現在時間を取得する
	nowCount_ = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

	const float micro = 1'000'000.0f;//マイクロ秒
	float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//マイクロ秒を秒に単位変換
	
	const float timeRateMax = 1.0f;
	timeRate_ = min(elapsed / maxTime_, timeRateMax);

	position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);

}

//離脱
void EnemyCore::UpdateLeave() {

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	deathTimer_++;

	if (deathTimer_ >= DEATH_TIME)
	{
		isDead_ = true;
		bossDead_ = true;
	}
}

const XMFLOAT3 EnemyCore::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	const XMFLOAT3 ans =
	{
	(1.0f - t) * (1.0f - t) * (1.0f - t) * p0.x + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.x + 3.0f * (1.0f - t) * t * t * p2.x + t * t * t * p3.x,
	(1.0f - t) * (1.0f - t) * (1.0f - t) * p0.y + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.y + 3.0f * (1.0f - t) * t * t * p2.y + t * t * t * p3.y,
	(1.0f - t) * (1.0f - t) * (1.0f - t) * p0.z + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.z + 3.0f * (1.0f - t) * t * t * p2.z + t * t * t * p3.z
	};
	return ans;
}


//ワールド座標を取得
XMFLOAT3 EnemyCore::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void EnemyCore::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;

	//煙プリセット
	const ParticleManager::Preset smoke =
	{
		particleSmoke_,
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
		particleFire_,
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
			if (life_ > deathLife_)
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
	else if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			pmFire_->ActiveZ(fire.particle, fire.startPos, fire.pos, fire.vel,
				fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);

			pmFire_->Update();

			phase_ = Phase::Leave;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}
