#include "EnemyCore.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;
using namespace IwasiEngine;
/*

*	EnemyCore.cpp

*	ボス敵のコア

*/
//メンバ関数ポインタテーブルの実体
void (EnemyCore::* EnemyCore::updateTable_[])() =
{
	&EnemyCore::UpdateCore,
	&EnemyCore::UpdateBreakCore,
	&EnemyCore::UpdateLeave
};

EnemyCore::~EnemyCore() {

}

std::unique_ptr<EnemyCore> EnemyCore::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene, [[maybe_unused]] unsigned short level)
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
	if (model) ins->SetModel(model);			//本体
	if (bullet) ins->modelBullet_ = bullet;		//弾
	//自機のセット
	if (player)ins->SetPlayer(player);
	//ゲームシーンのセット
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// 初期化
bool EnemyCore::Initialize() {

	//初期化
	if (!Object3d::Initialize()) return false;

	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//敵属性で能力は攻撃型
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);
	//各種パラメータ設定
	Parameter();

	//パーティクル
	//煙
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());

	//炎
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());

	return true;
}

//パラメータ
void EnemyCore::Parameter() {

	//フェーズ初期化
	phase_ = Phase::CoreStage1;
	//ライフ初期値
	const int32_t startLife = 5;
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
	const std::array<int, MM_num>randomMinMax = { 50,100 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);	//発射タイマー初期化

	//発射タイマー初期化
	fireTimer_ = fireInterval_;
	//死亡フラグ
	isDead_ = false;


}

//リセット
void EnemyCore::Reset() { Parameter(); }//各種パラメータだけ

//更新
void EnemyCore::Update(const bool isStart) {

	//パーティクルマネージャーにカメラをセット
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//座標を移動させる
		//switch (phase_) {
		//case EnemyCore::Phase::CoreStage1:	//行動時
		//	UpdateCore();
		//	break;

		//case EnemyCore::Phase::CoreBreak:	//撃破演出時
		//	UpdateBreakCore();

		//	break;
		//case EnemyCore::Phase::Leave:		//撃破時
		//	UpdateLeave();
		//	break;
		//}
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
	if (phase_ != Phase::Leave)Object3d::Draw();
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

	//速度計算用
	const float calcVelocity = hit_ * 0.2f;

	//速度
	XMFLOAT3 velocity;

	const XMFLOAT3 velDefault = { 0.3f + calcVelocity, 0.0f, 0.0f };//通常時スピード
	const XMFLOAT3 velReverse = { -0.3f - calcVelocity, 0.0f, 0.0f };//反転時スピード

	//移動
	if (!isReverse_)velocity = velDefault;
	else velocity = velReverse;

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
		//←から→
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
		//現在ポジションを記録(ベジェ曲線用)
		nowPos_ = Object3d::GetPosition();
		//ライフは0に
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
	//一定の位置まで達したら属性を自機の弾に切り替える
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
	//0~1
	const float timeRateMax = 1.0f;
	timeRate_ = min(elapsed / maxTime_, timeRateMax);
	//ベジェ曲線で飛んでいく
	position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);

}

//離脱
void EnemyCore::UpdateLeave() {
	//サブ属性を死亡した扱いにする(死亡演出のため)
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	//一定の値までカウントが進んだら死亡する
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)
	{
		isDead_ = true;
	}
}

const XMFLOAT3 EnemyCore::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
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

void EnemyCore::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;

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

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	//地形の場合何も起こらない
	else if (attribute == COLLISION_ATTR_PLAYERS)		//自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;		 //自機本体に触れても何も起こらない
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)		 //自機の弾の場合
		{
			if (life_ > deathLife_)//ライフが0じゃない場合
			{
				//パーティクルでヒット演出
				pmSmoke_->ActiveZ(smoke);

				pmSmoke_->Update();

			}
			else//ライフが0の場合
			{
				//パーティクルでヒット演出
				pmFire_->ActiveZ(fire);

				pmFire_->Update();

			}
			//ライフが減る
			life_--;
			//当たったカウント
			hit_++;
		}
	}
	else if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			pmFire_->ActiveZ(fire);

			pmFire_->Update();

			phase_ = Phase::Leave;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}
