#include "EnemyCore.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;
CollisionManager* EnemyCore::colManager_ = CollisionManager::GetInstance();

EnemyCore::~EnemyCore() {
	delete modelBullet_;
}

std::unique_ptr<EnemyCore> EnemyCore::Create(Model* model, Player* player, GamePlayScene* gamescene, unsigned short stage)
{
	//�C���X�^���X����
	std::unique_ptr<EnemyCore> ins = std::make_unique<EnemyCore>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	if (player)ins->SetPlayer(player);
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// ������
bool EnemyCore::Initialize() {

	if (!Object3d::Initialize()) return false;

	modelBullet_ = Model::LoadFromOBJ("enemybullet");

	startCount = std::chrono::steady_clock::now();	//�J�n����
	nowCount = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	maxTime = 5.0f;					//�S�̎���
	timeRate;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	Parameter();

	return true;
}

//�p�����[�^
void EnemyCore::Parameter() {
	phase_ = Phase::CoreStage1;
	maxTime = 2.0f;
	life_ = 10;


	isReverse_ = false;
	//���˃^�C�}�[������
	fireTimer = kFireInterval;

	isDead_ = false;


}

//���Z�b�g
void EnemyCore::Reset() { Parameter(); }

//�X�V
void EnemyCore::Update() {


	//���W���ړ�������
	switch (phase_) {
	case EnemyCore::Phase::CoreStage1:
		UpdateCore();
		break;

	case EnemyCore::Phase::CoreBreak:
		UpdateBreakCore();

		break;
	case EnemyCore::Phase::Leave:
		UpdateLeave();
		break;
	}

	//�s��X�V
	Trans();
	camera_->Update();
	Object3d::Update();
}

//�]��
void EnemyCore::Trans() {

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
void EnemyCore::Fire() {
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
	XMFLOAT3 position = GetPosition();

	//�e�𐶐���������
	std::unique_ptr<EnemyBullet> newBullet;
	newBullet = EnemyBullet::Create(position, velocity, modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//�e��o�^
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

//�`��
void EnemyCore::Draw() {

	//���f���̕`��
	Object3d::Draw();



}

void EnemyCore::UpdateCore()
{
	//���x
	float cameraMove = camera_->GetEye().x;

	//����_
	start = nowPos;
	p1 = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,40.0f,70.0f };
	p2 = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,25.0f,85.0f };
	end = { MyMath::RandomMTFloat(-20.0f,20.0f) + cameraMove,10.0f,100.0f };

	//���x
	XMFLOAT3 velocity;
	//�ړ�
	if (!isReverse_)velocity = { 0.3f, 0.0f, 0.0f };
	else velocity = { -0.3f, 0.0f, 0.0f };

	position.x += velocity.x;
	position.y += velocity.y;
	position.z += velocity.z;

	//�w��̈ʒu�ɓ��B�����甽�]
	if (position.x >= 65.0f) {
		//�����灩
		isReverse_ = true;
	}
	if (position.x <= -65.0f) {
		isReverse_ = false;
	}

	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer--;
	//�w�莞�ԂɒB����
	if (fireTimer <= 0) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		fireTimer = MyMath::RandomMTInt(kFireInterval, kFireInterval * 2);
	}
	//���񂾂玩�@�̒e�݂�����
	if (life_ == 0) {
		nowPos = Object3d::GetPosition();

		collider->SetAttribute(COLLISION_ATTR_PLAYERS);
		collider->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);

		life_ = 0;
		startCount = std::chrono::steady_clock::now();	//�J�n����
		//�G�ɂԂ���

		phase_ = Phase::CoreBreak;
	}
}

void EnemyCore::UpdateBreakCore()
{

	//����

	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

	float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	timeRate = min(elapsed / maxTime, 1.0f);

	position = Bezier3(start, p1, p2, end, timeRate);

}

//���E
void EnemyCore::UpdateLeave() {

	isDead_ = true;
	bossDead = true;
}

const XMFLOAT3 EnemyCore::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
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


//���[���h���W���擾
XMFLOAT3 EnemyCore::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void EnemyCore::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;
	}

	else if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isDead_ = true;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}