#include "Player.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "GamePlayScene.h"
#include "EnumList.h"
#include "MyMath.h"

using namespace DirectX;

/*

*	Player.cpp

*	自機

*/

//静的メンバ変数の実体
CollisionManager* Player::colManager_ = CollisionManager::GetInstance();

Player::~Player() {
	//スプライト解放
	delete spriteLifeBar_;
	delete spriteHit_;
	delete spriteExplosion_;

	//パーティクルモデルの解放
	delete particleSmoke_;
	delete pmSmoke_;

	delete particleFire_;
	delete pmFire_;
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

	//シングルトンインスタンス
	spCommon_ = SpriteCommon::GetInstance();
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
	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間
	maxTime_ = 1.0f;					//全体時間

	//スプライト
	spCommon_->LoadTexture(GPSPTI_PlayerLifeBarTex, "texture/plife2.png");
	spriteLifeBar_->Initialize(spCommon_, GPSPTI_PlayerLifeBarTex);
	spriteLifeBar_->SetPosition(lifeBarPos_);
	spriteLifeBar_->SetColor(green_);//基本は緑
	spriteLifeBar_->Update();

	spCommon_->LoadTexture(GPSPTI_PlayerHitEffectTex, "texture/fade2.png");
	spriteHit_->Initialize(spCommon_, GPSPTI_PlayerHitEffectTex);
	spriteHit_->SetColor(hitColor_);//色は赤いが基本は透明
	spriteHit_->Update();

	spCommon_->LoadTexture(GPSPTI_PlayerExplosionTex, "texture/explosion2.png");
	spriteExplosion_->Initialize(spCommon_, GPSPTI_PlayerExplosionTex);
	spriteExplosion_->SetAnchorPoint({ 0.5f,0.5f });
	spriteExplosion_->SetPosition(explosionPos_);
	spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[0].start,easeExplosionSizeAndAlpha_[1].start });
	spriteExplosion_->SetColor({ 1.0f,1.0f,1.0f,easeExplosionSizeAndAlpha_[2].start });
	spriteExplosion_->Update();
	
	//パーティクル
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_);

	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_);

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
	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間
	maxTime_ = 1.0f;					//全体時間

}
void Player::Update(bool isBack, bool isAttack, bool isStart) {
	
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		if (isAlive_)UpdateAlive(isBack, isAttack);
		else if (isBreak_)UpdateBreak();
		else if (isGoal_)UpdateGoal();
	}


	camera_->Update();
	UpdateWorldMatrix();
	pmFire_->Update();
	pmSmoke_->Update();

	collider_->Update();
	//着地処理
	Landing(COLLISION_ATTR_LANDSHAPE);

	spriteLifeBar_->SetTextureSize({ lifeBarDamageSize_ * life_,lifeBarDamageSize_ });
	spriteLifeBar_->SetSize({ lifeBarDamageSize_ * life_,lifeBarDamageSize_ });
	const int dangerLifeZone = 3;

	if (life_ <= dangerLifeZone) { spriteLifeBar_->SetColor(red_); }
	else { spriteLifeBar_->SetColor(green_); }

	spriteLifeBar_->Update();
	spriteHit_->Update();
	spriteExplosion_->Update();
}

void Player::Draw() { if (!isExplosion_)Object3d::Draw(); }

void Player::DrawSprite()
{
	if (!isBreak_)
	{
		if (isAlive_)spriteLifeBar_->Draw();
		spriteHit_->Draw();
	}
	else
	{
		spriteExplosion_->Draw();
	}
}

void Player::DrawParticle() {
	pmSmoke_->Draw();
	pmFire_->Draw();
	
}

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
			pmSmoke_->ActiveX(particleSmoke_, Object3d::GetPosition(), { 0.0f ,3.0f,0.0f }, { 3.0f,0.3f,0.3f }, { 0.0f,0.001f,0.0f }, 2, { 1.0f, 0.0f });
			rot = { 0.0f,-90.0f,0.0f };
			move.x -= moveSpeed * 1.5f;
			cmove.x -= moveSpeed * 1.5f;
			tmove.x -= moveSpeed * 1.5f;
			if (isShake_)hitMove_.x -= moveSpeed * 1.5f;
		}
		if (input_->PushKey(DIK_D)) {
			isRight_ = true;
			pmSmoke_->ActiveX(particleSmoke_, Object3d::GetPosition(), { 0.0f ,3.0f,0.0f }, { -3.0f,0.3f,0.3f }, { 0.0f,0.001f,0.0f }, 2, { 1.0f, 0.0f });
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

	if (onGround_)
	{
		if (!isJumpBack_)
		{
			if (input_->TriggerKey(DIK_Z))
			{
				jumpBackPos_ = position_;
				if (isBack_)isBack_ = false;
				else isBack_ = true;
				isJumpBack_ = true;
			}
		}
	}
	if (isJumpBack_)
	{
		const float offsetPosY = 1.0f;
		const float JumpBackPosY = 20.0f;
		//制御点
		start_ = { move.x,jumpBackPos_.y - offsetPosY,-60.0f };
		point1_ = { move.x,jumpBackPos_.y + JumpBackPosY,-40.0f };
		point2_ = { move.x,jumpBackPos_.y + JumpBackPosY,-20.0f };
		end_ = { move.x,jumpBackPos_.y - offsetPosY,0.0f };

		//現在時間を取得する
		nowCount_ = std::chrono::steady_clock::now();
		//前回記録からの経過時間を取得する
		elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

		timeRate_ = min(elapsed / maxTime_, 1.0f);

		if (isBack_)move = Bezier3(end_, point2_, point1_, start_, timeRate_);

		else move = Bezier3(start_, point1_, point2_, end_, timeRate_);

		if (move.z >= end_.z)
		{
			startCount_ = std::chrono::steady_clock::now();
			isJumpBack_ = false;
		}
		else if (move.z <= start_.z)
		{
			startCount_ = std::chrono::steady_clock::now();
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
void Player::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute) {
	if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (isShake_)return;

		if (subAttribute == SUBCOLLISION_ATTR_NONE)life_ -= 2;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_POWER)life_ -= 4;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_GUARD)life_--;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_SPEED)life_--;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_DEATH)life_ -= 6;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;

		pmSmoke_->ActiveZ(particleSmoke_, { Object3d::GetPosition() }, { 0.0f ,0.0f,25.0f },
			{ 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 30, { 3.0f, 0.0f });

		pmSmoke_->Update();
		isHit_ = true;
	}

	else if (attribute == COLLISION_ATTR_GIMMICK)
	{
		if (subAttribute == SUBCOLLISION_ATTR_GIMMICK_SPIKE)
		{
			if (isShake_)return;
			life_ -= 3;
			pmSmoke_->ActiveZ(particleSmoke_, { Object3d::GetPosition() }, { 0.0f ,0.0f,25.0f },
				{ 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 30, { 3.0f, 0.0f });

			pmSmoke_->Update();
			isHit_ = true;
		}

	}
	else if (attribute==COLLISION_ATTR_GOAL)
	{
		if (isGoal_)return;
		scale_ = { easeChangeScaleStageClear_[0].start,easeChangeScaleStageClear_[1].start ,easeChangeScaleStageClear_[2].start };
		for (int i = 0; i < 3; i++)easeChangeScaleStageClear_[i].Standby(false);
		stopPos_ = position_;
		isGoal_ = true;
		isAlive_ = false;
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

void Player::UpdateAlive(bool isBack, bool isAttack)
{
	if (isDead_)return;
	//移動処理
	if (!isJumpBack_)Move();
	//攻撃処理
	FallAndJump();
	if (isBack)JumpBack();
	if (isAttack)Attack();

	if (life_ <= 0)
	{
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();

		easeOffset_ = { -18.0f,position_.y,85.0f + position_.z };//最初にオフセットを足さないと右カメラ、1回足すと中央カメラ、2回足したら左カメラ
		//右カメラ視点[i][0]
		easeDeadCameraEye_[0][0].SetEasing(nowEye_.x, nowEye_.x + easeOffset_.x, 1.0f);
		easeDeadCameraEye_[1][0].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
		easeDeadCameraEye_[2][0].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
		//右カメラ注視点[i][0]
		easeDeadCameraTarget_[0][0].SetEasing(nowTarget_.x, nowTarget_.x + easeOffset_.x, 1.0f);
		easeDeadCameraTarget_[1][0].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, 1.0f);
		easeDeadCameraTarget_[2][0].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, 1.0f);

		for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][0].Standby(false);
		for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][0].Standby(false);

		isBreak_ = true;
		isAlive_ = false;
	}
	if (position_.y <= -60.0f)isDead_ = true;

	if (isHit_)
	{
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();
		easeHit_.Standby(false);
		isShake_ = true;
		isHit_ = false;
	}
	if (isShake_)
	{
		//視点シェイク
		XMFLOAT3 Eye = nowEye_ + hitMove_;
		const XMFLOAT3 hitEye = { 1.0f,1.0f,1.0f };
		camera_->ShakeEye(Eye, 1, { Eye.x - hitEye.x,Eye.y - hitEye.y,Eye.z - hitEye.z },
			{ Eye.x + hitEye.x,Eye.y + hitEye.y,Eye.z + hitEye.z });

		//注視点シェイク
		XMFLOAT3 Target = nowTarget_ + hitMove_;
		const XMFLOAT3 hitTarget = { 1.0f,1.0f,1.0f };
		camera_->ShakeTarget(Target, 1, { Target.x - hitTarget.x,Target.y - hitTarget.y,Target.z - hitTarget.z },
			{ Target.x + hitTarget.x,Target.y + hitTarget.y,Target.z + hitTarget.z });
		camera_->Update();

		//+してイージング
		easeHit_.ease_out_cubic();
		spriteHit_->SetColor({ hitColor_.x,hitColor_.y,hitColor_.z ,easeHit_.num_X });

		mutekiCount_++;
	}

	if (mutekiCount_ == MUTEKI_COUNT)
	{
		camera_->SetEye(nowEye_ + hitMove_);
		camera_->SetTarget(nowTarget_ + hitMove_);
		isShake_ = false;
		mutekiCount_ = 0;
		hitMove_ = resetHitMove_;
	}
	//移動制限
	Trans();

#ifdef _DEBUG
	//デバッグ用
	//if (input_->TriggerKey(DIK_M))
	//{
	//	nowEye_ = camera_->GetEye();
	//	nowTarget_ = camera_->GetTarget();

	//	easeOffset_ = { -18.0f,position_.y,85.0f + position_.z };//最初にオフセットを足さないと右カメラ、1回足すと中央カメラ、2回足したら左カメラ
	//	//右カメラ視点[i][0]
	//	easeDeadCameraEye_[0][0].SetEasing(nowEye_.x, nowEye_.x + easeOffset_.x, 1.0f);
	//	easeDeadCameraEye_[1][0].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
	//	easeDeadCameraEye_[2][0].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
	//	//右カメラ注視点[i][0]
	//	easeDeadCameraTarget_[0][0].SetEasing(nowTarget_.x, nowTarget_.x + easeOffset_.x, 1.0f);
	//	easeDeadCameraTarget_[1][0].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, 1.0f);
	//	easeDeadCameraTarget_[2][0].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, 1.0f);


	//	for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][0].Standby(false);
	//	for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][0].Standby(false);

	//	isBreak_ = true;
	//	isAlive_ = false;
	//}
#endif // _DEBUG
}

void Player::UpdateBreak()
{
	if (isExplosion_)
	{
		easeExplosionSizeAndAlpha_[2].ease_out_sine();
		spriteExplosion_->SetColor({ 1.0f,1.0f,1.0f,easeExplosionSizeAndAlpha_[2].num_X });
		//炎プリセット
		ParticleManager::Preset fire =
		{
			particleFire_,{position_.x,position_.y - 5.0f,position_.z},{ 15.0f ,15.0f,15.0f },
			{ 3.3f,3.3f,3.3f },{ 0.0f,0.001f,0.0f },6,{ 7.0f, 0.0f },
			{ MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		//
		pmFire_->SetBlendMode(ParticleManager::BP_ALPHA);
		pmFire_->ActiveY(fire.particle, fire.startPos, fire.pos, fire.vel,
			fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);

		//煙プリセット
		ParticleManager::Preset smoke =
		{
			particleSmoke_,{position_.x,position_.y+5.0f,position_.z},{ 25.0f ,10.0f,15.0f },
			{ MyMath::RandomMTFloat(0.0f,0.1f),MyMath::RandomMTFloat(0.5f,3.0f),0.3f },
			{ 0.0f,0.001f,0.0f },5,{ 4.0f, 0.0f },
			{ MyMath::RandomMTFloat(0.8f,1.0f),MyMath::RandomMTFloat(0.8f,1.0f),MyMath::RandomMTFloat(0.95f,1.0f),1.0f },
			{ 1.0f,1.0f,1.0f,0.0f }
		};

		//煙も舞う
		pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
		pmSmoke_->ActiveY(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		//カメラ速度
		XMFLOAT3 cameraSppedEyeTarget;
		cameraSppedEyeTarget.x = MyMath::RandomMTFloat(-0.05f, 0.05f);
		cameraSppedEyeTarget.y = MyMath::RandomMTFloat(-0.05f, 0.05f);
		const float SpeedChangePosZ = -30.0f;
		if (position_.z >= SpeedChangePosZ) cameraSppedEyeTarget.z = 2.5f;
		else cameraSppedEyeTarget.z = 1.0f;

		XMFLOAT3 cameraEye = camera_->GetEye();
		XMFLOAT3 cameraTarget = camera_->GetTarget();

		cameraEye.x -= cameraSppedEyeTarget.x;
		cameraEye.y -= cameraSppedEyeTarget.y;
		cameraEye.z -= cameraSppedEyeTarget.z;
		cameraTarget.x -= cameraSppedEyeTarget.x;
		cameraTarget.y -= cameraSppedEyeTarget.y;
		cameraTarget.z -= cameraSppedEyeTarget.z;

		camera_->SetEye(cameraEye);
		camera_->SetTarget(cameraTarget);

		if (camera_->GetEye().z <= -cameraEyeChangeGameover_) isDead_ = true;
	}
	else if (isCameraCentralEnd_)
	{
		//爆発
		for (int i = 0; i < 2; i++)easeExplosionSizeAndAlpha_[i].ease_out_expo();

		spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[0].num_X,easeExplosionSizeAndAlpha_[1].num_X });

		if (spriteExplosion_->GetSize().x == easeExplosionSizeAndAlpha_[0].end)
		{
			easeExplosionSizeAndAlpha_[2].Standby(false);//カラーだけ
			isExplosion_ = true;
			isCameraCentralEnd_ = false;
		}
	}
	else if (isCameraRightEnd_)
	{
		//カメラ移動第3波(中央)
		for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][2].ease_out_cubic();
		for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][2].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[0][2].num_X,easeDeadCameraEye_[1][2].num_X, easeDeadCameraEye_[2][2].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[0][2].num_X,easeDeadCameraTarget_[1][2].num_X, easeDeadCameraTarget_[2][2].num_X });
		if (camera_->GetEye().z == easeDeadCameraEye_[2][2].end)
		{

			for (int i = 0; i < 2; i++)easeExplosionSizeAndAlpha_[i].Standby(false);//サイズだけ
			isCameraCentralEnd_ = true;
			isCameraRightEnd_ = false;
		}

	}
	else if (isCameraLeftEnd_)
	{
		//カメラ移動第2波(左)
		for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][1].ease_out_cubic();
		for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][1].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[0][1].num_X,easeDeadCameraEye_[1][1].num_X, easeDeadCameraEye_[2][1].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[0][1].num_X,easeDeadCameraTarget_[1][1].num_X, easeDeadCameraTarget_[2][1].num_X });
		if (camera_->GetEye().z == easeDeadCameraEye_[2][1].end)
		{
			//中央カメラ視点[i][2]
			easeDeadCameraEye_[0][2].SetEasing(nowEye_.x + easeOffset_.x, nowEye_.x + easeOffset_.x, 1.0f);
			easeDeadCameraEye_[1][2].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
			easeDeadCameraEye_[2][2].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
			//中央カメラ注視点[i][2]
			easeDeadCameraTarget_[0][2].SetEasing(nowTarget_.x + easeOffset_.x, nowTarget_.x + easeOffset_.x, 1.0f);
			easeDeadCameraTarget_[1][2].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, 1.0f);
			easeDeadCameraTarget_[2][2].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, 1.0f);
			//カメラ移動第3波準備(中央)
			for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][2].Standby(false);
			for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][2].Standby(false);

			camera_->SetEye({ easeDeadCameraEye_[0][2].start,easeDeadCameraEye_[1][2].start, easeDeadCameraEye_[2][2].start });
			camera_->SetTarget({ easeDeadCameraTarget_[0][2].start,easeDeadCameraTarget_[1][2].start, easeDeadCameraTarget_[2][2].start });

			isCameraRightEnd_ = true;
			isCameraLeftEnd_ = false;
		}
	}
	else
	{
		//カメラ移動第1波(右)
		for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][0].ease_out_cubic();
		for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][0].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[0][0].num_X,easeDeadCameraEye_[1][0].num_X, easeDeadCameraEye_[2][0].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[0][0].num_X,easeDeadCameraTarget_[1][0].num_X, easeDeadCameraTarget_[2][0].num_X });
		if (camera_->GetEye().z == easeDeadCameraEye_[2][0].end)
		{
			//左カメラ視点[i][1]
			easeDeadCameraEye_[0][1].SetEasing(nowEye_.x + (easeOffset_.x + easeOffset_.x), nowEye_.x + easeOffset_.x, 1.0f);
			easeDeadCameraEye_[1][1].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
			easeDeadCameraEye_[2][1].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
			//左カメラ注視点[i][1]
			easeDeadCameraTarget_[0][1].SetEasing(nowTarget_.x + (easeOffset_.x + easeOffset_.x), nowTarget_.x + easeOffset_.x, 1.0f);
			easeDeadCameraTarget_[1][1].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, 1.0f);
			easeDeadCameraTarget_[2][1].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, 1.0f);
			//カメラ移動第2波準備(左)
			for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][1].Standby(false);
			for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][1].Standby(false);

			camera_->SetEye({ easeDeadCameraEye_[0][1].start,easeDeadCameraEye_[1][1].start, easeDeadCameraEye_[2][1].start });
			camera_->SetTarget({ easeDeadCameraTarget_[0][1].start,easeDeadCameraTarget_[1][1].start, easeDeadCameraTarget_[2][1].start });

			isCameraLeftEnd_ = true;
		}
	}

	if (input_->TriggerKey(DIK_M))isDead_ = true;
}

void Player::UpdateGoal()
{
	//座標を固定してスケールをイージング
	position_ = stopPos_;
	for (int i = 0; i < 3; i++)easeChangeScaleStageClear_[i].ease_out_cubic();
	scale_ = { easeChangeScaleStageClear_[0].num_X,easeChangeScaleStageClear_[1].num_X ,easeChangeScaleStageClear_[2].num_X };
	
}
