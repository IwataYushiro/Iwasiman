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
	maxTime_ = 5.0f;					//全体時間

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, this->radius_));
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);

	Parameter();

	return true;
}

//パラメータ
void EnemyCore::Parameter() {
	phase_ = Phase::CoreStage1;
	maxTime_ = 2.0f;
	life_ = 5;


	isReverse_ = false;
	//発射タイマー初期化
	fireTimer_ = kFireInterval;

	isDead_ = false;


}

//リセット
void EnemyCore::Reset() { Parameter(); }

//更新
void EnemyCore::Update(bool isStart) {

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
	Object3d::Draw();



}

void EnemyCore::UpdateCore()
{
	//速度
	float cameraMove = camera_->GetEye().x;

	//制御点
	start_ = nowPos_;
	point1_ = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,40.0f,70.0f };
	point2_ = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,25.0f,85.0f };
	end_ = { MyMath::RandomMTFloat(-20.0f,20.0f) + cameraMove,10.0f,100.0f };

	//速度
	XMFLOAT3 velocity;
	//移動
	if (!isReverse_)velocity = { 0.3f, 0.0f, 0.0f };
	else velocity = { -0.3f, 0.0f, 0.0f };

	position_.x += velocity.x;
	position_.y += velocity.y;
	position_.z += velocity.z;

	//指定の位置に到達したら反転
	if (position_.x >= 65.0f) {
		//→から←
		isReverse_ = true;
	}
	if (position_.x <= -65.0f) {
		isReverse_ = false;
	}

	//発射タイマーカウントダウン
	fireTimer_--;
	//指定時間に達した
	if (fireTimer_ <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer_ = MyMath::RandomMTInt(kFireInterval, kFireInterval * 2);
	}
	//死んだら自機の弾みたいに
	if (life_ == 0) {
		nowPos_ = Object3d::GetPosition();

		collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
		collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);

		life_ = 0;
		startCount_ = std::chrono::steady_clock::now();	//開始時間
		//敵にぶつける

		phase_ = Phase::CoreBreak;
	}
}

void EnemyCore::UpdateBreakCore()
{

	//時間

	//現在時間を取得する
	nowCount_ = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

	float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	timeRate_ = min(elapsed / maxTime_, 1.0f);

	position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);

}

//離脱
void EnemyCore::UpdateLeave() {

	isDead_ = true;
	bossDead_ = true;
}

const XMFLOAT3 EnemyCore::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
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
	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;
	}

	else if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isDead_ = true;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}
