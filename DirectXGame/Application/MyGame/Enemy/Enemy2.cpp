#include "Enemy2.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;

CollisionManager* Enemy2::colManager_ = CollisionManager::GetInstance();

Enemy2::~Enemy2() {
}

std::unique_ptr<Enemy2> Enemy2::Create(Model* model, Model* bullet, Player* player, GamePlayScene* gamescene, int level)
{
	//インスタンス生成
	std::unique_ptr<Enemy2> ins = std::make_unique<Enemy2>();
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
bool Enemy2::Initialize() {

	if (!Object3d::Initialize()) return false;
	Parameter();

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	return true;
}

//パラメータ
void Enemy2::Parameter() {

	isReverse_ = false;
	//ジャンプしたか
	onGround = true;
	//初期フェーズ
	phase_ = Phase::Approach;

	//発射タイマー初期化
	fireTimer = kFireInterval;

	life_ = 2;
	isDead_ = false;

	kFireInterval = MyMath::RandomMTInt(100, 150);
}

//リセット
void Enemy2::Reset() { Parameter(); }

//更新
void Enemy2::Update() {


	//座標を移動させる
	switch (phase_) {
	case Enemy2::Phase::Approach:

		UpdateApproach();
		break;
	case Enemy2::Phase::Leave:
		UpdateLeave();
		break;
	}


	//行列更新
	Trans();
	camera_->Update();
	collider->Update();
	//行列更新等
	Object3d::Update();

}

//転送
void Enemy2::Trans() {

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
	XMFLOAT3 position = GetPosition();

	//弾を生成し初期化
	std::unique_ptr<EnemyBullet> newBullet;
	newBullet = EnemyBullet::Create(position, velocity, modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//弾を登録
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

void Enemy2::Landing()
{
	//球コライダーの取得
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	//専用クエリーコールバッククラス定義
	class EnemyQueryCallback : public QueryCallback
	{
	public:
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
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	//position.z += callback.move.m128_f32[2];

	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();

	eyepos.x += callback.move.m128_f32[0];

	tarpos.x += callback.move.m128_f32[0];

	//コライダー更新
	UpdateWorldMatrix();
	camera_->SetEye(eyepos);
	camera_->SetTarget(tarpos);
	collider->Update();

	//球の上端から球の下端までのレイキャスト用レイを準備
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RaycastHit raycastHit;
	//接地状態
	if (onGround)
	{
		//スムーズに坂を下るための吸着処理
		const float adsDistance = 0.2f;
		//接地を維持
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//行列更新
			Object3d::Update();
		}
		//地面が無いので落下
		else
		{
			onGround = false;
			fallVec = {};
		}
	}
	//落下状態
	else if (fallVec.y <= 0.0f)
	{
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 2.0f))
		{
			//着地
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//行列更新
			Object3d::Update();
		}
	}

}

//描画
void Enemy2::Draw() {

	//モデルの描画
	Object3d::Draw();


}


//状態変化用の更新関数
//接近
void Enemy2::UpdateApproach() {
	//速度
	XMFLOAT3 velocity;

	//移動
	velocity = { 0.1f, 0.0f, -0.2f };
	if(isReverse_)position.x -= velocity.x;
	else position.x += velocity.x;

	position.y += velocity.y;
	position.z += velocity.z;

	if (position.x >= 25.0f) {
		isReverse_ = true;
	}
	if (position.x <= -25.0f) {
		isReverse_ = false;
	}
	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
		//弾発射
		Fire();
		//発射タイマー初期化
		fireTimer = MyMath::RandomMTInt(kFireInterval / 2, kFireInterval);
	}

	if (!onGround)
	{
		//下向き加速度
		const float fallAcc = -0.1f;
		const float fallVYMin = -2.0f;
		//加速
		fallVec.y = max(fallVec.y + fallAcc, fallVYMin);
		//移動
		position.x += fallVec.x;
		position.y += fallVec.y;
		position.z += fallVec.z;
	}

	//死んだら
	if (life_ <= 0) {
		isDead_ = true;
		life_ = 0;
	}
	if (position.z <= -200.0f)isDead_ = true;
}

//離脱
void Enemy2::UpdateLeave() {
	//速度

}

//ワールド座標を取得
XMFLOAT3 Enemy2::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void Enemy2::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;
	}
	else if (attribute == COLLISION_ATTR_EARTH)
	{
		isDead_ = true;
	}

}
