#include "EnemyBoss.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;
CollisionManager* EnemyBoss::colManager_ = CollisionManager::GetInstance();

EnemyBoss::~EnemyBoss() {
	delete modelBullet_;
}

std::unique_ptr<EnemyBoss> EnemyBoss::Create(Model* model, Player* player, GamePlayScene* gamescene)
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
	if (player)ins->SetPlayer(player);
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// 初期化
bool EnemyBoss::Initialize() {

	if (!Object3d::Initialize()) return false;

	modelBullet_ = Model::LoadFromOBJ("enemybullet");

	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間
	elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	maxTime = 5.0f;					//全体時間
	timeRate;

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	Parameter();

	return true;
}

//パラメータ
void EnemyBoss::Parameter() {



	//初期フェーズ

	phase_ = Phase::ApproachStage1;
	maxTime = 5.0f;
	life_ = 200;


	isReverse_ = false;
	//発射タイマー初期化
	fireTimer = kFireInterval;

	isDead_ = false;
	bossDead = false;

}

//リセット
void EnemyBoss::Reset() { }

//更新
void EnemyBoss::Update() {


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

	//行列更新
	Trans();
	camera_->Update();
	Object3d::Update();
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
	XMFLOAT3 position = GetPosition();

	//弾を生成し初期化
	std::unique_ptr<EnemyBullet> newBullet;
	newBullet = EnemyBullet::Create(position, velocity, modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//弾を登録
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

//描画
void EnemyBoss::Draw() {

	//モデルの描画
	Object3d::Draw();
}


//状態変化用の更新関数
//接近
void EnemyBoss::UpdateApproach() {
	//速度
	XMFLOAT3 velocity;

	//移動
	velocity = { 0.0f, 0.0f, -0.5f };
	position.x += velocity.x;
	position.y += velocity.y;
	position.z += velocity.z;

	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = MyMath::RandomMTInt(kFireInterval, kFireInterval * 2);
	}

	//指定の位置に到達したら攻撃
	if (position.z < 100.0f) {
		phase_ = Phase::AttackStage1;
	}
}
//攻撃
void EnemyBoss::UpdateAttack() {

	//速度
	float cameraMove = camera_->GetEye().x;
	//制御点
	start = { -30.0f + cameraMove,10.0f,100.0f };
	p1 = { -10.0f + cameraMove,-20.0f,100.0f };
	p2 = { 10.0f + cameraMove,40.0f,100.0f };
	end = { 30.0f + cameraMove,10.0f,100.0f };
	//時間

	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

	float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	timeRate = min(elapsed / maxTime, 1.0f);

	if (isReverse_) {
		position = Bezier3(end, p2, p1, start, timeRate);
	}
	else {
		position = Bezier3(start, p1, p2, end, timeRate);
	}
	//指定の位置に到達したら反転
	if (position.x >= 30.0f + cameraMove) {
		isReverse_ = true;
		startCount = std::chrono::steady_clock::now();
	}
	if (position.x <= -30.0f + cameraMove) {
		isReverse_ = false;
		startCount = std::chrono::steady_clock::now();
	}

	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = MyMath::RandomMTInt(kFireInterval, kFireInterval * 2);
	}
	//死んだら
	if (life_ <= 0) {
		life_ = 0;
		isDead_ = true;
		bossDead = true;
	}

}

//離脱
void EnemyBoss::UpdateLeave() {

	isDead_ = true;
	bossDead = true;
}

const XMFLOAT3 EnemyBoss::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	XMFLOAT3 ans;
	ans.x = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.x + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.x + 3 * (1.0f - t) * t * t * p2.x + t * t * t * p3.x;
	ans.y = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.y + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.y + 3 * (1.0f - t) * t * t * p2.y + t * t * t * p3.y;
	ans.z = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.z + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.z + 3 * (1.0f - t) * t * t * p2.z + t * t * t * p3.z;

	return ans;
}


//ワールド座標を取得
XMFLOAT3 EnemyBoss::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void EnemyBoss::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;

	}

}
