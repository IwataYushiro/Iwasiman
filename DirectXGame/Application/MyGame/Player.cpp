#include "Player.h"
#include <cassert>

using namespace DirectX;

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
	return nullptr;
}

bool Player::Initialize() {

	/*modelBullet_ = Model::LoadFromOBJ("playerbullet");
	objBullet_ = Object3d::Create();

	objBullet_->SetModel(modelBullet_);
	objBullet_->SetCamera(camera_);*/

	//�V���O���g���C���X�^���X���擾
	this->input_ = Input::GetInstance();

	//���[���h�ϊ��̏�����
	pos = { -20.0f,-10.0f,-60.0f };
	obj_->SetPosition(pos);
	//�W�����v������
	isJump = false;

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
	//�d��
	gravity = 0.0f;

	//�p�[�e�B�N��
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
	//�e���Z�b�g
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Reset();
	}
}
void Player::Update() {

	if (!isDead_) {
		//���S�t���O�̗������e���폜
		bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) { return bullet->IsDead(); });

		//�ړ�����
		Move();
		//�U������
		Jump();
		JumpBack();
		//Attack();

		//�e�X�V
		for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
			bullet->Update();
		}

		//�ړ�����
		Trans();
		
	}
	pmDash_->Update();
	obj_->Update();
}

void Player::Draw() {
	if (!isDead_) {
		obj_->Draw();

		//�e�`��
		for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
			bullet->Draw();
		}

	}
}

void Player::DrawParticle()
{
	pmDash_->Draw();
}

//�ړ�����
void Player::Move() {

	XMFLOAT3 move = obj_->GetPosition();
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
	
	obj_->SetPosition(move);
	camera_->SetEye(cmove);
	camera_->SetTarget(tmove);
}

void Player::Jump()
{

	XMFLOAT3 move = obj_->GetPosition();

	//�L�[�{�[�h���͂ɂ��ړ�����
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

	//����_
	start = { move.x,-10.0f,-60.0f };
	p1 = { move.x,10.0f,-40.0f };
	p2 = { move.x,10.0f,-20.0f };
	end = { move.x,-10.0f,0.0f };

	//����
	
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
	obj_->SetPosition(move);
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

		matVec *= obj_->GetWorld();

		//���L�����̍��W���R�s�[
		XMFLOAT3 position = obj_->GetPosition();

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

	XMMATRIX matScale = XMMatrixScaling(obj_->GetScale().x, obj_->GetScale().y, obj_->GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(obj_->GetRotation().z)
		* XMMatrixRotationX(obj_->GetRotation().x) * XMMatrixRotationY(obj_->GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(obj_->GetPosition().x,
		obj_->GetPosition().y, obj_->GetPosition().z);

	//����
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	obj_->SetWorld(world);

}

//���[���h���W���擾
XMFLOAT3 Player::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = obj_->GetPosition().x;
	worldPos.y = obj_->GetPosition().y;
	worldPos.z = obj_->GetPosition().z;

	return worldPos;
}

//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
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
