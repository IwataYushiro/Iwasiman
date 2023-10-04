#include "Player.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "GamePlayScene.h"

using namespace DirectX;
CollisionManager* Player::colManager_ = CollisionManager::GetInstance();

Player::~Player() {
	//���f���̉��
	delete particleDash_;
	delete pmDash_;
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
	startCount = std::chrono::steady_clock::now();	//�J�n����
	nowCount = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	maxTime = 1.0f;					//�S�̎���
	timeRate;

	//�p�[�e�B�N��
	particleDash_ = Particle::LoadFromParticleTexture("particle1.png");
	pmDash_ = ParticleManager::Create();
	pmDash_->SetParticleModel(particleDash_);

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
	startCount = std::chrono::steady_clock::now();	//�J�n����
	nowCount = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	maxTime = 1.0f;					//�S�̎���

}
void Player::Update(bool isBack, bool isAttack) {

	pmDash_->SetCamera(camera_);

	if (!isDead_)
	{

		//�ړ�����
		if (!isJumpBack_)Move();
		//�U������
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
		//�ړ�����
		Trans();

	}
	pmDash_->Update();

	camera_->Update();
	UpdateWorldMatrix();
	collider_->Update();

	//���n����
	Landing(COLLISION_ATTR_LANDSHAPE);

}

void Player::Draw() { Object3d::Draw(); }

void Player::DrawParticle() { pmDash_->Draw(); }

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

void Player::CameraMove()
{
	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	float moveSpeed = 1.0f;

	//�L�[�{�[�h���͂ɂ��ړ�����
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

	//����_
	start = { move.x,-10.0f,-60.0f };
	p1 = { move.x,10.0f,-40.0f };
	p2 = { move.x,10.0f,-20.0f };
	end = { move.x,-10.0f,0.0f };

	//����

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
		//���ݎ��Ԃ��擾����
		nowCount = std::chrono::steady_clock::now();
		//�O��L�^����̌o�ߎ��Ԃ��擾����
		elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

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
