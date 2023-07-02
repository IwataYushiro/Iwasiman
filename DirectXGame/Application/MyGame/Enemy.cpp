#include "Enemy.h"
#include <cassert>

#include "Player.h"

using namespace DirectX;

Enemy::~Enemy() {
	//モデルの解放

	delete modelBullet_;
	delete objBullet_;
}

// 初期化
void Enemy::Initialize(Model* model, Object3d* obj,Camera* camera) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	camera_ = camera;
	obj_ = obj;

	modelBullet_ = Model::LoadFromOBJ("enemybullet");
	objBullet_ = Object3d::Create();

	objBullet_->SetModel(modelBullet_);
	objBullet_->SetCamera(camera_);
	Stage1Parameter();

	startCount= std::chrono::steady_clock::now();	//開始時間
	nowCount= std::chrono::steady_clock::now();		//現在時間
	elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	maxTime = 10.0f;					//全体時間
	timeRate;
}

//パラメータ
void Enemy::Stage1Parameter() {

	isReverse_ = false;
	//初期ステージ
	scale = { 3.0f,3.0f,3.0f };
	pos = { -30.0f,0.0f,100.0f };
	obj_->SetPosition(pos);
	obj_->SetScale(scale);
	//初期フェーズ
	phase_ = Phase::ApproachStage1;

	//発射タイマー初期化
	fireTimer = kFireIntervalStage1;

	life_ = 40;
	isDead_ = false;

	isReverse_ = false;
	//弾リセット
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Reset();
	}
}

//リセット
void Enemy::Reset() { Stage1Parameter(); }

//更新
void Enemy::Update() {


	//死亡フラグの立った弾を削除
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });

	//座標を移動させる
	switch (phase_) {
	case Enemy::Phase::ApproachStage1:

		UpdateApproachStage1();
		break;

	case Enemy::Phase::AttackStage1:

		UpdateAttackStage1();

		break;
	}
	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Update();
	}

	//座標を移動させる
	switch (phase_) {
	case Enemy::Phase::Leave:
		UpdateLeave();
		break;

	}

	//行列更新
	Trans();

	obj_->Update();
}

//転送
void Enemy::Trans() {

	XMMATRIX world;
	//行列更新
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(obj_->GetScale().x, obj_->GetScale().y, obj_->GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(obj_->GetRotation().z)
		* XMMatrixRotationX(obj_->GetRotation().x) * XMMatrixRotationY(obj_->GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(obj_->GetPosition().x,
		obj_->GetPosition().y, obj_->GetPosition().z);

	//合成
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	obj_->SetWorld(world);

}
//弾発射
void Enemy::Fire() {

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
	XMFLOAT3 position = obj_->GetPosition();

	//弾を生成し初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(modelBullet_, objBullet_, position, velocity);

	//弾を登録
	enemyBullets_.push_back(std::move(newBullet));
}

//描画
void Enemy::Draw() {
	if (!isDead_) {
		//モデルの描画
		obj_->Draw();

		//弾描画
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			bullet->Draw();
		}
	}

}


//状態変化用の更新関数
//接近
void Enemy::UpdateApproachStage1() {
	//速度
	XMFLOAT3 velocity;
	float cameraMove = camera_->GetEye().x;

	//移動
	velocity = { 0.0f, 0.0f, -0.5f };
	pos.x += velocity.x;
	pos.y += velocity.y;
	pos.z += velocity.z;

	obj_->SetPosition({ pos.x + cameraMove,pos.y,pos.z });
	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = kFireIntervalStage1;
	}

	//指定の位置に到達したら攻撃
	if (pos.z < 60.0f) {
		phase_ = Phase::AttackStage1;
	}
}
//攻撃
void Enemy::UpdateAttackStage1() {

	//速度
	XMFLOAT3 velocity;
	float cameraMove = camera_->GetEye().x;
	//制御点
	start = { -30.0f+cameraMove,0.0f,60.0f };
	p1 = { -10.0f+cameraMove,-30.0f,60.0f };
	p2 = { 10.0f+cameraMove,30.0f,60.0f };
	end = { 30.0f+cameraMove,0.0f,60.0f };
	//時間

	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

	float elapsed= std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count()/1'000'000.0f;//マイクロ秒を秒に単位変換

	timeRate = min(elapsed / maxTime, 1.0f);

	//移動
	velocity = { 0.5f, 0.0f, 0.0f };
	if (isReverse_) {
		pos = Bezier3(end, p2, p1, start, timeRate);
	}
	else {
		pos = Bezier3(start, p1, p2, end, timeRate);
	}
	obj_->SetPosition(pos);
	//指定の位置に到達したら反転
	if (pos.x >= 30.0f+cameraMove) {
		isReverse_ = true;
		startCount = std::chrono::steady_clock::now();
	}
	if (pos.x <= -30.0f+cameraMove) {
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
		fireTimer = kFireIntervalStage1;
	}
	//死んだら
	if (life_ <= 0) {
		isDead_ = true;
		life_ = 0;
	}
	if (isDead_)
	{
		phase_ = Phase::Leave;
	}
}

//離脱
void Enemy::UpdateLeave() {
	//速度
	XMFLOAT3 velocity;

	//移動
	velocity = { 0.0f, 0.0f, 0.03f };
	pos.x += velocity.x;
	pos.y += velocity.y;
	pos.z += velocity.z;
	obj_->SetPosition(pos);
}

const XMFLOAT3 Enemy::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	XMFLOAT3 ans;
	ans.x=(1.0f - t) * (1.0f - t) * (1.0f - t) * p0.x + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.x + 3 * (1.0f - t) * t * t * p2.x + t * t * t * p3.x;
	ans.y = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.y + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.y + 3 * (1.0f - t) * t * t * p2.y + t * t * t * p3.y;
	ans.z = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.z + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.z + 3 * (1.0f - t) * t * t * p2.z + t * t * t * p3.z;

	return ans;
}


//ワールド座標を取得
XMFLOAT3 Enemy::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = obj_->GetPosition().x;
	worldPos.y = obj_->GetPosition().y;
	worldPos.z = obj_->GetPosition().z;

	return worldPos;
}
//衝突を検出したら呼び出されるコールバック関数
void Enemy::OnCollisionPlayer() { life_--; }