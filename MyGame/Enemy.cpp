#include "Enemy.h"
#include <cassert>
#include "MyGame.h"
#include "Player.h"

using namespace DirectX;

// 初期化
Enemy::~Enemy() {
	delete model_;
	delete modelBullet_;
	
}

void Enemy::Initialize(Model* model, Object3d* obj, Input* input) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	obj_ = obj;

	modelBullet_ = Model::LoadFromOBJ("enemybullet");
	objBullet_ = Object3d::Create();

	objBullet_->SetModel(modelBullet_);

	//シングルトンインスタンスを取得
	input_ = input;

	//ワールドトランスフォームの初期化

	isReverse_ = false;
	//初期ステージ
	Stage1Parameter();
}

//パラメータ
void Enemy::Stage1Parameter() {
	pos = { 0.0f,0.0f,600.0f };
	obj_->SetPosition(pos);
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

//エンディング用のポジション
void Enemy::EndingPosition() {
	worldTransform_.translation_ = { 10.0f, -10.0f, -10.0f };
	//回転速度
	const float kRotSpeed = -0.05f;
	worldTransform_.rotation_ = { 0.0f, kRotSpeed, 0.0f };
}
//更新
void Enemy::Update() {
	
	if (!isDead_) {
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

			//弾更新
			for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
				bullet->Update();
			}
		}
	}
	else {
		//座標を移動させる
		switch (phase_) {
		case Enemy::Phase::Leave:
			UpdateLeave();
			break;
		case Enemy::Phase::end:
			EndingPosition();
			break;
		}
	}
	//行列更新
	worldTransform_.Update(worldTransform_);
}

//弾発射
void Enemy::Fire() {

	assert(player_);

	//弾の速度
	const float kBulletSpeed = -1.0f;
	Vector3 velocity;

	//自機のワールド座標を取得
	player_->GetWorldPosition();
	//敵のワールド座標を取得
	GetWorldPosition();
	//敵→自機の差分ベクトルを求める
	velocity = player_->GetWorldPosition() -= GetWorldPosition();
	// ベクトルの正規化
	MyMathUtility::MyVector3Normalize(velocity);
	// ベクトルの長さを速さに合わせる
	velocity.z = kBulletSpeed;

	//弾を生成し初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(modelBullet_, worldTransform_.translation_, velocity);

	//弾を登録
	enemyBullets_.push_back(std::move(newBullet));
}

//描画
void Enemy::DrawStage1(const ViewProjection& viewProjection) {
	if (!isDead_) {
		//モデルの描画
		modelStage1_->Draw(worldTransform_, viewProjection);

		//弾描画
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			bullet->Draw(viewProjection);
		}
	}
	else {
		//モデルの描画
		modelDeadStage1_->Draw(worldTransform_, viewProjection);
	}
}

void Enemy::DrawStage2(const ViewProjection& viewProjection) {
	if (!isDead_) {
		//モデルの描画
		modelStage2_->Draw(worldTransform_, viewProjection);

		//弾描画
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			bullet->Draw(viewProjection);
		}
	}
	else {
		//モデルの描画
		modelDeadStage2_->Draw(worldTransform_, viewProjection);
	}
}

void Enemy::DrawStage3(const ViewProjection& viewProjection) {
	if (!isDead_) {
		//モデルの描画
		modelStage3_->Draw(worldTransform_, viewProjection);

		//弾描画
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			bullet->Draw(viewProjection);
		}
	}
	else {
		//モデルの描画
		modelSaveStage3_->Draw(worldTransform_, viewProjection);
	}
}

//状態変化用の更新関数
//接近
void Enemy::UpdateApproachStage1() {
	//速度
	Vector3 velocity;
	//移動
	velocity = { 0.0f, 0.0f, -0.1f };
	worldTransform_.translation_ += velocity;

	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = kFireIntervalStage1;
	}

	//指定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 50.0f) {
		phase_ = Phase::AttackStage1;
	}
}
void Enemy::UpdateApproachStage2() {
	//速度
	Vector3 velocity;
	//移動
	velocity = { 0.0f, 0.0f, -0.3f };
	worldTransform_.translation_ += velocity;

	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = kFireIntervalStage2;
	}
	if (worldTransform_.translation_.z < 80.0f) {
		phase_ = Phase::AttackStage2;
	}
}
void Enemy::UpdateApproachStage3() {
	//速度
	Vector3 velocity;
	//移動
	velocity = { 0.0f, 0.0f, -0.1f };
	worldTransform_.translation_ += velocity;

	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = kFireIntervalStage3;
	}
	if (worldTransform_.translation_.z < 70.0f) {
		phase_ = Phase::AttackStage3;
	}
}

//攻撃
void Enemy::UpdateAttackStage1() {

	//速度
	Vector3 velocity;
	//移動
	velocity = { 0.1f, 0.0f, 0.0f };
	if (isReverse_) {
		worldTransform_.translation_ -= velocity;
	}
	else {
		worldTransform_.translation_ += velocity;
	}

	//指定の位置に到達したら反転
	if (worldTransform_.translation_.x >= 30.0f) {
		isReverse_ = true;
	}
	if (worldTransform_.translation_.x <= -30.0f) {
		isReverse_ = false;
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
		phase_ = Phase::Leave;
		life_ = 0;
		isDead_ = true;
	}
}

void Enemy::UpdateAttackStage2() {

	//速度
	Vector3 velocity;
	//反転速度
	Vector3 velocityReverse;

	//移動
	velocity = { 0.2f, 0.2f, -0.03f };
	velocityReverse = { -0.2f, -0.2f, -0.03f };
	if (isReverse_) {
		worldTransform_.translation_ += velocityReverse;
	}
	else {
		worldTransform_.translation_ += velocity;
	}

	//指定の位置に到達したら反転
	if (worldTransform_.translation_.x >= 20.0f || worldTransform_.translation_.y >= 10.0f) {
		isReverse_ = true;
	}
	if (worldTransform_.translation_.x <= -20.0f || worldTransform_.translation_.y <= -10.0f) {
		isReverse_ = false;
	}

	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = kFireIntervalStage2;
	}
	//死んだら
	if (life_ <= 0) {
		phase_ = Phase::Leave;
		life_ = 0;
		isDead_ = true;
	}
}

void Enemy::UpdateAttackStage3() {

	//速度
	Vector3 velocity;
	//反転速度
	Vector3 velocityReverse;

	//移動
	velocity = { 0.2f, 0.2f, -0.02f };
	velocityReverse = { 0.2f, -0.2f, -0.02f };

	if (isReverse_) {
		worldTransform_.translation_ += velocityReverse;
	}
	else {
		worldTransform_.translation_ += velocity;
	}

	//指定の位置に到達したら反転
	if (worldTransform_.translation_.y >= 18.0f) {
		isReverse_ = true;
	}
	if (worldTransform_.translation_.y <= -18.0f) {
		isReverse_ = false;
	}

	if (worldTransform_.translation_.x >= 30.0f) {
		worldTransform_.translation_.x = -30.0f;
	}

	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = kFireIntervalStage3;
	}
	//死んだら
	if (life_ <= 0) {
		phase_ = Phase::SaveStage3;
		life_ = 0;
		isDead_ = true;
	}
	if (worldTransform_.translation_.z <= -10.0f) {
		phase_ = Phase::end;
	}
}
//離脱
void Enemy::UpdateLeave() {
	//速度
	Vector3 velocity;

	//移動
	velocity = { 0.0f, 0.0f, 0.03f };
	worldTransform_.translation_ += velocity;
}

//ステージ3限定離脱
void Enemy::UpdateSaveStage3() {
	//速度
	Vector3 velocity;

	//移動
	velocity = { 0.0f, 0.0f, -0.05f };
	worldTransform_.translation_ += velocity;
	if (input_->TriggerKey(DIK_SPACE)) {
		phase_ = Phase::end;
	}
}
//ワールド座標を取得
Vector3 Enemy::GetWorldPosition() {

	//ワールド座標を取得
	Vector3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}
//衝突を検出したら呼び出されるコールバック関数
void Enemy::OnCollisionPlayer() { life_ -= 2; }

void Enemy::OnCollisionOption() { life_--; }