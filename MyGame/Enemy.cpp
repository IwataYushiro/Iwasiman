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
void Enemy::Initialize(Model* model, Object3d* obj) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	obj_ = obj;

	modelBullet_ = Model::LoadFromOBJ("enemybullet");
	objBullet_ = Object3d::Create();

	objBullet_->SetModel(modelBullet_);
	Stage1Parameter();
}

//パラメータ
void Enemy::Stage1Parameter() {

	isReverse_ = false;
	//初期ステージ
	scale = { 3.0f,3.0f,3.0f };
	pos = { 0.0f,0.0f,100.0f };
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
		}
		//弾更新
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			bullet->Update();
		}
	}
	else {
		//座標を移動させる
		switch (phase_) {
		case Enemy::Phase::Leave:
			UpdateLeave();
			break;
		
		}
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
	newBullet->Initialize(modelBullet_,objBullet_, position, velocity);

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
	//移動
	velocity = { 0.0f, 0.0f, -0.3f };
	pos.x += velocity.x;
	pos.y += velocity.y;
	pos.z += velocity.z;

	obj_->SetPosition(pos);
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
	if (pos.z < 30.0f) {
		phase_ = Phase::AttackStage1;
	}
}
//攻撃
void Enemy::UpdateAttackStage1() {

	//速度
	XMFLOAT3 velocity;
	//移動
	velocity = { 0.3f, 0.0f, 0.0f };
	if (isReverse_) {
		pos.x -= velocity.x;
		pos.y -= velocity.y;
		pos.z -= velocity.z;
	}
	else {
		pos.x += velocity.x;
		pos.y += velocity.y;
		pos.z += velocity.z;
	}
	obj_->SetPosition(pos);
	//指定の位置に到達したら反転
	if (pos.x >= 30.0f) {
		isReverse_ = true;
	}
	if (pos.x <= -30.0f) {
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