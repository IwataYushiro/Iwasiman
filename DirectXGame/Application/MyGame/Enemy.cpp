#include "Enemy.h"
#include <cassert>
#include "SphereCollider.h"

#include "Player.h"

using namespace DirectX;

Enemy::~Enemy() {
	//���f���̉��

	delete modelBullet_;
	delete objBullet_;
}

Enemy* Enemy::Create(Model* model)
{
	//�C���X�^���X����
	Enemy* ins = new Enemy();
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

// ������
bool Enemy::Initialize() {
	
	if (!Object3d::Initialize()) return false;

	modelBullet_ = Model::LoadFromOBJ("enemybullet");
	objBullet_ = Object3d::Create();

	objBullet_->SetModel(modelBullet_);
	
	Stage1Parameter();

	startCount= std::chrono::steady_clock::now();	//�J�n����
	nowCount= std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	maxTime = 10.0f;					//�S�̎���
	timeRate;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,radius_,0.0f,0.0f }, radius_));

	return true;
}

//�p�����[�^
void Enemy::Stage1Parameter() {

	isReverse_ = false;
	//�����X�e�[�W
	scale = { 3.0f,3.0f,3.0f };
	pos = { -30.0f,0.0f,100.0f };
	Object3d::SetPosition(pos);
	Object3d::SetScale(scale);
	//�����t�F�[�Y
	phase_ = Phase::ApproachStage1;

	//���˃^�C�}�[������
	fireTimer = kFireIntervalStage1;

	life_ = 40;
	isDead_ = false;

	isReverse_ = false;
	//�e���Z�b�g
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Reset();
	}
}

//���Z�b�g
void Enemy::Reset() { Stage1Parameter(); }

//�X�V
void Enemy::Update() {

objBullet_->SetCamera(camera_);
	//���S�t���O�̗������e���폜
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });

	//���W���ړ�������
	switch (phase_) {
	case Enemy::Phase::ApproachStage1:

		UpdateApproachStage1();
		break;

	case Enemy::Phase::AttackStage1:

		UpdateAttackStage1();

		break;
	}
	//�e�X�V
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Update();
	}

	//���W���ړ�������
	switch (phase_) {
	case Enemy::Phase::Leave:
		UpdateLeave();
		break;

	}

	//�s��X�V
	Trans();
	camera_->Update();
	Object3d::Update();
}

//�]��
void Enemy::Trans() {

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
//�e����
void Enemy::Fire() {

	assert(player_);

	//�e�̑��x
	const float kBulletSpeed = 0.5f;
	XMFLOAT3 velocity = {};

	//���@�̃��[���h���W���擾
	player_->GetWorldPosition();
	//�G�̃��[���h���W���擾
	GetWorldPosition();
	//�G�����@�̍����x�N�g�������߂�
	velocity.x = player_->GetWorldPosition().x - GetWorldPosition().x;
	velocity.y = player_->GetWorldPosition().y - GetWorldPosition().y;
	velocity.z = player_->GetWorldPosition().z - GetWorldPosition().z;
	// �x�N�g���̐��K��
	float len;
	len = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
	if (len != 0)
	{
		velocity.x /= len;
		velocity.y /= len;
		velocity.z /= len;
	}

	// �x�N�g���̒����𑬂��ɍ��킹��

	velocity.z -= kBulletSpeed;

	//���W���R�s�[
	XMFLOAT3 position = Object3d::GetPosition();

	//�e�𐶐���������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(modelBullet_, objBullet_, position, velocity);

	//�e��o�^
	enemyBullets_.push_back(std::move(newBullet));
}

//�`��
void Enemy::Draw() {
	if (!isDead_) {
		//���f���̕`��
		Object3d::Draw();

		//�e�`��
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			bullet->Draw();
		}
	}

}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void Enemy::UpdateApproachStage1() {
	//���x
	XMFLOAT3 velocity;
	float cameraMove = camera_->GetEye().x;

	//�ړ�
	velocity = { 0.0f, 0.0f, -0.5f };
	pos.x += velocity.x;
	pos.y += velocity.y;
	pos.z += velocity.z;

	Object3d::SetPosition({ pos.x + cameraMove,pos.y,pos.z });
	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer--;
	//�w�莞�ԂɒB����
	if (fireTimer <= 0) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		fireTimer = kFireIntervalStage1;
	}

	//�w��̈ʒu�ɓ��B������U��
	if (pos.z < 60.0f) {
		phase_ = Phase::AttackStage1;
	}
}
//�U��
void Enemy::UpdateAttackStage1() {

	//���x
	XMFLOAT3 velocity;
	float cameraMove = camera_->GetEye().x;
	//����_
	start = { -30.0f+cameraMove,0.0f,60.0f };
	p1 = { -10.0f+cameraMove,-30.0f,60.0f };
	p2 = { 10.0f+cameraMove,30.0f,60.0f };
	end = { 30.0f+cameraMove,0.0f,60.0f };
	//����

	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

	float elapsed= std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count()/1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	timeRate = min(elapsed / maxTime, 1.0f);

	//�ړ�
	velocity = { 0.5f, 0.0f, 0.0f };
	if (isReverse_) {
		pos = Bezier3(end, p2, p1, start, timeRate);
	}
	else {
		pos = Bezier3(start, p1, p2, end, timeRate);
	}
	Object3d::SetPosition(pos);
	//�w��̈ʒu�ɓ��B�����甽�]
	if (pos.x >= 30.0f+cameraMove) {
		isReverse_ = true;
		startCount = std::chrono::steady_clock::now();
	}
	if (pos.x <= -30.0f+cameraMove) {
		isReverse_ = false;
		startCount = std::chrono::steady_clock::now();
	}

	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer--;
	//�w�莞�ԂɒB����
	if (fireTimer <= 0) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		fireTimer = kFireIntervalStage1;
	}
	//���񂾂�
	if (life_ <= 0) {
		isDead_ = true;
		life_ = 0;
	}
	if (isDead_)
	{
		phase_ = Phase::Leave;
	}
}

//���E
void Enemy::UpdateLeave() {
	//���x
	XMFLOAT3 velocity;

	//�ړ�
	velocity = { 0.0f, 0.0f, 0.03f };
	pos.x += velocity.x;
	pos.y += velocity.y;
	pos.z += velocity.z;
	Object3d::SetPosition(pos);
}

const XMFLOAT3 Enemy::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	XMFLOAT3 ans;
	ans.x=(1.0f - t) * (1.0f - t) * (1.0f - t) * p0.x + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.x + 3 * (1.0f - t) * t * t * p2.x + t * t * t * p3.x;
	ans.y = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.y + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.y + 3 * (1.0f - t) * t * t * p2.y + t * t * t * p3.y;
	ans.z = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.z + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.z + 3 * (1.0f - t) * t * t * p2.z + t * t * t * p3.z;

	return ans;
}


//���[���h���W���擾
XMFLOAT3 Enemy::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void Enemy::OnCollision(const CollisionInfo& info)
{
	life_--;
}
//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
void Enemy::OnCollisionPlayer() { life_--; }