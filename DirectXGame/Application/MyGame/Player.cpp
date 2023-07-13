#include "Player.h"
#include <cassert>

using namespace DirectX;

Player::~Player() {
	//モデルの解放

	delete modelBullet_;
	delete objBullet_;
	delete particleDash_;
	delete pmDash_;
}

Player* Player::Create(Model* model)
{
	//インスタンス生成
	Player* ins = new Player();
	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		delete ins;
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	return nullptr;
}

bool Player::Initialize() {

	/*modelBullet_ = Model::LoadFromOBJ("playerbullet");
	objBullet_ = Object3d::Create();

	objBullet_->SetModel(modelBullet_);
	objBullet_->SetCamera(camera_);*/

	//シングルトンインスタンスを取得
	this->input_ = Input::GetInstance();

	//ワールド変換の初期化
	pos = { -20.0f,-10.0f,-60.0f };
	obj_->SetPosition(pos);
	//ジャンプしたか
	isJump = false;

	//奥側にいるか
	isJumpBack = false;
	//奧にいるか
	isBack = false;

	//奥側ジャンプに使うベジェ曲線用の時間
	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間
	elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	maxTime = 1.0f;					//全体時間
	timeRate;
	//重力
	gravity = 0.0f;

	//パーティクル
	particleDash_ = Particle::LoadFromParticleTexture("particle1.png");
	pmDash_ = ParticleManager::Create();
	pmDash_->SetParticleModel(particleDash_);
	pmDash_->SetCamera(camera_);

	return true;
}

void Player::Reset() {
	pos = { -20.0f, -10.0f, -60.0f };

	life_ = 5;
	isDead_ = false;
	//弾リセット
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Reset();
	}
}
void Player::Update() {

	if (!isDead_) {
		//死亡フラグの立った弾を削除
		bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) { return bullet->IsDead(); });

		//移動処理
		Move();
		//攻撃処理
		Jump();
		JumpBack();
		//Attack();

		//弾更新
		for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
			bullet->Update();
		}

		//移動制限
		Trans();
		
	}
	pmDash_->Update();
	obj_->Update();
}

void Player::Draw() {
	if (!isDead_) {
		obj_->Draw();

		//弾描画
		for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
			bullet->Draw();
		}

	}
}

void Player::DrawParticle()
{
	pmDash_->Draw();
}

//移動処理
void Player::Move() {

	XMFLOAT3 move = obj_->GetPosition();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	float moveSpeed = 0.5f;

	//キーボード入力による移動処理
	XMMATRIX matTrans = XMMatrixIdentity();
	if (input_->PushKey(DIK_A)) {
		move.x -= moveSpeed;
		cmove.x -= moveSpeed;
		tmove.x -= moveSpeed;
	}
	if (input_->PushKey(DIK_D)) {
		move.x += moveSpeed;
		cmove.x += moveSpeed;
		tmove.x += moveSpeed;
	}

	
	//ダッシュ
	if (input_->PushKey(DIK_LSHIFT)|| input_->PushKey(DIK_RSHIFT))
	{
		if (input_->PushKey(DIK_A)) {
			pmDash_->ActiveX(particleDash_, obj_->GetPosition(), {20.0f ,10.0f,0.0f}, {-4.2f,0.2f,0.0f}, {0.0f,0.001f,0.0f}, 1, {3.0f, 0.0f});
			move.x -= moveSpeed * 2.0f;
			cmove.x -= moveSpeed * 2.0f;
			tmove.x -= moveSpeed * 2.0f;
		}
		if (input_->PushKey(DIK_D)) {
			pmDash_->ActiveX(particleDash_, obj_->GetPosition(), { 20.0f ,10.0f,0.0f }, { 4.2f,0.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
			move.x += moveSpeed * 2.0f;
			cmove.x += moveSpeed * 2.0f;
			tmove.x += moveSpeed * 2.0f;
		}
	}
	/*if (input_->PushKey(DIK_W)) {
		move.y += moveSpeed;
	}
	if (input_->PushKey(DIK_S)) {
		move.y -= moveSpeed;
	}*/

	obj_->SetPosition(move);
	camera_->SetEye(cmove);
	camera_->SetTarget(tmove);
}

void Player::CameraMove()
{
	XMFLOAT3 move = obj_->GetPosition();
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
	
	obj_->SetPosition(move);
	camera_->SetEye(cmove);
	camera_->SetTarget(tmove);
}

void Player::Jump()
{

	XMFLOAT3 move = obj_->GetPosition();

	//キーボード入力による移動処理
	XMMATRIX matTrans = XMMatrixIdentity();
	if (!isJump)
	{
		if (input_->TriggerKey(DIK_SPACE)) {
			isJump = true;
			gravity = 0.0f;
		}
	}
	else
	{
		move.y += power + gravity;
		gravity -= 0.1f;

		if (gravity <= -4.0f)
		{
			gravity = -4.0f;
		}
		if (move.y <= -10.0f)
		{
			move.y = -10.0f;
			isJump = false;
		}
	}
	obj_->SetPosition(move);
}

void Player::JumpBack()
{
	XMFLOAT3 move = obj_->GetPosition();

	//制御点
	start = { move.x,-10.0f,-60.0f };
	p1 = { move.x,10.0f,-40.0f };
	p2 = { move.x,10.0f,-20.0f };
	end = { move.x,-10.0f,0.0f };

	//時間
	
	if (!isJump)
	{
		if (!isJumpBack)
		{
			if (input_->TriggerKey(DIK_Z))
			{
				if (isBack)isBack = false;
				else isBack = true;
				isJumpBack = true;
			}
		}
		else
		{
			//現在時間を取得する
			nowCount = std::chrono::steady_clock::now();
			//前回記録からの経過時間を取得する
			elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

			float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

			timeRate = min(elapsed / maxTime, 1.0f);

			if (isBack)move = Bezier3(end, p2, p1, start, timeRate);

			else move = Bezier3(start, p1, p2, end, timeRate);
			
			if (move.z >= end.z)
			{
				startCount = std::chrono::steady_clock::now();
				isJumpBack = false;
			}
			else if (move.z <= start.z)
			{
				startCount = std::chrono::steady_clock::now();
				isJumpBack = false;
			}
		}
	}
	obj_->SetPosition(move);
}

//攻撃処理
void Player::Attack() {

	if (input_->TriggerKey(DIK_SPACE)) {
		//弾の速度
		const float kBulletSpeed = 1.0f;
		XMFLOAT3 velocity(0.0f, 0.0f, kBulletSpeed);

		XMMATRIX matVec = XMMatrixIdentity();
		matVec.r[0].m128_f32[0] = velocity.x;
		matVec.r[0].m128_f32[1] = velocity.y;
		matVec.r[0].m128_f32[2] = velocity.z;
		matVec.r[0].m128_f32[3] = 0.0f;

		matVec *= obj_->GetWorld();

		//自キャラの座標をコピー
		XMFLOAT3 position = obj_->GetPosition();

		//弾を生成し初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(modelBullet_, objBullet_, position, velocity);

		//弾を登録
		bullets_.push_back(std::move(newBullet));
		//あるメモリの所有権を持つunique_ptrはただ一つしか存在できない
		//その所有権を謙渡するための機能が std::move()
	}
}

void Player::Trans() {

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

//ワールド座標を取得
XMFLOAT3 Player::GetWorldPosition() {

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = obj_->GetPosition().x;
	worldPos.y = obj_->GetPosition().y;
	worldPos.z = obj_->GetPosition().z;

	return worldPos;
}

//衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision() {
	life_--;
	if (life_ <= 0) {
		isDead_ = true;
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
