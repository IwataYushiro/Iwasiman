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

}

std::unique_ptr<Player> Player::Create(const PlayerModelList* model,GamePlayScene* gamescene)
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
	if (model->playerModel)ins->modelPlayer_ = model->playerModel;
	if (model->playerBullet) ins->modelBullet_ = model->playerBullet;
	if (model->playerHit)ins->modelHit_ = model->playerHit;
	if (model->playerMove)ins->modelMove_ = model->playerMove;
	if (gamescene)ins->SetGameScene(gamescene);

	//最初のモデル
	ins->SetModel(model->playerModel);
	return ins;
}

bool Player::Initialize() {

	if (!Object3d::Initialize()) return false;

	//シングルトンインスタンス
	spCommon_ = SpriteCommon::GetInstance();
	input_ = Input::GetInstance();

	const int32_t startLife = 10;
	life_ = startLife;
	isDead_ = false;
	isHit_ = false;
	mutekiCount_ = 0;

	isRight_ = true;
	//ジャンプしたか
	onGround_ = true;

	const float startJumpVYFist = 2.0f;
	jumpVYFist_ = startJumpVYFist;

	//奥側にいるか
	isJumpBack_ = false;
	//奧にいるか
	isBack_ = false;

	//奥側ジャンプに使うベジェ曲線用の時間
	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間

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
	spriteExplosion_->SetAnchorPoint(explosionAnchorPoint_);
	spriteExplosion_->SetPosition(explosionPos_);
	spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[XYW_X].start,easeExplosionSizeAndAlpha_[XYW_Y].start });
	spriteExplosion_->SetColor({ asIsColor_.x,asIsColor_.y,asIsColor_.z,easeExplosionSizeAndAlpha_[XYW_W].start });
	spriteExplosion_->Update();

	//パーティクル
	particleSmoke_ = Particle::LoadFromParticleTexture("particle8.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());

	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());

	//コライダー追加
	const XMVECTOR colliderOffset = { 0.0f,0.0f,0.0f,0.0f };
	SetCollider(new SphereCollider(colliderOffset, radius_));
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	easelifeBarSize_.Standby(false);
	return true;
}

void Player::Reset() {

	const int32_t resetLife = 10;
	life_ = resetLife;
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
}
void Player::Update(const bool isBack, const bool isAttack, const bool isStart) {

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

	easelifeBarSize_.ease_in_cubic();
	lifeBarDamageSize_.x = easelifeBarSize_.num_X;

	spriteLifeBar_->SetTextureSize({ lifeBarDamageSize_.x * life_,lifeBarDamageSize_.y });
	spriteLifeBar_->SetSize({ lifeBarDamageSize_.x * life_,lifeBarDamageSize_.y });
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

	easeRotateRightY_.ease_out_cubic();

	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 rot = Object3d::GetRotation();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	//スピード
	const float moveSpeed = 0.5f;//通常時
	const float dashSpeed = 1.5f;//ダッシュ時に掛ける

	//パーティクル
	const ParticleManager::Preset smoke =
	{
		particleSmoke_.get(),
		position_,
		{ 0.0f ,3.0f,0.0f },
		{ 3.0f,0.3f,0.3f },
		{ 0.0f,0.001f,0.0f },
		2,
		{ 1.0f, 0.0f },
		{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
		{0.0f,0.0f,0.0f,1.0f}
	};
	const XMFLOAT3 reverseParticleVel = { -smoke.vel.x,-smoke.vel.y, -smoke.vel.z };
	const int32_t walkParticleNum = 1;
	const XMFLOAT4 walkStartColor = { MyMath::RandomMTFloat(0.9f,1.0f),0.6f,0.6f,1.0f };
	//キーボード入力による移動処理
	XMMATRIX matTrans = XMMatrixIdentity();

	//ダッシュ
	if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
	{
		if (input_->PushKey(DIK_A)) {
			if (isRight_)
			{
				easeRotateRightY_.Standby(true);
				rot.y = easeRotateRightY_.end;
			}
			isRight_ = false;
			pmSmoke_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
				smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);
			move.x -= moveSpeed * dashSpeed;
			cmove.x -= moveSpeed * dashSpeed;
			tmove.x -= moveSpeed * dashSpeed;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x -= moveSpeed * dashSpeed;
		}
		else if (input_->PushKey(DIK_D)) {
			if (!isRight_)
			{
				easeRotateRightY_.Standby(false);
				rot.y = easeRotateRightY_.start;
			}
			model_ = modelMove_;
			isRight_ = true;
			pmSmoke_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, reverseParticleVel,
				smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);
			move.x += moveSpeed * dashSpeed;
			cmove.x += moveSpeed * dashSpeed;
			tmove.x += moveSpeed * dashSpeed;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x += moveSpeed * dashSpeed;
		}
	}
	else
	{
		if (input_->PushKey(DIK_A)) {
			if (isRight_)
			{
				easeRotateRightY_.Standby(true);
				rot.y = easeRotateRightY_.end;
			}
			isRight_ = false;
			pmSmoke_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
				smoke.acc, walkParticleNum, smoke.scale, walkStartColor, smoke.endColor);
			move.x -= moveSpeed;
			cmove.x -= moveSpeed;
			tmove.x -= moveSpeed;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x -= moveSpeed;
		}
		else if (input_->PushKey(DIK_D)) {
			if (!isRight_)
			{
				
				easeRotateRightY_.Standby(false);
				rot.y = easeRotateRightY_.start;
			}
			model_ = modelMove_;
			isRight_ = true;
			pmSmoke_->ActiveX(smoke.particle, smoke.startPos, smoke.pos, reverseParticleVel,
				smoke.acc, walkParticleNum, smoke.scale, walkStartColor, smoke.endColor);
			move.x += moveSpeed;
			cmove.x += moveSpeed;
			tmove.x += moveSpeed;
			rot.y = easeRotateRightY_.num_X;
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
	const float jumpPowerUp = 3.0f;
	const float jumpPowerDefault = 2.0f;
	if (isGetJumpItem_)
	{
		if (onGround_)jumpVYFist_ = jumpPowerUp;
		jumpPowerUpcount_++;
	}
	else
	{
		if (onGround_)jumpVYFist_ = jumpPowerDefault;
	}

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

		const XMFLOAT3 startJumpVec = { 0.0f,jumpVYFist_,0.0f };
		fallVec_ = startJumpVec;
	}
	

	if (jumpPowerUpcount_ >= JUMPITEM_MAX_TIME)
	{
		const float countReset = 0.0f;
		jumpPowerUpcount_ = countReset;
		isGetJumpItem_ = false;
	}

}

void Player::JumpBack()
{
	const float offsetPosY = 1.0f;
	const float JumpBackPosY = 20.0f;

	//ベジェ曲線の値
	const XMFLOAT3 startBezier3Pos = { position_.x,jumpBackPos_.y - offsetPosY,-60.0f };
	const XMFLOAT3 point1Bezier3Pos = { position_.x,jumpBackPos_.y + JumpBackPosY,-40.0f };
	const XMFLOAT3 point2Bezier3Pos = { position_.x,jumpBackPos_.y + JumpBackPosY,-20.0f };
	const XMFLOAT3 endBezier3Pos = { position_.x,jumpBackPos_.y - offsetPosY,0.0f };

	//制御点
	start_ = startBezier3Pos;
	point1_ = point1Bezier3Pos;
	point2_ = point2Bezier3Pos;
	end_ = endBezier3Pos;

	if (onGround_)
	{
		if (!isJumpBack_)
		{
			if (input_->TriggerKey(DIK_W))
			{
				if (isBack_)return;
				startCount_ = std::chrono::steady_clock::now();
				jumpBackPos_ = position_;
				isBack_ = true;
				isJumpBack_ = true;
			}
			if (input_->TriggerKey(DIK_S))
			{
				if (!isBack_)return;
				startCount_ = std::chrono::steady_clock::now();
				jumpBackPos_ = position_;
				isBack_ = false;
				isJumpBack_ = true;
			}
		}
	}
	if (isJumpBack_)
	{

		//現在時間を取得する
		nowCount_ = std::chrono::steady_clock::now();
		//前回記録からの経過時間を取得する
		elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

		const float micro = 1'000'000.0f;
		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//マイクロ秒を秒に単位変換

		const float timeRateMax = 1.0f;
		timeRate_ = min(elapsed / maxTime_, timeRateMax);

		if (isBack_)
		{
			position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);
			if (position_.z >= end_.z)isJumpBack_ = false;
		}
		else
		{
			position_ = Bezier3(end_, point2_, point1_, start_, timeRate_);
			if (position_.z <= start_.z)isJumpBack_ = false;
		}
	}

}

void Player::Landing(const unsigned short attribute)
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
			float cos = XMVector3Dot(rejectDir, up).m128_f32[XYZ_X];

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
	position_.x += callback.move.m128_f32[XYZ_X];
	position_.y += callback.move.m128_f32[XYZ_Y];
	//position_.z += callback.move.m128_f32[2];

	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();

	eyepos.x += callback.move.m128_f32[XYZ_X];

	tarpos.x += callback.move.m128_f32[XYZ_X];

	//コライダー更新
	UpdateWorldMatrix();
	camera_->SetEye(eyepos);
	camera_->SetTarget(tarpos);
	collider_->Update();

	//球の上端から球の下端までのレイキャスト用レイを準備
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[XYZ_Y] += sphereCollider->GetRadius();
	const XMVECTOR rayDir = { 0.0f,-1.0f,0.0f,0.0f };
	ray.dir = rayDir;
	RaycastHit raycastHit;

	//半径　X　2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//落下状態になるY値のスピード
	const float fallSpeedY = 0.0f;
	//接地状態
	if (onGround_)
	{
		//スムーズに坂を下るための吸着処理
		const float adsDistance = 0.2f;
		//接地を維持
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum + adsDistance))
		{
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
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
	else if (fallVec_.y <= fallSpeedY)
	{
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum))
		{
			//着地
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
			//行列更新
			Object3d::Update();
		}
	}

	//行列更新等
	Object3d::Update();
}

//攻撃処理
void Player::Attack() {

	if (input_->TriggerKey(DIK_L)) {
		//弾の速度
		const float bulletSpeed = 1.0f;
		XMFLOAT3 velocity;
		//向きによってスピードが変わる
		const XMFLOAT3 velLeft = { -bulletSpeed, 0.0f, 0.0f };
		const XMFLOAT3 velRight = { bulletSpeed, 0.0f, 0.0f };

		if (isRight_)velocity = velRight;
		else velocity = velLeft;
		//行列に速度値を渡す
		const XMFLOAT4 velosityMoveMatrix = { velocity.x,velocity.y,velocity.z,0.0f };

		XMMATRIX matVec = XMMatrixIdentity();
		matVec.r[XYZW_X].m128_f32[XYZW_X] = velosityMoveMatrix.x;
		matVec.r[XYZW_X].m128_f32[XYZW_Y] = velosityMoveMatrix.y;
		matVec.r[XYZW_X].m128_f32[XYZW_Z] = velosityMoveMatrix.z;
		matVec.r[XYZW_X].m128_f32[XYZW_W] = velosityMoveMatrix.w;
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
const XMFLOAT3 Player::GetWorldPosition() const {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

//衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision([[maybe_unused]] const CollisionInfo& info, 
	const unsigned short attribute, const unsigned short subAttribute) {

	//ダメージ管理の構造体
	struct DamageType
	{
		const int32_t enemyNone = 2;
		const int32_t enemyPower = 4;
		const int32_t enemyGuard = 1;
		const int32_t enemySpeed = 1;
		const int32_t enemyDeath = 6;
		const int32_t enemyBullet = 1;
		const int32_t GimmickSpike = 3;
	};
	DamageType damege;

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
		{ 1.0f,0.4f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (isShake_)return;
		if (isHit_)return;
		if (subAttribute == SUBCOLLISION_ATTR_NONE)life_ -= damege.enemyNone;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_POWER)life_ -= damege.enemyPower;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_GUARD)life_ -= damege.enemyGuard;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_SPEED)life_ -= damege.enemySpeed;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_DEATH)life_ -= damege.enemyDeath;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_ISDEAD)return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_ -= damege.enemyBullet;

		pmSmoke_->ActiveZ(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		pmSmoke_->Update();
		/*
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();
		easeHit_.Standby(false);
		if (onGround_) { isShake_ = true; }
		else isHit_ = true;
		*/
		//model_ = modelHit_;
		isHit_ = true;
	}

	else if (attribute == COLLISION_ATTR_GIMMICK)
	{
		if (subAttribute == SUBCOLLISION_ATTR_GIMMICK_SPIKE)
		{
			if (isShake_)return;
			if (isHit_)return;
			life_ -= damege.GimmickSpike;
			pmSmoke_->ActiveZ(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
				smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

			pmSmoke_->Update();

			/*
			nowEye_ = camera_->GetEye();
			nowTarget_ = camera_->GetTarget();
			easeHit_.Standby(false);
			if (onGround_) { isShake_ = true; }
			else isHit_ = true;
			*/
			//model_ = modelHit_;
			isHit_ = true;
		}

	}
	else if (attribute == COLLISION_ATTR_GOAL)
	{
		if (isGoal_)return;
		scale_ = { easeChangeScaleStageClear_[XYZ_X].start,easeChangeScaleStageClear_[XYZ_Y].start ,easeChangeScaleStageClear_[XYZ_Z].start };
		for (int i = 0; i < XYZ_Num; i++)easeChangeScaleStageClear_[i].Standby(false);
		stopPos_ = position_;
		isGoal_ = true;
		isAlive_ = false;
	}
	else if (attribute == COLLISION_ATTR_ITEM)
	{
		
		if (subAttribute == SUBCOLLISION_ATTR_ITEM_JUMP)
		{
			if (isGetJumpItem_)return; //多重ヒット防止
			isGetJumpItem_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_ITEM_HEAL)
		{
			const int heal = 1;
			life_ += heal;
			
		}
		
	}
}

const XMFLOAT3 Player::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
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

void Player::UpdateAlive(const bool isBack, const bool isAttack)
{
	if (isDead_)return;
	//移動処理
	Move();
	//攻撃処理
	FallAndJump();
	if (isBack)JumpBack();
	if (isAttack)Attack();

	if (life_ <= 0)
	{
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();

		const XMFLOAT3 offset = { -10.0f,position_.y,85.0f + position_.z };
		easeOffset_ = offset;//最初にオフセットを足さないと右カメラ、1回足すと中央カメラ、2回足したら左カメラ
		const float easeTime = 1.0f;
		//右カメラ視点[i]
		easeDeadCameraEye_[XYZ_X].SetEasing(nowEye_.x, nowEye_.x + easeOffset_.x, easeTime);
		easeDeadCameraEye_[XYZ_Y].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, easeTime);
		easeDeadCameraEye_[XYZ_Z].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, easeTime);
		//右カメラ注視点[i]
		easeDeadCameraTarget_[XYZ_X].SetEasing(nowTarget_.x, nowTarget_.x + easeOffset_.x, easeTime);
		easeDeadCameraTarget_[XYZ_Y].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, easeTime);
		easeDeadCameraTarget_[XYZ_Z].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, easeTime);

		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraEye_[i].Standby(false);
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraTarget_[i].Standby(false);

		isBreak_ = true;
		isAlive_ = false;
	}
	const float deadPosY = -60.0;
	if (position_.y <= deadPosY)isDead_ = true;

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
		
		const int32_t shakeCount = 1;
		//視点シェイク
		XMFLOAT3 Eye = nowEye_ + hitMove_;
		const XMFLOAT3 hitEye = { 1.0f,1.0f,1.0f };
		camera_->ShakeEye(Eye, shakeCount, { Eye.x - hitEye.x,Eye.y - hitEye.y,Eye.z - hitEye.z },
			{ Eye.x + hitEye.x,Eye.y + hitEye.y,Eye.z + hitEye.z });

		//注視点シェイク
		XMFLOAT3 Target = nowTarget_ + hitMove_;
		const XMFLOAT3 hitTarget = { 1.0f,1.0f,1.0f };
		camera_->ShakeTarget(Target, shakeCount, { Target.x - hitTarget.x,Target.y - hitTarget.y,Target.z - hitTarget.z },
			{ Target.x + hitTarget.x,Target.y + hitTarget.y,Target.z + hitTarget.z });
		camera_->Update();

		//+してイージング
		easeHit_.ease_in_out_cubic();
		spriteHit_->SetColor({ hitColor_.x,hitColor_.y,hitColor_.z ,easeHit_.num_X });

		mutekiCount_++;
	}

	if (mutekiCount_ == MUTEKI_COUNT)
	{
		if (isShake_)
		{
			camera_->SetEye(nowEye_ + hitMove_);
			camera_->SetTarget(nowTarget_ + hitMove_);
			isShake_ = false;
		}
		else isHit_ = false;
		//model_ = modelPlayer_;
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
		easeExplosionSizeAndAlpha_[XYW_W].ease_out_sine();
		spriteExplosion_->SetColor({ asIsColor_.x,asIsColor_.y,asIsColor_.z,
			easeExplosionSizeAndAlpha_[XYW_W].num_X });
		//炎プリセット
		const ParticleManager::Preset fire =
		{
			particleFire_.get(),
			{position_.x,position_.y - 5.0f,position_.z},
			{ 15.0f ,15.0f,15.0f },
			{ 3.3f,3.3f,3.3f },
			{ 0.0f,0.001f,0.0f },
			5,
			{ 7.0f, 0.0f },
			{ MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		//大爆発
		pmFire_->SetBlendMode(ParticleManager::BP_ADD);
		pmFire_->ActiveY(fire.particle, fire.startPos, fire.pos, fire.vel,
			fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);

		//煙プリセット
		const ParticleManager::Preset smoke =
		{
			particleSmoke_.get(),
			{position_.x,position_.y + 5.0f,position_.z},
			{ 25.0f ,10.0f,15.0f },
			{ MyMath::RandomMTFloat(0.0f,0.1f),MyMath::RandomMTFloat(0.5f,3.0f),0.3f },
			{ 0.0f,0.001f,0.0f },
			4,
			{ 4.0f, 0.0f },
			{ MyMath::RandomMTFloat(0.8f,1.0f),MyMath::RandomMTFloat(0.8f,1.0f),MyMath::RandomMTFloat(0.95f,1.0f),1.0f },
			{ 1.0f,1.0f,1.0f,0.0f }
		};

		//煙も舞う
		pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
		pmSmoke_->ActiveY(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		//カメラ速度
		XMFLOAT3 cameraSppedEyeTarget;
		const XMFLOAT2 shakeEyeTargetMinMax = { -0.1f,0.1f };
		cameraSppedEyeTarget.x = MyMath::RandomMTFloat(shakeEyeTargetMinMax.x, shakeEyeTargetMinMax.y);
		cameraSppedEyeTarget.y = MyMath::RandomMTFloat(shakeEyeTargetMinMax.x, shakeEyeTargetMinMax.y);
		const float speedChangePosZ = -30.0f;
		const XMFLOAT2 cameraSpeedZ = { 2.5f,1.0f };//奥側と手前側
		if (position_.z >= speedChangePosZ) cameraSppedEyeTarget.z = cameraSpeedZ.x;
		else cameraSppedEyeTarget.z = cameraSpeedZ.y;

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
	else if (isCameraEnd_)
	{
		//爆発
		for (int i = 0; i < XY_Num; i++)easeExplosionSizeAndAlpha_[i].ease_out_expo();

		spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[XYW_X].num_X,easeExplosionSizeAndAlpha_[XYW_Y].num_X });

		if (spriteExplosion_->GetSize().x == easeExplosionSizeAndAlpha_[XYW_X].end)
		{
			easeExplosionSizeAndAlpha_[XYW_W].Standby(false);//カラーだけ
			isExplosion_ = true;
			isCameraEnd_ = false;
		}
	}
	else
	{
		//カメラ移動
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraEye_[i].ease_out_cubic();
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraTarget_[i].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[XYZ_X].num_X,easeDeadCameraEye_[XYZ_Y].num_X, easeDeadCameraEye_[XYZ_Z].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[XYZ_X].num_X,easeDeadCameraTarget_[XYZ_Y].num_X, easeDeadCameraTarget_[XYZ_Z].num_X });
		if (camera_->GetEye().z == easeDeadCameraEye_[XYZ_Z].end)
		{
			for (int i = 0; i < XY_Num; i++)easeExplosionSizeAndAlpha_[i].Standby(false);//サイズだけ
			isCameraEnd_ = true;
		}
	}

}

void Player::UpdateGoal()
{
	//座標を固定してスケールをイージング
	position_ = stopPos_;
	for (int i = 0; i < XYZ_Num; i++)easeChangeScaleStageClear_[i].ease_out_cubic();
	scale_ = { easeChangeScaleStageClear_[XYZ_X].num_X,easeChangeScaleStageClear_[XYZ_Y].num_X ,easeChangeScaleStageClear_[XYZ_Z].num_X };

}
