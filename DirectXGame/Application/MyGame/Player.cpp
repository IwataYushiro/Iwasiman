#include "Player.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* Player::colManager_ = CollisionManager::GetInstance();

Player::~Player() {
	//���f���̉��

	delete modelBullet_;
	delete objBullet_;
	delete particleDash_;
	delete pmDash_;
}

Player* Player::Create(Model* model)
{
	//�C���X�^���X����
	Player* ins = new Player();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		delete ins;
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	return ins;
}

bool Player::Initialize() {

	if (!Object3d::Initialize()) return false;

	/*modelBullet_ = Model::LoadFromOBJ("playerbullet");
	objBullet_ = Object3d::Create();

	objBullet_->SetModel(modelBullet_);
	objBullet_->SetCamera(camera_);*/

	//���[���h�ϊ��̏�����
	pos = { -20.0f,-10.0f,-60.0f };
	Object3d::SetPosition(pos);
	//�W�����v������
	onGround = true;

	//�����ɂ��邩
	isJumpBack = false;
	//���ɂ��邩
	isBack = false;

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
	collider->SetAttribute(COLLISION_ATTR_ALLIES);

	return true;
}

void Player::Reset() {
	pos = { -20.0f, -10.0f, -60.0f };

	life_ = 5;
	isDead_ = false;
	//�e���Z�b�g
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Reset();
	}
}
void Player::Update() {
	input_ = Input::GetInstance();

	pmDash_->SetCamera(camera_);

	if (!isDead_) {
		//���S�t���O�̗������e���폜
		bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) { return bullet->IsDead(); });

		//�ړ�����
		Move();
		//�U������
		FallAndJump();
		JumpBack();
		//Attack();

		////�e�X�V
		//for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		//	bullet->Update();
		//}

		//�ړ�����
		Trans();
		
	}
	pmDash_->Update();

	camera_->Update();
	Object3d::Update();
	//���n����
	Landing();
}

void Player::Draw() {
	if (!isDead_) {
		Object3d::Draw();

		////�e�`��
		//for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		//	bullet->Draw();
		//}

	}
}

void Player::DrawParticle()
{
	pmDash_->Draw();
}

//�ړ�����
void Player::Move() {

	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	float moveSpeed = 0.5f;

	//�L�[�{�[�h���͂ɂ��ړ�����
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

	
	//�_�b�V��
	if (input_->PushKey(DIK_LSHIFT)|| input_->PushKey(DIK_RSHIFT))
	{
		if (input_->PushKey(DIK_A)) {
			pmDash_->ActiveX(particleDash_, Object3d::GetPosition(), { 20.0f ,10.0f,0.0f }, { -4.2f,0.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
			move.x -= moveSpeed * 2.0f;
			cmove.x -= moveSpeed * 2.0f;
			tmove.x -= moveSpeed * 2.0f;
		}
		if (input_->PushKey(DIK_D)) {
			pmDash_->ActiveX(particleDash_, Object3d::GetPosition(), { 20.0f ,10.0f,0.0f }, { 4.2f,0.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
			move.x += moveSpeed * 2.0f;
			cmove.x += moveSpeed * 2.0f;
			tmove.x += moveSpeed * 2.0f;
		}
	}
	if (input_->PushKey(DIK_W)) {
		move.y += moveSpeed;
	}
	if (input_->PushKey(DIK_S)) {
		move.y -= moveSpeed;
	}

	Object3d::SetPosition(move);
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

	if (!onGround)
	{
		//�����������x
		const float fallAcc = -0.1f;
		const float fallVYMin = -2.0f;
		//����
		fallVec.y = max(fallVec.y + fallAcc, fallVYMin);
		//�ړ�
		position.x += fallVec.x;
		position.y += fallVec.y;
		position.z += fallVec.z;
	}
	//�W�����v����
	else if (input_->TriggerKey(DIK_SPACE))
	{
		onGround = false;
		const float jumpVYFist = 2.0f;
		fallVec = { 0.0f,jumpVYFist,0.0f };
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

	if (onGround)
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
			//���ݎ��Ԃ��擾����
			nowCount = std::chrono::steady_clock::now();
			//�O��L�^����̌o�ߎ��Ԃ��擾����
			elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

			float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

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
	Object3d::SetPosition(move);
}

void Player::Landing()
{
	//���R���C�_�[�̎擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RaycastHit raycastHit;
	//�ڒn���
	if (onGround)
	{
		//�X���[�Y�ɍ�����邽�߂̋z������
		const float adsDistance = 0.2f;
		//�ڒn���ێ�
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//�s��X�V
			Object3d::Update();
		}
		//�n�ʂ������̂ŗ���
		else
		{
			onGround = false;
			fallVec = {};
		}
	}
	//�������
	else if (fallVec.y >= -10.0f)
	{
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 2.0f))
		{
			//���n
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//�s��X�V
			Object3d::Update();
		}
	}
}

//�U������
void Player::Attack() {

	if (input_->TriggerKey(DIK_SPACE)) {
		//�e�̑��x
		const float kBulletSpeed = 1.0f;
		XMFLOAT3 velocity(0.0f, 0.0f, kBulletSpeed);

		XMMATRIX matVec = XMMatrixIdentity();
		matVec.r[0].m128_f32[0] = velocity.x;
		matVec.r[0].m128_f32[1] = velocity.y;
		matVec.r[0].m128_f32[2] = velocity.z;
		matVec.r[0].m128_f32[3] = 0.0f;

		matVec *= Object3d::GetWorld();

		//���L�����̍��W���R�s�[
		XMFLOAT3 position = Object3d::GetPosition();

		//�e�𐶐���������
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(modelBullet_, objBullet_, position, velocity);

		//�e��o�^
		bullets_.push_back(std::move(newBullet));
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
void Player::OnCollision(const CollisionInfo& info, unsigned short attribute) {
	if (attribute == COLLISION_ATTR_ENEMYS)
	{
		life_--;
		pmDash_->ActiveZ(particleDash_, { Object3d::GetPosition() }, { 0.0f ,0.0f,25.0f },
			{ 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 2, { 3.0f, 0.0f });
		if (life_ <= 0) {
			isDead_ = true;
		}
		pmDash_->Update();
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
