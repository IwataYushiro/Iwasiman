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

*	���@

*/

//�ÓI�����o�ϐ��̎���
CollisionManager* Player::colManager_ = CollisionManager::GetInstance();

Player::~Player() {
	//�X�v���C�g���
	delete spriteLifeBar_;
	delete spriteHit_;
	delete spriteExplosion_;

	//�p�[�e�B�N�����f���̉��
	delete particleSmoke_;
	delete pmSmoke_;

	delete particleFire_;
	delete pmFire_;
}

std::unique_ptr<Player> Player::Create(Model* model, Model* bullet, GamePlayScene* gamescene)
{
	//�C���X�^���X����
	std::unique_ptr<Player> ins = std::make_unique<Player>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	if (bullet) ins->modelBullet_ = bullet;
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

bool Player::Initialize() {

	if (!Object3d::Initialize()) return false;

	//�V���O���g���C���X�^���X
	spCommon_ = SpriteCommon::GetInstance();
	input_ = Input::GetInstance();

	life_ = 10;
	isDead_ = false;
	isHit_ = false;
	mutekiCount_ = 0;

	isRight_ = true;
	//�W�����v������
	onGround_ = true;

	//�����ɂ��邩
	isJumpBack_ = false;
	//���ɂ��邩
	isBack_ = false;

	//�����W�����v�Ɏg���x�W�F�Ȑ��p�̎���
	startCount_ = std::chrono::steady_clock::now();	//�J�n����
	nowCount_ = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount_;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	maxTime_ = 1.0f;					//�S�̎���

	//�X�v���C�g
	spCommon_->LoadTexture(GPSPTI_PlayerLifeBarTex, "texture/plife2.png");
	spriteLifeBar_->Initialize(spCommon_, GPSPTI_PlayerLifeBarTex);
	spriteLifeBar_->SetPosition(lifeBarPos_);
	spriteLifeBar_->SetColor(green_);//��{�͗�
	spriteLifeBar_->Update();

	spCommon_->LoadTexture(GPSPTI_PlayerHitEffectTex, "texture/fade2.png");
	spriteHit_->Initialize(spCommon_, GPSPTI_PlayerHitEffectTex);
	spriteHit_->SetColor(hitColor_);//�F�͐Ԃ�����{�͓���
	spriteHit_->Update();

	spCommon_->LoadTexture(GPSPTI_PlayerExplosionTex, "texture/explosion2.png");
	spriteExplosion_->Initialize(spCommon_, GPSPTI_PlayerExplosionTex);
	spriteExplosion_->SetAnchorPoint({ 0.5f,0.5f });
	spriteExplosion_->SetPosition(explosionPos_);
	spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[0].start,easeExplosionSizeAndAlpha_[1].start });
	spriteExplosion_->SetColor({ 1.0f,1.0f,1.0f,easeExplosionSizeAndAlpha_[2].start });
	spriteExplosion_->Update();
	
	//�p�[�e�B�N��
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_);

	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_);

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	return true;
}

void Player::Reset() {

	life_ = 10;
	isDead_ = false;

	isRight_ = true;
	//�W�����v������
	onGround_ = true;

	//�����ɂ��邩
	isJumpBack_ = false;
	//���ɂ��邩
	isBack_ = false;

	//�����W�����v�Ɏg���x�W�F�Ȑ��p�̎���
	startCount_ = std::chrono::steady_clock::now();	//�J�n����
	nowCount_ = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount_;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	maxTime_ = 1.0f;					//�S�̎���

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
	//���n����
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

//�ړ�����
void Player::Move() {

	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 rot = Object3d::GetRotation();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	float moveSpeed = 0.5f;

	//�L�[�{�[�h���͂ɂ��ړ�����
	XMMATRIX matTrans = XMMatrixIdentity();

	//�_�b�V��
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
			isRight_ = false;//������
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
	//��
	//XMFLOAT3 move = Object3d::GetPosition();

	////�L�[�{�[�h���͂ɂ��ړ�����
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
		//�����������x
		const float fallAcc = -0.1f;
		const float fallVYMin = -2.0f;
		//����
		fallVec_.y = max(fallVec_.y + fallAcc, fallVYMin);
		//�ړ�
		position_.x += fallVec_.x;
		position_.y += fallVec_.y;
		position_.z += fallVec_.z;
	}
	//�W�����v����
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
		//����_
		start_ = { move.x,jumpBackPos_.y - offsetPosY,-60.0f };
		point1_ = { move.x,jumpBackPos_.y + JumpBackPosY,-40.0f };
		point2_ = { move.x,jumpBackPos_.y + JumpBackPosY,-20.0f };
		end_ = { move.x,jumpBackPos_.y - offsetPosY,0.0f };

		//���ݎ��Ԃ��擾����
		nowCount_ = std::chrono::steady_clock::now();
		//�O��L�^����̌o�ߎ��Ԃ��擾����
		elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

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
	//���R���C�_�[�̎擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	assert(sphereCollider);

	//��p�N�G���[�R�[���o�b�N�N���X��`
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		PlayerQueryCallback(Sphere* sphere) :sphere_(sphere) {};

		//�Փˎ��̃R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info)
		{
			//���[���h�����
			const XMVECTOR up = { 0.0f,1.0f,0.0f,0.0f };
			//�r�˕���
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//������Ɣr�˕����̊p�x���̃R�T�C���l
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			//�n�ʔ���̂������l�p�x
			const float threshold = cosf(XMConvertToRadians(30.0f));
			//�p�x���ɂ���ēV�䖔�͒n�ʂƔ��肳���ꍇ��������
			if (-threshold < cos && cos < threshold)
			{
				//����r��(�����o��)
				sphere_->center += info.reject;
				move += info.reject;
			}

			return true;
		}

	public:
		Sphere* sphere_ = nullptr;
		//�r�˂ɂ��ړ���
		XMVECTOR move = {};

	};

	// ���N�G���[�A�R���C�_�[�X�V
	//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
	PlayerQueryCallback callback(sphereCollider);

	//���ƒn�`�̌�����S����
	colManager_->QuerySphere(*sphereCollider, &callback, attribute);
	//�����ɂ��r�˕�������
	position_.x += callback.move.m128_f32[0];
	position_.y += callback.move.m128_f32[1];
	//position_.z += callback.move.m128_f32[2];

	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();

	eyepos.x += callback.move.m128_f32[0];

	tarpos.x += callback.move.m128_f32[0];

	//�R���C�_�[�X�V
	UpdateWorldMatrix();
	camera_->SetEye(eyepos);
	camera_->SetTarget(tarpos);
	collider_->Update();

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RaycastHit raycastHit;
	//�ڒn���
	if (onGround_)
	{
		//�X���[�Y�ɍ�����邽�߂̋z������
		const float adsDistance = 0.2f;
		//�ڒn���ێ�
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//�s��X�V
			Object3d::Update();
		}
		//�n�ʂ������̂ŗ���
		else
		{
			onGround_ = false;
			fallVec_ = {};
		}
	}
	//�������
	else if (fallVec_.y <= 0.0f)
	{
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * 2.0f))
		{
			//���n
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//�s��X�V
			Object3d::Update();
		}
	}

	//�s��X�V��
	Object3d::Update();
}

//�U������
void Player::Attack() {

	if (input_->TriggerKey(DIK_X)) {
		//�e�̑��x
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
		//���L�����̍��W���R�s�[
		XMFLOAT3 pos = Object3d::GetPosition();

		//�e�𐶐���������
		std::unique_ptr<PlayerBullet> newBullet;
		newBullet = PlayerBullet::Create(position_, velocity, modelBullet_);
		newBullet->SetCamera(camera_);
		newBullet->Update();

		//�e��o�^
		gameScene_->AddPlayerBullet(std::move(newBullet));
		//���郁�����̏��L��������unique_ptr�͂�����������݂ł��Ȃ�
		//���̏��L�������n���邽�߂̋@�\�� std::move()
	}
}

void Player::Trans() {

	XMMATRIX world;
	//�s��X�V
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//����
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	Object3d::SetWorld(world);

}

//���[���h���W���擾
XMFLOAT3 Player::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
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
	//�ړ�����
	if (!isJumpBack_)Move();
	//�U������
	FallAndJump();
	if (isBack)JumpBack();
	if (isAttack)Attack();

	if (life_ <= 0)
	{
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();

		easeOffset_ = { -18.0f,position_.y,85.0f + position_.z };//�ŏ��ɃI�t�Z�b�g�𑫂��Ȃ��ƉE�J�����A1�񑫂��ƒ����J�����A2�񑫂����獶�J����
		//�E�J�������_[i][0]
		easeDeadCameraEye_[0][0].SetEasing(nowEye_.x, nowEye_.x + easeOffset_.x, 1.0f);
		easeDeadCameraEye_[1][0].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
		easeDeadCameraEye_[2][0].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
		//�E�J���������_[i][0]
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
		//���_�V�F�C�N
		XMFLOAT3 Eye = nowEye_ + hitMove_;
		const XMFLOAT3 hitEye = { 1.0f,1.0f,1.0f };
		camera_->ShakeEye(Eye, 1, { Eye.x - hitEye.x,Eye.y - hitEye.y,Eye.z - hitEye.z },
			{ Eye.x + hitEye.x,Eye.y + hitEye.y,Eye.z + hitEye.z });

		//�����_�V�F�C�N
		XMFLOAT3 Target = nowTarget_ + hitMove_;
		const XMFLOAT3 hitTarget = { 1.0f,1.0f,1.0f };
		camera_->ShakeTarget(Target, 1, { Target.x - hitTarget.x,Target.y - hitTarget.y,Target.z - hitTarget.z },
			{ Target.x + hitTarget.x,Target.y + hitTarget.y,Target.z + hitTarget.z });
		camera_->Update();

		//+���ăC�[�W���O
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
	//�ړ�����
	Trans();

#ifdef _DEBUG
	//�f�o�b�O�p
	//if (input_->TriggerKey(DIK_M))
	//{
	//	nowEye_ = camera_->GetEye();
	//	nowTarget_ = camera_->GetTarget();

	//	easeOffset_ = { -18.0f,position_.y,85.0f + position_.z };//�ŏ��ɃI�t�Z�b�g�𑫂��Ȃ��ƉE�J�����A1�񑫂��ƒ����J�����A2�񑫂����獶�J����
	//	//�E�J�������_[i][0]
	//	easeDeadCameraEye_[0][0].SetEasing(nowEye_.x, nowEye_.x + easeOffset_.x, 1.0f);
	//	easeDeadCameraEye_[1][0].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
	//	easeDeadCameraEye_[2][0].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
	//	//�E�J���������_[i][0]
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
		//���v���Z�b�g
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

		//���v���Z�b�g
		ParticleManager::Preset smoke =
		{
			particleSmoke_,{position_.x,position_.y+5.0f,position_.z},{ 25.0f ,10.0f,15.0f },
			{ MyMath::RandomMTFloat(0.0f,0.1f),MyMath::RandomMTFloat(0.5f,3.0f),0.3f },
			{ 0.0f,0.001f,0.0f },5,{ 4.0f, 0.0f },
			{ MyMath::RandomMTFloat(0.8f,1.0f),MyMath::RandomMTFloat(0.8f,1.0f),MyMath::RandomMTFloat(0.95f,1.0f),1.0f },
			{ 1.0f,1.0f,1.0f,0.0f }
		};

		//��������
		pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
		pmSmoke_->ActiveY(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
			smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

		//�J�������x
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
		//����
		for (int i = 0; i < 2; i++)easeExplosionSizeAndAlpha_[i].ease_out_expo();

		spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[0].num_X,easeExplosionSizeAndAlpha_[1].num_X });

		if (spriteExplosion_->GetSize().x == easeExplosionSizeAndAlpha_[0].end)
		{
			easeExplosionSizeAndAlpha_[2].Standby(false);//�J���[����
			isExplosion_ = true;
			isCameraCentralEnd_ = false;
		}
	}
	else if (isCameraRightEnd_)
	{
		//�J�����ړ���3�g(����)
		for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][2].ease_out_cubic();
		for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][2].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[0][2].num_X,easeDeadCameraEye_[1][2].num_X, easeDeadCameraEye_[2][2].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[0][2].num_X,easeDeadCameraTarget_[1][2].num_X, easeDeadCameraTarget_[2][2].num_X });
		if (camera_->GetEye().z == easeDeadCameraEye_[2][2].end)
		{

			for (int i = 0; i < 2; i++)easeExplosionSizeAndAlpha_[i].Standby(false);//�T�C�Y����
			isCameraCentralEnd_ = true;
			isCameraRightEnd_ = false;
		}

	}
	else if (isCameraLeftEnd_)
	{
		//�J�����ړ���2�g(��)
		for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][1].ease_out_cubic();
		for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][1].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[0][1].num_X,easeDeadCameraEye_[1][1].num_X, easeDeadCameraEye_[2][1].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[0][1].num_X,easeDeadCameraTarget_[1][1].num_X, easeDeadCameraTarget_[2][1].num_X });
		if (camera_->GetEye().z == easeDeadCameraEye_[2][1].end)
		{
			//�����J�������_[i][2]
			easeDeadCameraEye_[0][2].SetEasing(nowEye_.x + easeOffset_.x, nowEye_.x + easeOffset_.x, 1.0f);
			easeDeadCameraEye_[1][2].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
			easeDeadCameraEye_[2][2].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
			//�����J���������_[i][2]
			easeDeadCameraTarget_[0][2].SetEasing(nowTarget_.x + easeOffset_.x, nowTarget_.x + easeOffset_.x, 1.0f);
			easeDeadCameraTarget_[1][2].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, 1.0f);
			easeDeadCameraTarget_[2][2].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, 1.0f);
			//�J�����ړ���3�g����(����)
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
		//�J�����ړ���1�g(�E)
		for (int i = 0; i < 3; i++)easeDeadCameraEye_[i][0].ease_out_cubic();
		for (int i = 0; i < 3; i++)easeDeadCameraTarget_[i][0].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[0][0].num_X,easeDeadCameraEye_[1][0].num_X, easeDeadCameraEye_[2][0].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[0][0].num_X,easeDeadCameraTarget_[1][0].num_X, easeDeadCameraTarget_[2][0].num_X });
		if (camera_->GetEye().z == easeDeadCameraEye_[2][0].end)
		{
			//���J�������_[i][1]
			easeDeadCameraEye_[0][1].SetEasing(nowEye_.x + (easeOffset_.x + easeOffset_.x), nowEye_.x + easeOffset_.x, 1.0f);
			easeDeadCameraEye_[1][1].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, 1.0f);
			easeDeadCameraEye_[2][1].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, 1.0f);
			//���J���������_[i][1]
			easeDeadCameraTarget_[0][1].SetEasing(nowTarget_.x + (easeOffset_.x + easeOffset_.x), nowTarget_.x + easeOffset_.x, 1.0f);
			easeDeadCameraTarget_[1][1].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, 1.0f);
			easeDeadCameraTarget_[2][1].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, 1.0f);
			//�J�����ړ���2�g����(��)
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
	//���W���Œ肵�ăX�P�[�����C�[�W���O
	position_ = stopPos_;
	for (int i = 0; i < 3; i++)easeChangeScaleStageClear_[i].ease_out_cubic();
	scale_ = { easeChangeScaleStageClear_[0].num_X,easeChangeScaleStageClear_[1].num_X ,easeChangeScaleStageClear_[2].num_X };
	
}
