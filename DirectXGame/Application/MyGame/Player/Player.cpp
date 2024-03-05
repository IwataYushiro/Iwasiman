#include "Player.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "GamePlayScene.h"
#include "EnumList.h"
#include "MyMath.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	Player.cpp

*	自機

*/

//静的メンバ変数の実体
IwasiEngine::CollisionManager* Player::colManager_ = CollisionManager::GetInstance();

Player::~Player() {

}

std::unique_ptr<Player> Player::Create(const PlayerModelList* model, GamePlayScene* gamescene)
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
	if (model->playerModel)ins->modelPlayer_ = model->playerModel;		//通常立ち姿
	if (model->playerBullet) ins->modelBullet_ = model->playerBullet;	//弾
	if (model->playerHit)ins->modelHit_ = model->playerHit;				//ヒット時
	if (model->playerMove)ins->modelMove_ = model->playerMove;			//移動時
	if (model->playerJump)ins->modelJump_ = model->playerJump;			//ジャンプ時
	if (model->playerAttack)ins->modelAttack_ = model->playerAttack;	//攻撃時
	//ゲームシーンのセット
	if (gamescene)ins->SetGameScene(gamescene);

	//最初のモデル
	ins->SetModel(model->playerMove);
	return ins;
}

bool Player::Initialize() {

	if (!Object3d::Initialize()) return false;

	//シングルトンインスタンス
	spCommon_ = SpriteCommon::GetInstance();
	input_ = Input::GetInstance();
	//初期値セット
	const int32_t startLife = 10;	//開始時ライフ
	life_ = startLife;				//→ライフを代入
	isDead_ = false;				//死亡フラグ
	isHit_ = false;					//命中フラグ
	mutekiCount_ = 0;				//無敵時間

	//右を向いているか
	isRight_ = true;
	//ジャンプしたか
	onGround_ = true;
	//ジャンプ力
	const float startJumpVYFist = 2.0f;
	jumpVYFist_ = startJumpVYFist;

	//奥側にいるか
	isJumpBack_ = false;
	//奧にいるか
	isBack_ = false;
	//初期は右向き
	isRight_ = true;

	//奥側ジャンプに使うベジェ曲線用の時間
	startCount_ = std::chrono::steady_clock::now();	//開始時間
	nowCount_ = std::chrono::steady_clock::now();		//現在時間
	elapsedCount_;	//経過時間 経過時間=現在時間-開始時間

	//スプライト
	//ライフバー
	spCommon_->LoadTexture(GPSPTI_PlayerLifeBarTex, "texture/plife2.png");
	spriteLifeBar_->Initialize(spCommon_, GPSPTI_PlayerLifeBarTex);
	spriteLifeBar_->SetPosition(lifeBarPos_);
	spriteLifeBar_->SetColor(green_);//基本は緑
	spriteLifeBar_->Update();
	//被弾演出
	spCommon_->LoadTexture(GPSPTI_PlayerHitEffectTex, "texture/fade2.png");
	spriteHit_->Initialize(spCommon_, GPSPTI_PlayerHitEffectTex);
	spriteHit_->SetColor(hitColor_);//色は赤いが基本は透明
	spriteHit_->Update();
	//爆発用スプライト
	spCommon_->LoadTexture(GPSPTI_PlayerExplosionTex, "texture/explosion2.png");
	spriteExplosion_->Initialize(spCommon_, GPSPTI_PlayerExplosionTex);
	spriteExplosion_->SetAnchorPoint(explosionAnchorPoint_);
	spriteExplosion_->SetPosition(explosionPos_);
	spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[XYW_X].start,easeExplosionSizeAndAlpha_[XYW_Y].start });
	spriteExplosion_->SetColor({ asIsColor_.x,asIsColor_.y,asIsColor_.z,easeExplosionSizeAndAlpha_[XYW_W].start });
	spriteExplosion_->Update();

	//パーティクル
	//爆発時の煙
	particleSmoke_ = Particle::LoadFromParticleTexture("particle9.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());
	pmSmoke_->SetBlendMode(ParticleManager::BP_ALPHA);
	//ダッシュ、爆発用の炎
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());
	//弾を撃つときの軌跡
	particleBullet_ = Particle::LoadFromParticleTexture("kaze.png");
	pmBullet_ = ParticleManager::Create();
	pmBullet_->SetParticleModel(particleBullet_.get());

	//コライダー追加
	const XMVECTOR colliderOffset = { -radius_,0.0f,0.0f,0.0f };//初期値は右方向
	SetCollider(new SphereCollider(colliderOffset, radius_));
	//自機本体
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	//イージングスタンバイ
	easelifeBarSize_.Standby(false);
	return true;
}

void Player::Reset() {
	//値リセット
	const int32_t resetLife = 10;	//初期値ライフにリセット
	life_ = resetLife;				//それを代入
	isDead_ = false;				//死亡フラグ

	//右を向いているか
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

	//パーティクルマネージャーにカメラをセット
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	pmBullet_->SetCamera(camera_);

	if (!isStart)//スタート演出時は何もしない
	{
		if (isAlive_)UpdateAlive(isBack, isAttack);		//生存時
		else if (isBreak_)UpdateBreak();				//撃破時
		else if (isGoal_)UpdateGoal();					//ゴール時
	}
	else isAlive_ = true;								//最初は生きてないがスタート演出時に生存扱いする

	//更新
	camera_->Update();		//カメラ
	UpdateWorldMatrix();	//行列
	pmFire_->Update();		//パーティクルマネージャー(炎)
	pmSmoke_->Update();		//パーティクルマネージャー(煙)
	pmBullet_->Update();	//パーティクルマネージャー（弾の軌跡）
	collider_->Update();	//コライダー


	//着地処理
	Landing(COLLISION_ATTR_LANDSHAPE);

	//ライフバーのイージング
	easelifeBarSize_.ease_in_cubic();
	lifeBarDamageSize_.x = easelifeBarSize_.num_X;//サイズをセット
	spriteLifeBar_->SetTextureSize({ lifeBarDamageSize_.x * life_,lifeBarDamageSize_.y });
	spriteLifeBar_->SetSize({ lifeBarDamageSize_.x * life_,lifeBarDamageSize_.y });

	//ピンチ時はライフバーを赤くし、それ以外は緑に
	if (life_ <= dangerLifeZone_)spriteLifeBar_->SetColor(red_);
	else spriteLifeBar_->SetColor(green_);
	//スプライト更新
	spriteLifeBar_->Update();
	spriteHit_->Update();
	spriteExplosion_->Update();
}

void Player::Draw() { if (!isExplosion_)Object3d::Draw(); }//描画

void Player::DrawSprite()
{
	if (!isBreak_)//生きてたら
	{
		if (isAlive_)spriteLifeBar_->Draw();
		spriteHit_->Draw();
	}
	else//やられたら
	{
		spriteExplosion_->Draw();
	}
}

void Player::DrawParticle() {
	//各パーティクル描画
	pmSmoke_->Draw();
	pmFire_->Draw();
}

//移動処理
void Player::Move() {

	//振り向くためのイージング
	easeRotateRightY_.ease_out_cubic();
	//値のゲッター
	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 rot = Object3d::GetRotation();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();

	//パーティクル
	const XMFLOAT3 startPosRight = { position_.x - 2.0f,position_.y + 1.0f ,position_.z };
	const XMFLOAT3 startPosLeft = { position_.x + 2.0f,position_.y + 1.0f ,position_.z };
	const ParticleManager::Preset fire =	//炎プリセット
	{
		particleFire_.get(),
		position_,//使わない
		{ 0.0f ,2.0f,0.0f },
		{ 3.0f,0.3f,0.3f },
		{ 0.0f,0.001f,0.0f },
		2,
		{ 1.0f, 0.0f },
		{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
		{0.0f,0.0f,0.0f,1.0f}
	};
	//逆方向時の速度
	const XMFLOAT3 reverseParticleVel = { -fire.vel.x,-fire.vel.y, -fire.vel.z };
	//通常移動時に出すパーティクルの量
	const int32_t walkParticleNum = 1;
	//通常移動時に出すパーティクルの色合い
	const XMFLOAT4 walkStartColor = { MyMath::RandomMTFloat(0.9f,1.0f),0.6f,0.6f,1.0f };

	//球コライダーの取得
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	assert(sphereCollider);
	//球コライダーのオフセットも取得
	XMVECTOR colliderOffset = sphereCollider->GetOffset();

	//左右コライダーオフセット
	const XMVECTOR limitColliderOffsetRight = { -radius_,0.0f,0.0f,0.0f };		//右方向
	const XMVECTOR limitColliderOffsetLeft = { radius_,0.0f,0.0f,0.0f };		//左方向

	//キーボード入力による移動処理
	XMMATRIX matTrans = XMMatrixIdentity();

	//ダッシュ
	if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
	{
		//左にダッシュ移動
		if (input_->PushKey(DIK_A)) {
			//カウントリセット
			activeCount_ = 0.0f;
			//操作している扱いに
			isActive_ = true;
			if (isRight_)
			{
				easeRotateRightY_.Standby(true);
				rot.y = easeRotateRightY_.end;
				isRight_ = false;
			}
			if (onGround_)model_ = modelMove_;//地面についてたらモデルを切り替える
			//パーティクルを出す
			pmFire_->ActiveX(fire.particle, startPosLeft, fire.pos, fire.vel,
				fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
			//移動
			move.x -= moveSpeed_ * dashSpeed_;
			cmove.x -= moveSpeed_ * dashSpeed_;
			tmove.x -= moveSpeed_ * dashSpeed_;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x -= moveSpeed_ * dashSpeed_;
			//オフセットは逆に動かす
			colliderOffset.m128_f32[XYZ_X] += moveSpeed_ * dashSpeed_;
		}
		//右にダッシュ移動
		else if (input_->PushKey(DIK_D)) {
			//カウントリセット
			activeCount_ = 0.0f;
			//操作している扱いに
			isActive_ = true;
			if (!isRight_)
			{
				easeRotateRightY_.Standby(false);
				rot.y = easeRotateRightY_.start;
				isRight_ = true;
			}
			if (onGround_)model_ = modelMove_;//地面についてたらモデルを切り替える
			//パーティクルを出す
			pmFire_->ActiveX(fire.particle, startPosRight, fire.pos, reverseParticleVel,
				fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
			//移動
			move.x += moveSpeed_ * dashSpeed_;
			cmove.x += moveSpeed_ * dashSpeed_;
			tmove.x += moveSpeed_ * dashSpeed_;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x += moveSpeed_ * dashSpeed_;
			//オフセットは逆に動かす
			colliderOffset.m128_f32[XYZ_X] -= moveSpeed_ * dashSpeed_;
		}
	}
	else//通常移動
	{
		//左に通常移動
		if (input_->PushKey(DIK_A)) {
			//カウントリセット
			activeCount_ = 0.0f;
			//操作している扱いに
			isActive_ = true;
			if (isRight_)
			{
				easeRotateRightY_.Standby(true);
				rot.y = easeRotateRightY_.end;
				isRight_ = false;
			}
			if (onGround_)model_ = modelMove_;//地面についてたらモデルを切り替える
			//パーティクルを出す
			pmFire_->ActiveX(fire.particle, startPosLeft, fire.pos, fire.vel,
				fire.acc, walkParticleNum, fire.scale, walkStartColor, fire.endColor);
			//移動
			move.x -= moveSpeed_;
			cmove.x -= moveSpeed_;
			tmove.x -= moveSpeed_;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x -= moveSpeed_;
			//オフセットは逆に動かす
			colliderOffset.m128_f32[XYZ_X] += moveSpeed_;

		}
		//右に通常移動
		else if (input_->PushKey(DIK_D)) {
			//カウントリセット
			activeCount_ = 0.0f;
			//操作している扱いに
			isActive_ = true;
			if (!isRight_)
			{
				easeRotateRightY_.Standby(false);
				rot.y = easeRotateRightY_.start;
				isRight_ = true;
			}
			if (onGround_)model_ = modelMove_;//地面についてたらモデルを切り替える
			//パーティクルを出す
			pmFire_->ActiveX(fire.particle, startPosRight, fire.pos, reverseParticleVel,
				fire.acc, walkParticleNum, fire.scale, walkStartColor, fire.endColor);
			//移動
			move.x += moveSpeed_;
			cmove.x += moveSpeed_;
			tmove.x += moveSpeed_;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x += moveSpeed_;
			//オフセットは逆に動かす
			colliderOffset.m128_f32[XYZ_X] -= moveSpeed_ * dashSpeed_;

		}
	}

	//上限(左)
	if (colliderOffset.m128_f32[XYZ_X] >= limitColliderOffsetLeft.m128_f32[XYZ_X])
	{
		colliderOffset.m128_f32[XYZ_X] = limitColliderOffsetLeft.m128_f32[XYZ_X];
	}
	//上限(右)
	else if (colliderOffset.m128_f32[XYZ_X] <= limitColliderOffsetRight.m128_f32[XYZ_X])
	{
		colliderOffset.m128_f32[XYZ_X] = limitColliderOffsetRight.m128_f32[XYZ_X];
	}

	//値の更新
	Object3d::SetPosition(move);
	Object3d::SetRotation(rot);
	camera_->SetEye(cmove);
	camera_->SetTarget(tmove);
	sphereCollider->SetOffset(colliderOffset);
}

void Player::FallAndJump()
{
	//ジャンプ力強化アイテムを取っているかいないかでジャンプ力が変わる
	const float jumpPowerUp = 3.0f;			//強化時
	const float jumpPowerDefault = 2.0f;	//通常時
	if (isGetJumpItem_)
	{
		if (onGround_)jumpVYFist_ = jumpPowerUp;
		jumpPowerUpcount_++;
	}
	else
	{
		if (onGround_)jumpVYFist_ = jumpPowerDefault;
	}

	if (!onGround_)//ジャンプ中
	{
		//モデルを変更
		model_ = modelJump_;
		//操作している扱いに
		isActive_ = true;
		//カウントリセット
		activeCount_ = 0.0f;
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
		//地面に着いているときにスペースキーでジャンプ
		onGround_ = false;
		const XMFLOAT3 startJumpVec = { 0.0f,jumpVYFist_,0.0f };
		fallVec_ = startJumpVec;
	}

	//強化アイテムを取得しているとき一定の時間がたったらジャンプ力がリセットされる
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

	if (onGround_)//地面に着いていたら
	{
		if (!isJumpBack_)
		{
			if (input_->TriggerKey(DIK_W))//奥側へジャンプ
			{

				if (isBack_)return;
				startCount_ = std::chrono::steady_clock::now();
				jumpBackPos_ = position_;
				isBack_ = true;
				isJumpBack_ = true;
			}
			if (input_->TriggerKey(DIK_S))//手前側へジャンプ
			{
				if (!isBack_)return;
				startCount_ = std::chrono::steady_clock::now();
				jumpBackPos_ = position_;
				isBack_ = false;
				isJumpBack_ = true;
			}
		}
	}
	if (isJumpBack_)//奥側、手前側ジャンプ中
	{
		//カウントリセット
		activeCount_ = 0.0f;
		//操作している扱いに
		isActive_ = true;
		//現在時間を取得する
		nowCount_ = std::chrono::steady_clock::now();
		//前回記録からの経過時間を取得する
		elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

		const float micro = 1'000'000.0f;
		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//マイクロ秒を秒に単位変換
		//0~1
		const float timeRateMax = 1.0f;
		timeRate_ = min(elapsed / maxTime_, timeRateMax);

		if (isBack_)//奥側へ行く場合
		{
			//ベジェ曲線で飛んでいく
			position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);
			if (position_.z >= end_.z)isJumpBack_ = false;
		}
		else//手前側へ行く場合
		{
			//ベジェ曲線で飛んでいく
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

	//自機専用クエリーコールバッククラス定義
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		//コンストラクタ
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
		//球
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

	//視点と注視点をゲット
	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();
	//X分だけ動かす
	eyepos.x += callback.move.m128_f32[XYZ_X];
	tarpos.x += callback.move.m128_f32[XYZ_X];

	//更新
	UpdateWorldMatrix();			//行列更新
	camera_->SetEye(eyepos);		//視点セット
	camera_->SetTarget(tarpos);		//注視点セット
	collider_->Update();			//コライダー更新

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
			//着地時にモデルを着地モデルに
			model_ = modelPlayer_;
			//着地
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);


			//着地プリセット
			const ParticleManager::Preset smoke =
			{
				particleSmoke_.get(),
				position_,
				{ radius_ ,-radius_ * 2.0f,radius_ },
				{ 0.2f,0.05f,0.2f },
				{ 0.0f,0.001f,0.0f },
				MyMath::RandomMTInt(15,20),
				{ 1.0f, 0.0f },
				{ 1.0f,1.0f,1.0f,0.5f },
				{ 0.0f,0.0f,0.0f,0.0f }
			};
			//着地したら土煙っぽいのを出す
			pmSmoke_->ActiveY(smoke);

			//行列更新
			Object3d::Update();
		}
	}

	//オブジェクト更新
	Object3d::Update();
}

//攻撃処理
void Player::Attack() {

	if (input_->TriggerKey(DIK_L)) {
		//猶予
		const float graceCountNum = 30.0f;
		//カウントに猶予を持たせてリセット
		activeCount_ = 0.0f - graceCountNum;
		//操作している扱いに
		isActive_ = true;

		//地面にいるとき,通常の立ち絵のとき限定でモデルを変える
		if (model_ == modelPlayer_ && onGround_) model_ = modelAttack_;
		//弾の速度
		const float bulletSpeed = 1.0f;
		XMFLOAT3 velocity;
		//向きによってスピードが変わる
		const XMFLOAT3 velLeft = { -bulletSpeed, 0.0f, 0.0f };
		const XMFLOAT3 velRight = { bulletSpeed, 0.0f, 0.0f };
		//向いている方向を同期
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
		const XMFLOAT3 bulletPositionOffsetRight = { position_.x + 0.8f,position_.y + 3.0f,position_.z };
		const XMFLOAT3 bulletPositionOffsetLeft = { position_.x - 0.8f,position_.y + 3.0f,position_.z };
		std::unique_ptr<PlayerBullet> newBullet;
		if (isRight_)newBullet = PlayerBullet::Create(bulletPositionOffsetRight, velocity, modelBullet_, particleBullet_.get(), pmBullet_.get());
		else newBullet = PlayerBullet::Create(bulletPositionOffsetLeft, velocity, modelBullet_, particleBullet_.get(), pmBullet_.get());
		newBullet->SetCamera(camera_);
		newBullet->Update();

		//弾を登録
		gameScene_->AddPlayerBullet(std::move(newBullet));
		//あるメモリの所有権を持つunique_ptrはただ一つしか存在できない
		//その所有権を謙渡するための機能が std::move()
	}
}

//衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision([[maybe_unused]] const CollisionInfo& info,
	const unsigned short attribute, const unsigned short subAttribute) {

	//ダメージ管理の構造体
	struct DamageType
	{
		const uint32_t enemyNone = 2;
		const uint32_t enemyPower = 4;
		const uint32_t enemyGuard = 1;
		const uint32_t enemySpeed = 1;
		const uint32_t enemyDeath = 15;
		const uint32_t enemyBullet = 1;
		const uint32_t GimmickSpike = 3;
	};
	DamageType damege;

	//煙プリセット
	const ParticleManager::Preset fire =
	{
		particleFire_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		30,
		{ 3.0f, 0.0f },
		{ 1.0f,0.4f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_ENEMYS)//敵の場合
	{
		//ヒット時(無敵時間時)は効かない
		if (isShake_)return;
		if (isHit_)return;
		//ダメージ計算
		if (subAttribute == SUBCOLLISION_ATTR_NONE)life_ -= damege.enemyNone;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_POWER)life_ -= damege.enemyPower;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_GUARD)life_ -= damege.enemyGuard;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_SPEED)life_ -= damege.enemySpeed;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_DEATH)life_ -= damege.enemyDeath;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_ISDEAD)return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_ -= damege.enemyBullet;

		//ヒット演出
		pmFire_->ActiveZ(fire);
		pmFire_->Update();
		//model_ = modelHit_;
		isHit_ = true;

		//ピンチ時はポストエフェクトの色は危険色に
		if (life_ <= dangerLifeZone_)
		{
			//ポストエフェクトはビネットに変更
			gameScene_->ChangePostEffect("Vignette");
			//カラーもチェンジ
			const XMFLOAT4 dangerColor = { 1.0f,0.1f * life_,0.1f * life_,1.0f };
			gameScene_->ChangePostEffectColor(dangerColor);
		}

	}

	else if (attribute == COLLISION_ATTR_GIMMICK)//ギミックの場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_GIMMICK_SPIKE)//トゲ
		{
			//ヒット時(無敵時間時)は効かない
			if (isShake_)return;
			if (isHit_)return;
			//ダメージ計算
			life_ -= damege.GimmickSpike;

			//ヒット演出
			pmFire_->ActiveZ(fire.particle, fire.startPos, fire.pos, fire.vel,
				fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
			pmFire_->Update();

			//model_ = modelHit_;
			isHit_ = true;

			//ピンチ時はポストエフェクトの色は危険色に
			if (life_ <= dangerLifeZone_)
			{
				//ポストエフェクトはビネットに変更
				gameScene_->ChangePostEffect("Vignette");
				//カラーもチェンジ
				const XMFLOAT4 dangerColor = { 1.0f,0.2f * life_,0.2f * life_,1.0f };
				gameScene_->ChangePostEffectColor(dangerColor);
			}

		}

	}
	else if (attribute == COLLISION_ATTR_GOAL)//ゴールの場合
	{
		//多重ヒットを防止
		if (isGoal_)return;
		//自機のスケールをイージングで変更
		scale_ = { easeChangeScaleStageClear_[XYZ_X].start,easeChangeScaleStageClear_[XYZ_Y].start ,easeChangeScaleStageClear_[XYZ_Z].start };
		for (int i = 0; i < XYZ_Num; i++)easeChangeScaleStageClear_[i].Standby(false);
		//動かせないようにする
		stopPos_ = position_;
		isGoal_ = true;
		isAlive_ = false;
	}
	else if (attribute == COLLISION_ATTR_ITEM)//アイテムの場合
	{

		if (subAttribute == SUBCOLLISION_ATTR_ITEM_JUMP)//ジャンプ強化アイテム
		{
			if (isGetJumpItem_)
			{
				jumpPowerUpcount_ = 0;//時間をリセット
				return; //多重ヒット防止
			}

			isGetJumpItem_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_ITEM_HEAL)//ライフ回復アイテム
		{
			//回復する前のライフがちょうどピンチ状態から脱却するとき（ライフ3のとき）
			if (life_ == dangerLifeZone_)
			{
				//回復アイテム取得時にだけポストエフェクトも切り替える
				gameScene_->ChangePostEffect("None");
				//ポストエフェクトの色も変更
				const XMFLOAT4 resetPostEffectColor = { 1.0f,1.0f,1.0f,1.0f };
				gameScene_->ChangePostEffectColor(resetPostEffectColor);
			}
			//ピンチ時はポストエフェクトの色を変える
			else
			{
				//カラーもチェンジ
				const XMFLOAT4 dangerColor = { 1.0f,0.2f * life_,0.2f * life_,1.0f };
				gameScene_->ChangePostEffectColor(dangerColor);
			}
			//回復する
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
	isActive_ = false;
	//移動処理
	Move();
	//ジャンプ処理
	FallAndJump();
	//手前。奥側ジャンプ処理
	if (isBack)JumpBack();
	//攻撃処理
	if (isAttack)Attack();

	if (life_ <= 0)//ライフが0になったら死ぬ
	{
		//現在視点と注視点を記録
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
		//イージングスタンバイ
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraEye_[i].Standby(false);
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraTarget_[i].Standby(false);

		//ポストエフェクトも切り替える
		gameScene_->ChangePostEffect("Vignette");

		isBreak_ = true;
		isAlive_ = false;


	}
	//一定の位置まで落ちても死ぬ
	const float deadPosY = -60.0;
	if (position_.y <= deadPosY)isDead_ = true;

	if (isHit_)//敵の攻撃が当たったら
	{
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();
		easeHit_.Standby(false);
		isShake_ = true;
		isHit_ = false;
	}
	if (isShake_)
	{
		//当たったらシェイクさせる
		const int32_t shakeCount = 1;
		//視点シェイク
		XMFLOAT3 Eye = nowEye_ + hitMove_;
		const XMFLOAT3 hitEye = { 0.5f,0.5f,0.5f };
		camera_->ShakeEye(Eye, shakeCount, { Eye.x - hitEye.x,Eye.y - hitEye.y,Eye.z - hitEye.z },
			{ Eye.x + hitEye.x,Eye.y + hitEye.y,Eye.z + hitEye.z });

		//注視点シェイク
		XMFLOAT3 Target = nowTarget_ + hitMove_;
		const XMFLOAT3 hitTarget = { 0.5f,0.5f,0.5f };
		camera_->ShakeTarget(Target, shakeCount, { Target.x - hitTarget.x,Target.y - hitTarget.y,Target.z - hitTarget.z },
			{ Target.x + hitTarget.x,Target.y + hitTarget.y,Target.z + hitTarget.z });
		camera_->Update();

		//+してイージング
		easeHit_.ease_in_out_cubic();

		//ライフがある場合のみモデルの色を変更
		const XMFLOAT4 hitColorObject = { 1.0f - easeHit_.num_X,1.0f - easeHit_.num_X,1.0f,1.0f - (easeHit_.num_X / 2.0f) };
		if (life_ > 0)SetColor(hitColorObject);

		//ヒット時のスプライトを出す
		spriteHit_->SetColor({ hitColor_.x,hitColor_.y,hitColor_.z ,easeHit_.num_X });
		//無敵時間カウントを進める
		mutekiCount_++;
	}
	//無敵時間カウントが一定の時間にまで達したらシェイク終了
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
	//カウントを進める
	activeCount_++;
	//最大値まで達したら操作してない扱いとする
	if (activeCount_ >= maxActiveCount_)
	{
		//時間を固定化
		activeCount_ = maxActiveCount_;
		//モデルを立ち絵に
		model_ = modelPlayer_;
		//操作してない扱いにする
		isActive_ = false;
	}
#ifdef _DEBUG
	//デバッグ用

#endif // _DEBUG
}

void Player::UpdateBreak()
{
	if (isExplosion_)//スプライト爆発し終わった時
	{
		//イージングで徐々に爆発スプライトを薄くしていき最終的には透明にする
		easeExplosionSizeAndAlpha_[XYW_W].ease_out_sine();
		spriteExplosion_->SetColor({ asIsColor_.x,asIsColor_.y,asIsColor_.z,easeExplosionSizeAndAlpha_[XYW_W].num_X });
		//炎プリセット
		const ParticleManager::Preset fire =
		{
			particleFire_.get(),
			{position_.x,position_.y - 5.0f,position_.z},
			{ 10.0f ,10.0f,10.0f },
			{ 3.3f,3.3f,3.3f },
			{ 0.0f,0.001f,0.0f },
			4,
			{ 7.0f, 0.0f },
			{ MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		//大爆発
		pmFire_->ActiveY(fire);

		const DirectX::XMFLOAT2 smokeOffsetXY = { -2.0f,-1.5f };//煙初期座標オフセット
		//煙プリセット
		const ParticleManager::Preset smoke =
		{
			particleSmoke_.get(),
			{position_.x + smokeOffsetXY.x,position_.y + smokeOffsetXY.y,position_.z},
			{ 3.0f ,4.0f,0.0f },
			{ 0.5f,2.3f,0.0f },
			{ 0.0f,0.001f,0.0f },
			2,
			{ 1.0f, 5.0f },
			{0.5f,0.5f,0.5f,1.0f },
			{ 0.0f,0.0f,0.0f,0.0f }
		};

		//煙も舞う
		pmSmoke_->ActiveY(smoke);

		//カメラ速度
		XMFLOAT3 cameraSppedEyeTarget;
		const XMFLOAT2 shakeEyeTargetMinMax = { -0.1f,0.1f };//視点、注視点のXY座標はシェイクさせる
		//シェイクの値はランダム
		cameraSppedEyeTarget.x = MyMath::RandomMTFloat(shakeEyeTargetMinMax.x, shakeEyeTargetMinMax.y);
		cameraSppedEyeTarget.y = MyMath::RandomMTFloat(shakeEyeTargetMinMax.x, shakeEyeTargetMinMax.y);

		const float speedChangePosZ = -30.0f;//手前側、奥側に分ける基準の値(これより大きかったら奥側)
		const XMFLOAT2 cameraSpeedZ = { 1.0f,0.5f };//奥側と手前側の視点、注視点のZ軸移動の速度
		//奥側と手前側でズームアウト速度を変える
		//奥側
		if (position_.z >= speedChangePosZ) cameraSppedEyeTarget.z = cameraSpeedZ.x;
		//手前側
		else cameraSppedEyeTarget.z = cameraSpeedZ.y;

		//徐々にカメラはズームアウト
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
		//Z視点が一定の位置に達したらゲームオーバー
		if (camera_->GetEye().z <= -cameraEyeChangeGameover_) isDead_ = true;
	}
	else if (isCameraEnd_)//カメラ移動終わったとき
	{
		//爆発
		for (int i = 0; i < XY_Num; i++)easeExplosionSizeAndAlpha_[i].ease_out_expo();
		//スプライトで爆発を表現
		spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[XYW_X].num_X,easeExplosionSizeAndAlpha_[XYW_Y].num_X });
		//サイズがイージング終了サイズまで達したらパーティクルの爆発、煙の出番
		if (spriteExplosion_->GetSize().x == easeExplosionSizeAndAlpha_[XYW_X].end)
		{
			easeExplosionSizeAndAlpha_[XYW_W].Standby(false);//カラーだけ
			isExplosion_ = true;
			isCameraEnd_ = false;
		}
	}
	else//ライフが0になった瞬間
	{
		//死亡時モデルに切り替える
		model_ = modelHit_;
		//カメラ移動
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraEye_[i].ease_out_cubic();
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraTarget_[i].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[XYZ_X].num_X,easeDeadCameraEye_[XYZ_Y].num_X, easeDeadCameraEye_[XYZ_Z].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[XYZ_X].num_X,easeDeadCameraTarget_[XYZ_Y].num_X, easeDeadCameraTarget_[XYZ_Z].num_X });
		//カメラのイージングが終わったら爆発
		if (camera_->GetEye().z == easeDeadCameraEye_[XYZ_Z].end)
		{
			for (int i = 0; i < XY_Num; i++)easeExplosionSizeAndAlpha_[i].Standby(false);//サイズだけ
			isCameraEnd_ = true;
		}
	}
	//ポストエフェクトの色は死亡色に
	const XMFLOAT4 deathColor = { 1.0f,MyMath::RandomMTFloat(0.0f,0.3f),MyMath::RandomMTFloat(0.0f,0.3f),1.0f };
	gameScene_->ChangePostEffectColor(deathColor);

	//スキップで死亡演出スキップ
	if (input_->TriggerKey(DIK_SPACE))isSkipDeadDirection_ = true;
}

void Player::UpdateGoal()
{
	//座標を固定してスケールをイージング
	position_ = stopPos_;
	for (int i = 0; i < XYZ_Num; i++)easeChangeScaleStageClear_[i].ease_out_cubic();
	scale_ = { easeChangeScaleStageClear_[XYZ_X].num_X,easeChangeScaleStageClear_[XYZ_Y].num_X ,easeChangeScaleStageClear_[XYZ_Z].num_X };

}
