#include "EnemyBoss.h"
#include <cassert>
#include "SphereCollider.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	EnemyBoss.cpp

*	左右に動くボス敵

*/
//メンバ関数ポインタテーブルの実体
void (EnemyBoss::* EnemyBoss::updateTable_[])() =
{
	&EnemyBoss::UpdateApproach,
	&EnemyBoss::UpdateAttack,
	&EnemyBoss::UpdateLeave
};

EnemyBoss::~EnemyBoss() {

}

std::unique_ptr<EnemyBoss> EnemyBoss::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene, const bool isNotBossStage)
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
	if (model) ins->SetModel(model);		//本体
	if (bullet) ins->modelBullet_ = bullet;	//弾
	//自機のセット
	if (player)ins->SetPlayer(player);
	//ゲームシーンのセット
	if (gamescene)ins->SetGameScene(gamescene);
	//ボスステージじゃない場合カメラの動きと連動させる
	if (isNotBossStage)ins->collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_NOTSTAGEBOSS);
	else ins->collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	return ins;
}

// 初期化
bool EnemyBoss::Initialize() {

	if (!Object3d::Initialize()) return false;
	//入力情報を取得
	input_ = Input::GetInstance();

	//ベジェ曲線情報
	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//敵本体
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

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

//パラメータ
void EnemyBoss::Parameter() {

	//初期フェーズ
	phase_ = Phase::ApproachStage1;
	//ライフ初期値
	const int32_t startLife = 2;
	life_ = startLife;
	//反転したか
	isReverse_ = false;
	//弾初期値
	enum MinMax
	{
		MM_min = 0,		//最小値
		MM_max = 1,		//最大値
		MM_num = 2,		//配列用
	};
	//敵弾の発射間隔はランダム
	const std::array<int, MM_num>randomMinMax = { 40,75 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);
	//発射タイマー初期化
	fireTimer_ = fireInterval_;
	//死亡フラグ
	isDead_ = false;
	bossDead_ = false;

}

//リセット
void EnemyBoss::Reset() { Parameter(); }//各種パラメータだけ

//更新
void EnemyBoss::Update(const bool isStart) {

	//パーティクルマネージャーにカメラをセット
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
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
	//パーティクル更新
	pmFire_->Update();
	pmSmoke_->Update();
}
//弾発射
void EnemyBoss::Fire() {
	assert(player_);

	//弾の速度
	const float kBulletSpeed = 0.8f / life_;
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
	//各パーティクル描画
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//状態変化用の更新関数
//接近
void EnemyBoss::UpdateApproach() {


	//通常ステージの場合自機とシンクロ
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_NOTSTAGEBOSS)
	{
		UpdateSynchronizePlayerMove(true);
	}
	else UpdateSynchronizePlayerMove(false);//ボスステージの場合シンクロしない

	//発射タイマーカウントダウン
	fireTimer_--;
	//指定時間に達した
	if (fireTimer_ <= endFireTime_) {
		//弾発射
		Fire();
		//発射タイマー初期化
		const int32_t minInterval_ = fireInterval_ / 2;
		const int32_t maxInterval_ = fireInterval_;
		fireTimer_ = MyMath::RandomMTInt(minInterval_ * life_, maxInterval_ * life_);
	}
	//ここまで来たら攻撃フェーズへ
	const float moveAttackPhasePosZ = 100.0f;
	//指定の位置に到達したら攻撃
	if (position_.z < moveAttackPhasePosZ) {

		//ベジェ曲線の値
		const XMFLOAT3 startBezier3Pos = { position_.x ,10.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point1Bezier3Pos = { -5.0f,-20.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point2Bezier3Pos = { 5.0f ,40.0f,moveAttackPhasePosZ };
		const XMFLOAT3 endBezier3Pos = { 15.0f ,10.0f,moveAttackPhasePosZ };

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

	//通常ステージの場合自機とシンクロ
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_NOTSTAGEBOSS)
	{
		UpdateBezierMove(true);
	}
	else UpdateBezierMove(false);//ボスステージの場合シンクロしない
	

	//発射タイマーカウントダウン
	fireTimer_--;
	//指定時間に達した
	if (fireTimer_ <= endFireTime_) {
		//弾発射
		Fire();
		//発射タイマー初期化
		const int32_t minInterval_ = fireInterval_/2;
		const int32_t maxInterval_ = fireInterval_;
		fireTimer_ = MyMath::RandomMTInt(minInterval_, maxInterval_);
	}
	//死んだら死亡演出
	if (life_ <= deathLife_) {
		phase_ = Phase::Leave;
	}

}

//離脱
void EnemyBoss::UpdateLeave() {

	//サブ属性を死亡した扱いにする(死亡演出のため)
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	//一定の値までカウントが進んだら死亡する
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

void EnemyBoss::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
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

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	// 地形の場合何も起こらない
	else if (attribute == COLLISION_ATTR_PLAYERS)		// 自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;	   //自機本体に触れても何も起こらない
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)	   //自機の弾の場合
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

void EnemyBoss::UpdateBezierMove(const bool notStageBoss)
{
	if (notStageBoss)//通常ステージの場合は上下移動
	{
		//ダッシュ
		if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
		{
			//左はA、右はD
			if (input_->PushKey(DIK_A))position_.x -= player_->GetSpeedDash();
			else if (input_->PushKey(DIK_D))position_.x += player_->GetSpeedDash();
		}
		else//通常移動
		{
			//左はA、右はD
			if (input_->PushKey(DIK_A))position_.x -= player_->GetSpeedMove();
			else if (input_->PushKey(DIK_D))position_.x += player_->GetSpeedMove();
		}

		//速度
		XMFLOAT3 velocity;
		//登場速度
		const XMFLOAT3 approachSpeed = { 0.0f,0.5f,0.0f };
		//移動
		velocity = approachSpeed;
		position_.x += velocity.x;
		if(isReverse_)position_.y -= velocity.y;
		else position_.y += velocity.y;
		position_.z += velocity.z;
		//この位置に達したら反転(上下)
		const XMFLOAT2 reverseUpDownPosY = { 30.0f,0.0f };
		//指定の位置に到達したら反転
		if (position_.y >= reverseUpDownPosY.x) isReverse_ = true;	//上から下へ
		if (position_.y <= reverseUpDownPosY.y) isReverse_ = false;	//下から上へ
	}
	else//ベジェ曲線はボス戦用に
	{
		//ベジェ曲線の値
		const XMFLOAT3 startBezier3Pos = { -15.0f,10.0f,100.0f };
		const XMFLOAT3 point1Bezier3Pos = { -5.0f,-20.0f,100.0f };
		const XMFLOAT3 point2Bezier3Pos = { 5.0f,40.0f,100.0f };
		const XMFLOAT3 endBezier3Pos = { 15.0f,10.0f,100.0f };
		//時間
		//現在時間を取得する
		nowCount_ = std::chrono::steady_clock::now();
		//前回記録からの経過時間を取得する
		elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

		const float micro = 1'000'000.0f;//マイクロ秒
		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//マイクロ秒を秒に単位変換

		//0~1
		const float timeRateMax = 1.0f;
		timeRate_ = min(elapsed / maxTime_, timeRateMax);

		if (isReverse_)	//反転中
		{
			position_ = Bezier3(end_, point2_, point1_, start_, timeRate_);
		}
		else			//通常時
		{
			position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);
		}

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
	}
}

void EnemyBoss::UpdateSynchronizePlayerMove(const bool notStageBoss)
{
	//速度
	XMFLOAT3 velocity;

	//登場速度
	const XMFLOAT3 approachSpeed = { 0.0f,0.0f,-0.5f };
	//移動
	velocity = approachSpeed;
	if (notStageBoss)//通常ステージの場合
	{
		//ダッシュ
		if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
		{
			//左はA、右はD
			if (input_->PushKey(DIK_A))velocity.x -= player_->GetSpeedDash();
			else if (input_->PushKey(DIK_D))velocity.x += player_->GetSpeedDash();
		}
		else//通常移動
		{
			//左はA、右はD
			if (input_->PushKey(DIK_A))velocity.x -= player_->GetSpeedMove();
			else if (input_->PushKey(DIK_D))velocity.x += player_->GetSpeedMove();
		}
		position_.x += velocity.x;
		position_.y += velocity.y;
		position_.z += velocity.z;
	}
	else//ボスステージの場合シンクロしない
	{
		//移動
		position_.x += velocity.x;
		position_.y += velocity.y;
		position_.z += velocity.z;
	}
}
