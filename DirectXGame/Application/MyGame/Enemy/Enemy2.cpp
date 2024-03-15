#include "Enemy2.h"
#include <cassert>
#include "SphereCollider.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
#include "Vector2.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	Enemy2.cpp

*	昇っては落ちる雑魚敵

*/

//静的メンバ変数の実体
IwasiEngine::CollisionManager* Enemy2::colManager_ = CollisionManager::GetInstance();
//メンバ関数ポインタテーブルの実体
void (Enemy2::* Enemy2::updateTable_[])() =
{
	&Enemy2::UpdateApproach,
	&Enemy2::UpdateBack,
	&Enemy2::UpdateLeave
};

Enemy2::~Enemy2() {

}

std::unique_ptr<Enemy2> Enemy2::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene, int level)
{
	//インスタンス生成
	std::unique_ptr<Enemy2> ins = std::make_unique<Enemy2>();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize(level))
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);		//本体
	if (bullet) ins->modelBullet_ = bullet;	//弾
	//自機のセット
	if (player)ins->SetPlayer(player);
	//ゲームシーンのセット
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// 初期化
bool Enemy2::Initialize(const int level) {

	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//敵本体
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	//ここでサブ属性を初期化
	InitSubATTR(level);
	//各種パラメータ設定
	Parameter();

	//パーティクル
	//煙
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmSmoke_->SetParticleModel(particleSmoke_.get());
	//炎
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmFire_->SetParticleModel(particleFire_.get());

	return true;
}

void Enemy2::InitSubATTR(const int level)
{
	if (level == ET_Normal)collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);				//通常属性
	else if (level == ET_Power)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);	//攻撃属性
	else if (level == ET_Guard)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_GUARD);	//防御属性
	else if (level == ET_Speed)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_SPEED);	//速度属性
	else if (level == ET_Death)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);	//危険属性
}

void Enemy2::InitSpeed()
{
	//速度
	struct SpeedType
	{
		const XMFLOAT3 none = { 0.0f,-1.0f,0.0f };		  //通常属性
		const XMFLOAT3 power = { 0.0f,-0.5f,0.0f };		  //攻撃属性
		const XMFLOAT3 guard = { 0.0f,-1.0f,0.0f };		  //防御属性
		const XMFLOAT3 speed = { 0.0f,-2.0f,0.0f };		  //速度属性
		const XMFLOAT3 death = { 0.0f,-0.5f,0.0f };		  //危険属性
	};
	SpeedType speedType;
	//戻る速度
	struct BackSpeedType
	{
		const XMFLOAT3 none = { 0.0f,0.5f,0.0f };  		  //通常属性
		const XMFLOAT3 power = { 0.0f,0.25f,0.0f };		  //攻撃属性
		const XMFLOAT3 guard = { 0.0f,0.5f,0.0f }; 		  //防御属性
		const XMFLOAT3 speed = { 0.0f,1.0f,0.0f }; 		  //速度属性
		const XMFLOAT3 death = { 0.0f,0.25f,0.0f };		  //危険属性
	};
	BackSpeedType backSpeedType;
	//移動
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE)					//通常属性
	{
		speed_ = speedType.none;
		backSpeed_ = backSpeedType.none;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER)		//攻撃属性
	{
		speed_ = speedType.power;
		backSpeed_ = backSpeedType.power;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD)		//防御属性
	{
		speed_ = speedType.guard;
		backSpeed_ = backSpeedType.guard;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED)		//速度属性
	{
		speed_ = speedType.speed;
		backSpeed_ = backSpeedType.speed;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH)		//危険属性
	{
		speed_ = speedType.death;
		backSpeed_ = backSpeedType.death;
	}

}

void Enemy2::InitLife()
{
	struct LifeType
	{
		const int32_t none = 3;		//通常属性
		const int32_t power = 3;	//攻撃属性
		const int32_t guard = 5;	//防御属性
		const int32_t speed = 2;	//速度属性
		const int32_t death = 2;	//危険属性
	};
	LifeType lifeType;
	//ライフ
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) life_ = lifeType.none;				   //通常属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) life_ = lifeType.power;	   //攻撃属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) life_ = lifeType.guard;	   //防御属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) life_ = lifeType.speed;	   //速度属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) life_ = lifeType.death;	   //危険属性
}

//パラメータ
void Enemy2::Parameter() {

	//反転したか
	isReverse_ = false;
	//ジャンプしたか
	onGround_ = false;
	//初期フェーズ
	phase_ = Phase::Approach;
	//弾初期値
	enum MinMax
	{
		MM_min = 0,
		MM_max = 1,
		MM_num = 2,
	};
	//敵弾の発射間隔はランダム
	const std::array<int, MM_num>randomMinMax = { 75,100 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);
	//発射タイマー初期化
	fireTimer_ = fireInterval_;

	//移動
	InitSpeed();
	//ライフ
	InitLife();
	//色
	InitColor();
	//死亡フラグ
	isDead_ = false;


}

//リセット
void Enemy2::Reset() { Parameter(); }//各種パラメータだけ

//更新
void Enemy2::Update(const bool isStart) {

	//パーティクルマネージャーにカメラをセット
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);

	if (!isStart)//スタート演出時は何もしない
	{
		//座標を移動させる
		(this->*updateTable_[static_cast<size_t>(phase_)])();
		//死亡処理(Leaveが死亡演出)
		if (phase_ != Phase::Leave)Dead();

	}
	//座標の転送
	Trans();
	//更新
	camera_->Update();		//カメラ
	Object3d::Update();		//3Dオブジェクト
	collider_->Update();	//コライダー

	//着地処理
	Landing();
	//パーティクル更新
	pmFire_->Update();
	pmSmoke_->Update();
}
void Enemy2::Attack()
{
	//発射タイマーカウントダウン
	fireTimer_--;
	//指定時間に達した
	if (fireTimer_ <= endFireTime_) {
		//弾発射
		Fire();
		//発射タイマー初期化
		const int32_t minInterval = fireInterval_ / 2;
		const int32_t maxInterval = fireInterval_;
		fireTimer_ = MyMath::RandomMTInt(minInterval, maxInterval);
	}
}

//弾発射
void Enemy2::Fire() {
	assert(player_);

	//弾の速度
	const float kBulletSpeed = 0.3f;
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

void Enemy2::Dead()
{
	//ライフが0になったら
	if (life_ <= deathLife_) {

		life_ = deathLife_;//ライフをゼロに

		//最新の情報をセットして死亡演出の準備
		EaseDeadDirectionRotStart(false);	//回転
		EaseDeadDirectionScaleStart(false);	//スケール
		//フェーズ切り替え
		phase_ = Phase::Leave;
	}

}

void Enemy2::Landing()
{
	//球コライダーの取得
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	assert(sphereCollider);

	//専用クエリーコールバッククラス定義
	class EnemyQueryCallback : public QueryCallback
	{
	public:
		//コンストラクタ
		EnemyQueryCallback(Sphere* sphere) :sphere_(sphere) {};

		//衝突時のコールバック関数
		bool OnQueryHit(const QueryHit& info)
		{
			//ワールド上方向
			const XMVECTOR up = { 0.0f,1.0f,0.0f,0.0f };
			//排斥方向
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//上方向と排斥方向の角度差のコサイン値
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			//地面判定のしきい値角度
			const float threshold = cosf(XMConvertToRadians(30.0f));
			//角度差によって天井又は地面と判定される場合を除いて
			if (-threshold < cos && cos < threshold)
			{
				//球を排斥(押し出す)
				sphere_->center += info.reject;
				move += info.reject;
			}

			return true;
		}

	public:
		//球
		Sphere* sphere_ = nullptr;
		//排斥による移動量
		XMVECTOR move = {};

	};

	// 球クエリー、コライダー更新
	//クエリーコールバックの関数オブジェクト
	EnemyQueryCallback callback(sphereCollider);

	//球と地形の交差を全検索
	colManager_->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	//交差による排斥分動かす
	position_.x += callback.move.m128_f32[XYZ_X];
	position_.y += callback.move.m128_f32[XYZ_Y];
	//position_.z += callback.move.m128_f32[2];

	//視点と注視点をゲット
	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();
	//X分だけ動かす
	eyepos.x += callback.move.m128_f32[XYZ_X];
	tarpos.x += callback.move.m128_f32[XYZ_X];

	//更新
	UpdateWorldMatrix();			//行列更新
	camera_->SetEye(eyepos);		//視点セット
	camera_->SetTarget(tarpos);		//注視点セット
	collider_->Update();			//コライダー

	//球の上端から球の下端までのレイキャスト用レイを準備
	Ray ray;
	const XMVECTOR rayDir = { 0.0f,-1.0f,0.0f,0.0f };
	ray.start = sphereCollider->center;
	ray.start.m128_f32[XYZ_Y] += sphereCollider->GetRadius();
	ray.dir = rayDir;
	RaycastHit raycastHit;
	//半径　X　2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//接地状態
	if (onGround_)
	{
		//後退する
		if (count_ == MAX_GROUND)
		{
			const int8_t resetCount = 0;
			onGround_ = false;
			count_ = resetCount;
			upPos_ = Object3d::GetPosition();
			phase_ = Phase::Back;
		}

		count_++;
	}
	//落下状態
	else
	{
		//接地
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum))
		{
			onGround_ = true;
		}
		//初期位置に達したらまた落ちるように
		if (position_.y >= startPos_.y)
		{
			phase_ = Phase::Approach;
		}
	}
	//行列更新等
	Object3d::Update();
}

//描画
void Enemy2::Draw() {
	//モデルの描画
	Object3d::Draw();
}

void Enemy2::DrawParticle()
{
	//各パーティクル描画
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//状態変化用の更新関数
//下降
void Enemy2::UpdateApproach() {

	const float updatePositionX = 90.0f;
	if (player_->GetPosition().x - position_.x <= -updatePositionX)return;
	//移動
	if (!onGround_)
	{
		position_.x += speed_.x;
		position_.y += speed_.y;
		position_.z += speed_.z;
	}
	//攻撃処理
	Attack();
	//一定の位置まで達したら上へ
	if (position_.y <= backFallPosY)phase_ = Phase::Back;

}

//上昇
void Enemy2::UpdateBack()
{
	const float updatePositionX = 90.0f;
	if (player_->GetPosition().x - position_.x <= -updatePositionX)return;
	//移動
	position_.x += backSpeed_.x;
	position_.y += backSpeed_.y;
	position_.z += backSpeed_.z;
	//攻撃処理
	Attack();
	//初期位置まで達したら下へ
	if (position_.y >= startPos_.y) phase_ = Phase::Approach;
}

//離脱
void Enemy2::UpdateLeave() {

	//サブ属性を死亡した扱いにする(死亡演出のため)
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	//イージングをし転送
	EaseDeadDirectionRotStart(true);	//回転
	EaseDeadDirectionScaleStart(true);	//スケール
	//一定の値までカウントが進んだら死亡する
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)isDead_ = true;
}

void Enemy2::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;//死亡時は何も起こらない
	if (life_ <= 0) return;
	//現在ライフによる判定処理の基準となるライフ
	const int hitLife = 1;
	//煙プリセット
	const ParticleManager::Preset smoke =
	{
		particleSmoke_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		20,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f },
		20
	};
	//爆発プリセット
	const ParticleManager::Preset fire =
	{
		particleFire_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		40,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f },
		20
	};

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	  //地形の場合何も起こらない
	else if (attribute == COLLISION_ATTR_PLAYERS)		  //自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;		 //自機本体に触れても何も起こらない
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)		 //自機の弾の場合
		{
			//死んだらスキップ
			if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_ISDEAD) return;
			if (life_ > hitLife)//ライフが1より大きい場合
			{
				//パーティクルでヒット演出
				pmSmoke_->ActiveZ(smoke);

				pmSmoke_->Update();
				
			}
			else//1以下の場合
			{
				//カラー変更
				color_ = hitColor_;
				//パーティクルでヒット演出
				pmFire_->ActiveZ(fire);

				pmFire_->Update();
			}

			//ライフが減る
			life_--;
		}
	}

}
