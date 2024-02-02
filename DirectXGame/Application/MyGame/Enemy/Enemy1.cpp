#include "Enemy1.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"

using namespace DirectX;
using namespace IwasiEngine;

/*

*	Enemy1.cpp

*	そのまま歩き続ける敵

*/

//静的メンバ変数の実体
IwasiEngine::CollisionManager* Enemy1::colManager_ = CollisionManager::GetInstance();
//メンバ関数ポインタテーブルの実体
void (Enemy1::* Enemy1::updateTable_[])() =
{
	&Enemy1::UpdateApproach,
	&Enemy1::UpdateLeave
};

Enemy1::~Enemy1() {
	
}

std::unique_ptr<Enemy1> Enemy1::Create(const Model* model, const Model* bullet,
	const Player* player,GamePlayScene* gamescene, int level)
{
	//インスタンス生成
	std::unique_ptr<Enemy1> ins = std::make_unique<Enemy1>();
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
bool Enemy1::Initialize(const int level) {
	
	//初期化
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
void Enemy1::InitSubATTR(const int level)
{
	if (level == ET_Normal)collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);				//通常属性
	else if (level == ET_Power)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);	//攻撃属性
	else if (level == ET_Guard)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_GUARD);	//防御属性
	else if (level == ET_Speed)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_SPEED);	//速度属性
	else if (level == ET_Death)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);	//危険属性
}

void Enemy1::InitSpeed()
{
	//属性によって速度が違う
	struct SpeedType
	{
		const XMFLOAT3 none = { -0.2f,0.0f,0.0f }; 		  //通常属性
		const XMFLOAT3 power = { -0.1f,0.0f,0.0f };		  //攻撃属性
		const XMFLOAT3 guard = { -0.2f,0.0f,0.0f };		  //防御属性
		const XMFLOAT3 speed = { -0.4f,0.0f,0.0f };		  //速度属性
		const XMFLOAT3 death = { -0.2f,0.0f,0.0f };		  //危険属性
	};
	SpeedType speedType;
	
	//移動
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) speed_ = speedType.none;				//通常属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) speed_ = speedType.power;	//攻撃属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) speed_ = speedType.guard;	//防御属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) speed_ = speedType.speed;	//速度属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) speed_ = speedType.death;	//危険属性

}

void Enemy1::InitLife()
{
	//属性によってライフも違う
	struct LifeType
	{
		const int32_t none = 3;		  //通常属性
		const int32_t power = 3;	  //攻撃属性
		const int32_t guard = 5;	  //防御属性
		const int32_t speed = 2;	  //速度属性
		const int32_t death = 2;	  //危険属性
	};
	LifeType lifeType;
	//ライフ
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) life_ = lifeType.none;				  //通常属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) life_ = lifeType.power;	  //攻撃属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) life_ = lifeType.guard;	  //防御属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) life_ = lifeType.speed;	  //速度属性
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) life_ = lifeType.death;	  //危険属性
}

//パラメータ
void Enemy1::Parameter() {

	//反転したか
	isReverse_ = false;
	//ジャンプしたか
	onGround_ = true;
	//初期フェーズ
	phase_ = Phase::Approach;
	//弾初期値
	enum MinMax
	{
		MM_min = 0,		//最小値
		MM_max = 1,		//最大値
		MM_num = 2,		//配列用
	};
	//敵弾の発射間隔はランダム
	const std::array<int, MM_num>randomMinMax = { 100,150 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);
	//発射タイマー初期化
	fireTimer_ = fireInterval_;

	//移動
	InitSpeed();
	//ライフ
	InitLife();
	//死亡フラグ
	isDead_ = false;
	
}

//リセット
void Enemy1::Reset() { Parameter(); }//各種パラメータだけ

//更新
void Enemy1::Update(const bool isStart) {

	//パーティクルマネージャーにカメラをセット
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);

	if (!isStart)//スタート演出時は何もしない
	{
		//座標を移動させる
		(this->*updateTable_[static_cast<size_t>(phase_)])();
	}
	//座標を転送
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

//弾発射
void Enemy1::Fire() {
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
	newBullet = EnemyBullet::Create(pos, velocity,modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//弾を登録
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

void Enemy1::Landing()
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
			float cos = XMVector3Dot(rejectDir, up).m128_f32[XYZ_X];

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
	ray.start = sphereCollider->center;
	ray.start.m128_f32[XYZ_Y] += sphereCollider->GetRadius();
	const XMVECTOR rayDir = { 0.0f,-1.0f,0.0f,0.0f };
	ray.dir = rayDir;
	//レイキャスト
	RaycastHit raycastHit;
	//半径　X　2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//落下状態になるY値のスピード
	const float fallSpeedY = 0.0f;
	//接地状態
	if (onGround_)
	{
		//スムーズに坂を下るための吸着処理
		const float adsDistance = 0.2f;
		//接地を維持
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum + adsDistance))
		{
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
			//行列更新
			Object3d::Update();
		}
		//地面が無いので落下
		else
		{
			onGround_ = false;
			fallVec_ = {};
		}
	}
	//落下状態
	else if (fallVec_.y <= fallSpeedY)
	{
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum))
		{
			//着地
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
			//行列更新
			Object3d::Update();
		}
	}

	//行列更新等
	Object3d::Update();
}

//描画
void Enemy1::Draw() {

	//モデルの描画
	Object3d::Draw();


}

void Enemy1::DrawParticle()
{
	//各パーティクル描画
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//状態変化用の更新関数
//接近
void Enemy1::UpdateApproach() {
	//自機の距離ー敵の距離が一定の距離より小さかったら動くようにする
	const float updatePositionX = 90.0f;
	if (player_->GetPosition().x - position_.x <= -updatePositionX)return;

	//移動
	position_.x += speed_.x;
	position_.y += speed_.y;
	position_.z += speed_.z;

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

	if (!onGround_)//地面から離れたら落下
	{
		//下向き加速度
		const float fallAcc = -0.1f;
		const float fallVYMin = -2.0f;
		//加速
		fallVec_.y = max(fallVec_.y + fallAcc, fallVYMin);
		//移動
		position_.x += fallVec_.x;
		position_.y += fallVec_.y;
		position_.z += fallVec_.z;
	}

	//死んだら
	if (life_ <= deathLife_) {
		
		life_ = deathLife_;//ライフをゼロに

		//最新の情報をセットして死亡演出の準備
		EaseDeadDirectionRotStart(false);	//回転
		EaseDeadDirectionScaleStart(false);	//スケール
		
		//フェーズ切り替え
		phase_ = Phase::Leave;
		
	}
	//Y軸が一定座標に達しても死亡
	const float deathFallPosY = -60.0f;
	if (position_.y <= deathFallPosY)isDead_ = true;
}

//離脱
void Enemy1::UpdateLeave() {
	//サブ属性を死亡した扱いにする(死亡演出のため)
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	
	//イージングをし転送
	EaseDeadDirectionRotStart(true);	//回転
	EaseDeadDirectionScaleStart(true);	//スケール
	//一定の値までカウントが進んだら死亡する
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)isDead_ = true;
}
void Enemy1::OnCollision([[maybe_unused]]const CollisionInfo& info,
	const unsigned short attribute,const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;//死亡時は何も起こらない

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
		40,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	//地形の場合何も起こらない
	else if (attribute == COLLISION_ATTR_PLAYERS)		//自機の場合
	{
		if (phase_ == Phase::Leave) return;						//死亡時は何も起こらない
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;		//自機本体に触れても何も起こらない
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)		//自機の弾の場合
		{
			if (life_ > hitLife)//ライフが1より大きい場合
			{
				//パーティクルでヒット演出
				pmSmoke_->ActiveZ(smoke);

				pmSmoke_->Update();
				
			}
			else//1以下の場合
			{
				//パーティクルでヒット演出
				pmFire_->ActiveZ(fire);

				pmFire_->Update();
			}
			//ライフが減る
			life_--;
		}
			
	}

}
