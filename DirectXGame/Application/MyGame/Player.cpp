#include "Player.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "GamePlayScene.h"

using namespace DirectX;
CollisionManager* Player::colManager_ = CollisionManager::GetInstance();

Player::~Player() {
	//モデルの解放
	delete particleDash_;
	delete pmDash_;
}

std::unique_ptr<Player> Player::Create(Model* model, Model* bullet, GamePlayScene* gamescene)
{
	//インスタンス生成
	std::unique_ptr<Player> ins = std::make_unique<Player>();
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
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

bool Player::Initialize() {

	if (!Object3d::Initialize()) return false;
	input_ = Input::GetInstance();

	life_ = 10;
	isDead_ = false;
	isHit_ = false;
	mutekiCount_ = 0;

	isRight_ = true;
	//ジャンプしたか
	onGround_ = true;

	//奥側にいるか
	isJumpBack_ = false;
	//奧にいるか
	isBack_ = false;

	//奥側ジャンプに使うベジェ曲線用の時間
	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間
	elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	maxTime = 1.0f;					//全体時間
	timeRate;

	//パーティクル
	particleDash_ = Particle::LoadFromParticleTexture("particle1.png");
	pmDash_ = ParticleManager::Create();
	pmDash_->SetParticleModel(particleDash_);

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	return true;
}

void Player::Reset() {

	life_ = 10;
	isDead_ = false;

	isRight_ = true;
	//ジャンプしたか
	onGround_ = true;

	//奥側にいるか
	isJumpBack_ = false;
	//奧にいるか
	isBack_ = false;

	//奥側ジャンプに使うベジェ曲線用の時間
	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間
	elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	maxTime = 1.0f;					//全体時間

}
void Player::Update(bool isBack, bool isAttack) {

	pmDash_->SetCamera(camera_);

	if (!isDead_)
	{

		//移動処理
		if (!isJumpBack_)Move();
		//攻撃処理
		FallAndJump();
		if (isBack)JumpBack();
		if (isAttack)Attack();

		if (life_ <= 0)
		{
			isDead_ = true;
		}
		if (position_.y <= -60.0f)isDead_ = true;

		if (isHit_)
		{
			nowEye_ = camera_->GetEye();
			nowTarget_ = camera_->GetTarget();

			isShake_ = true;
			isHit_ = false;
		}
		if (isShake_)
		{
			XMFLOAT3 Eye = nowEye_ + hitMove_;
			XMFLOAT3 Target = nowTarget_ + hitMove_;
			camera_->ShakeEye(Eye, 10, { Eye.x - 2.0f,Eye.y - 2.0f,Eye.z - 2.0f },
				{ Eye.x + 2.0f,Eye.y + 2.0f,Eye.z + 2.0f });

			camera_->ShakeTarget(Target, 10, { Target.x - 2.0f,Target.y - 2.0f,Target.z - 2.0f },
				{ Target.x + 2.0f,Target.y + 2.0f,Target.z + 2.0f });
			camera_->Update();
			mutekiCount_++;
		}

		if (mutekiCount_ == MUTEKI_COUNT)
		{
			camera_->SetEye(nowEye_ + hitMove_);
			camera_->SetTarget(nowTarget_ + hitMove_);
			isShake_ = false;
			mutekiCount_ = 0;
			hitMove_ = { 0.0f,0.0f,0.0f };
		}
		//移動制限
		Trans();

	}
	pmDash_->Update();

	camera_->Update();
	UpdateWorldMatrix();
	collider_->Update();

	//着地処理
	Landing(COLLISION_ATTR_LANDSHAPE);

}

void Player::Draw() { Object3d::Draw(); }

void Player::DrawParticle() { pmDash_->Draw(); }

//移動処理
void Player::Move() {

	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 rot = Object3d::GetRotation();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	float moveSpeed = 0.5f;

	//キーボード入力による移動処理
	XMMATRIX matTrans = XMMatrixIdentity();

	//ダッシュ
	if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
	{
		if (input_->PushKey(DIK_A)) {
			isRight_ = false;
			pmDash_->ActiveX(particleDash_, Object3d::GetPosition(), { 0.0f ,3.0f,0.0f }, { 3.0f,0.3f,0.3f }, { 0.0f,0.001f,0.0f }, 2, { 1.0f, 0.0f });
			rot = { 0.0f,-90.0f,0.0f };
			move.x -= moveSpeed * 1.5f;
			cmove.x -= moveSpeed * 1.5f;
			tmove.x -= moveSpeed * 1.5f;
			if (isShake_)hitMove_.x -= moveSpeed * 1.5f;
		}
		if (input_->PushKey(DIK_D)) {
			isRight_ = true;
			pmDash_->ActiveX(particleDash_, Object3d::GetPosition(), { 0.0f ,3.0f,0.0f }, { -3.0f,0.3f,0.3f }, { 0.0f,0.001f,0.0f }, 2, { 1.0f, 0.0f });
			rot = { 0.0f,90.0f,0.0f };
			move.x += moveSpeed * 1.5f;
			cmove.x += moveSpeed * 1.5f;
			tmove.x += moveSpeed * 1.5f;
			if (isShake_)hitMove_.x += moveSpeed * 1.5f;
		}
	}
	else
	{
		if (input_->PushKey(DIK_A)) {
			isRight_ = false;//左向き
			rot = { 0.0f,-90.0f,0.0f };
			move.x -= moveSpeed;
			cmove.x -= moveSpeed;
			tmove.x -= moveSpeed;
			if (isShake_)hitMove_.x -= moveSpeed;
		}
		if (input_->PushKey(DIK_D)) {
			isRight_ = true;
			rot = { 0.0f,90.0f,0.0f };
			move.x += moveSpeed;
			cmove.x += moveSpeed;
			tmove.x += moveSpeed;
			if (isShake_)hitMove_.x += moveSpeed;
		}


	}

	Object3d::SetPosition(move);
	Object3d::SetRotation(rot);
	camera_->SetEye(cmove);
	camera_->SetTarget(tmove);
}

void Player::CameraMove()
{
	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	float moveSpeed = 1.0f;

	//キーボード入力による移動処理
	XMMATRIX matTrans = XMMatrixIdentity();
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= moveSpeed;
		cmove.x -= moveSpeed;
		tmove.x -= moveSpeed;
	}
	if (input_->PushKey(DIK_RIGHT)) {
		move.x += moveSpeed;
		cmove.x += moveSpeed;
		tmove.x += moveSpeed;
	}

	Object3d::SetPosition(move);
	camera_->SetEye(cmove);
	camera_->SetTarget(tmove);
}

void Player::FallAndJump()
{
	//旧
	//XMFLOAT3 move = Object3d::GetPosition();

	////キーボード入力による移動処理
	//XMMATRIX matTrans = XMMatrixIdentity();
	//if ()
	//{
	//	if (input_->TriggerKey(DIK_SPACE)) {
	//		isJump = true;
	//		gravity = 0.0f;
	//	}
	//}
	//else
	//{
	//	move.y += power + gravity;
	//	gravity -= 0.1f;

	//	if (gravity <= -4.0f)
	//	{
	//		gravity = -4.0f;
	//	}
	//	if (move.y <= -10.0f)
	//	{
	//		move.y = -10.0f;
	//		isJump = false;
	//	}
	//}
	//Object3d::SetPosition(move);

	if (!onGround_)
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
	//ジャンプ操作
	else if (input_->TriggerKey(DIK_SPACE))
	{
		onGround_ = false;

		fallVec_ = { 0.0f,jumpVYFist_,0.0f };
	}

}

void Player::JumpBack()
{
	XMFLOAT3 move = Object3d::GetPosition();

	//制御点
	start = { move.x,-10.0f,-60.0f };
	p1 = { move.x,10.0f,-40.0f };
	p2 = { move.x,10.0f,-20.0f };
	end = { move.x,-10.0f,0.0f };

	//時間

	if (onGround_)
	{
		if (!isJumpBack_)
		{
			if (input_->TriggerKey(DIK_Z))
			{

				if (isBack_)isBack_ = false;
				else isBack_ = true;
				isJumpBack_ = true;
			}
		}
	}
	if (isJumpBack_)
	{
		//現在時間を取得する
		nowCount = std::chrono::steady_clock::now();
		//前回記録からの経過時間を取得する
		elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

		timeRate = min(elapsed / maxTime, 1.0f);

		if (isBack_)move = Bezier3(end, p2, p1, start, timeRate);

		else move = Bezier3(start, p1, p2, end, timeRate);

		if (move.z >= end.z)
		{
			startCount = std::chrono::steady_clock::now();
			isJumpBack_ = false;
		}
		else if (move.z <= start.z)
		{
			startCount = std::chrono::steady_clock::now();
			isJumpBack_ = false;
		}
	}

	Object3d::SetPosition(move);
}

void Player::Landing(unsigned short attribute)
{
	//球コライダーの取得
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	assert(sphereCollider);

	//専用クエリーコールバッククラス定義
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		PlayerQueryCallback(Sphere* sphere) :sphere_(sphere) {};

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
	PlayerQueryCallback callback(sphereCollider);

	//球と地形の交差を全検索
	colManager_->QuerySphere(*sphereCollider, &callback, attribute);
	//交差による排斥分動かす
	position_.x += callback.move.m128_f32[0];
	position_.y += callback.move.m128_f32[1];
	//position_.z += callback.move.m128_f32[2];

	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();

	eyepos.x += callback.move.m128_f32[0];

	tarpos.x += callback.move.m128_f32[0];

	//コライダー更新
	UpdateWorldMatrix();
	camera_->SetEye(eyepos);
	camera_->SetTarget(tarpos);
	collider_->Update();

	//球の上端から球の下端までのレイキャスト用レイを準備
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RaycastHit raycastHit;
	//接地状態
	if (onGround_)
	{
		//スムーズに坂を下るための吸着処理
		const float adsDistance = 0.2f;
		//接地を維持
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
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
	else if (fallVec_.y <= 0.0f)
	{
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * 2.0f))
		{
			//着地
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//行列更新
			Object3d::Update();
		}
	}

	//行列更新等
	Object3d::Update();
}

//攻撃処理
void Player::Attack() {

	if (input_->TriggerKey(DIK_X)) {
		//弾の速度
		const float kBulletSpeed = 1.0f;
		XMFLOAT3 velocity;
		if (isRight_)velocity = { kBulletSpeed, 0.0f, 0.0f };
		else velocity = { -kBulletSpeed, 0.0f, 0.0f };

		XMMATRIX matVec = XMMatrixIdentity();
		matVec.r[0].m128_f32[0] = velocity.x;
		matVec.r[0].m128_f32[1] = velocity.y;
		matVec.r[0].m128_f32[2] = velocity.z;
		matVec.r[0].m128_f32[3] = 0.0f;
		matVec *= Object3d::GetWorld();
		//自キャラの座標をコピー
		XMFLOAT3 pos = Object3d::GetPosition();

		//弾を生成し初期化
		std::unique_ptr<PlayerBullet> newBullet;
		newBullet = PlayerBullet::Create(position_, velocity, modelBullet_);
		newBullet->SetCamera(camera_);
		newBullet->Update();

		//弾を登録
		gameScene_->AddPlayerBullet(std::move(newBullet));
		//あるメモリの所有権を持つunique_ptrはただ一つしか存在できない
		//その所有権を謙渡するための機能が std::move()
	}
}

void Player::Trans() {

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

//ワールド座標を取得
XMFLOAT3 Player::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

//衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute) {
	if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (isShake_)return;

		if (subAttribute == SUBCOLLISION_ATTR_NONE)life_ -= 2;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_POWER)life_ -= 4;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_GUARD)life_--;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_SPEED)life_--;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_DEATH)life_ -= 6;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;

		pmDash_->ActiveZ(particleDash_, { Object3d::GetPosition() }, { 0.0f ,0.0f,25.0f },
			{ 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 30, { 3.0f, 0.0f });

		pmDash_->Update();
		isHit_ = true;
	}

	else if (attribute == COLLISION_ATTR_GIMMICK)
	{
		if (subAttribute == SUBCOLLISION_ATTR_GIMMICK_SPIKE)
		{
			if (isShake_)return;
			life_ -= 3;
			pmDash_->ActiveZ(particleDash_, { Object3d::GetPosition() }, { 0.0f ,0.0f,25.0f },
				{ 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 30, { 3.0f, 0.0f });

			pmDash_->Update();
			isHit_ = true;
		}

	}
}

const XMFLOAT3 Player::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
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
