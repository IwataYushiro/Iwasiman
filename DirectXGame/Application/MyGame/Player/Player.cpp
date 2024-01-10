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

*	���@

*/

//�ÓI�����o�ϐ��̎���
IwasiEngine::CollisionManager* Player::colManager_ = CollisionManager::GetInstance();

Player::~Player() {

}

std::unique_ptr<Player> Player::Create(const PlayerModelList* model,GamePlayScene* gamescene)
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
	if (model->playerModel)ins->modelPlayer_ = model->playerModel;		//�ʏ헧���p
	if (model->playerBullet) ins->modelBullet_ = model->playerBullet;	//�e
	if (model->playerHit)ins->modelHit_ = model->playerHit;				//�q�b�g��
	if (model->playerMove)ins->modelMove_ = model->playerMove;			//�ړ���
	if (model->playerJump)ins->modelJump_ = model->playerJump;			//�W�����v��
	if (model->playerAttack)ins->modelAttack_ = model->playerAttack;	//�U����
	//�Q�[���V�[���̃Z�b�g
	if (gamescene)ins->SetGameScene(gamescene);

	//�ŏ��̃��f��
	ins->SetModel(model->playerMove);
	return ins;
}

bool Player::Initialize() {

	if (!Object3d::Initialize()) return false;

	//�V���O���g���C���X�^���X
	spCommon_ = SpriteCommon::GetInstance();
	input_ = Input::GetInstance();
	//�����l�Z�b�g
	const int32_t startLife = 10;	//�J�n�����C�t
	life_ = startLife;				//�����C�t����
	isDead_ = false;				//���S�t���O
	isHit_ = false;					//�����t���O
	mutekiCount_ = 0;				//���G����

	//�E�������Ă��邩
	isRight_ = true;
	//�W�����v������
	onGround_ = true;
	//�W�����v��
	const float startJumpVYFist = 2.0f;
	jumpVYFist_ = startJumpVYFist;

	//�����ɂ��邩
	isJumpBack_ = false;
	//���ɂ��邩
	isBack_ = false;

	//�����W�����v�Ɏg���x�W�F�Ȑ��p�̎���
	startCount_ = std::chrono::steady_clock::now();	//�J�n����
	nowCount_ = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount_;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

	//�X�v���C�g
	//���C�t�o�[
	spCommon_->LoadTexture(GPSPTI_PlayerLifeBarTex, "texture/plife2.png");
	spriteLifeBar_->Initialize(spCommon_, GPSPTI_PlayerLifeBarTex);
	spriteLifeBar_->SetPosition(lifeBarPos_);
	spriteLifeBar_->SetColor(green_);//��{�͗�
	spriteLifeBar_->Update();
	//��e���o
	spCommon_->LoadTexture(GPSPTI_PlayerHitEffectTex, "texture/fade2.png");
	spriteHit_->Initialize(spCommon_, GPSPTI_PlayerHitEffectTex);
	spriteHit_->SetColor(hitColor_);//�F�͐Ԃ�����{�͓���
	spriteHit_->Update();
	//�����p�X�v���C�g
	spCommon_->LoadTexture(GPSPTI_PlayerExplosionTex, "texture/explosion2.png");
	spriteExplosion_->Initialize(spCommon_, GPSPTI_PlayerExplosionTex);
	spriteExplosion_->SetAnchorPoint(explosionAnchorPoint_);
	spriteExplosion_->SetPosition(explosionPos_);
	spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[XYW_X].start,easeExplosionSizeAndAlpha_[XYW_Y].start });
	spriteExplosion_->SetColor({ asIsColor_.x,asIsColor_.y,asIsColor_.z,easeExplosionSizeAndAlpha_[XYW_W].start });
	spriteExplosion_->Update();

	//�p�[�e�B�N��
	//�������̉�
	particleSmoke_ = Particle::LoadFromParticleTexture("particle9.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());
	pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	//�_�b�V���A�����p�̉�
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());
	//�e�����Ƃ��̉�
	particleBullet_ = Particle::LoadFromParticleTexture("particle2.png");
	pmBullet_ = ParticleManager::Create();
	pmBullet_->SetParticleModel(particleBullet_.get());

	//�R���C�_�[�ǉ�
	const XMVECTOR colliderOffset = { 0.0f,0.0f,0.0f,0.0f };
	SetCollider(new SphereCollider(colliderOffset, radius_));
	//���@�{��
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	//�C�[�W���O�X�^���o�C
	easelifeBarSize_.Standby(false);
	return true;
}

void Player::Reset() {
	//�l���Z�b�g
	const int32_t resetLife = 10;	//�����l���C�t�Ƀ��Z�b�g
	life_ = resetLife;				//�������
	isDead_ = false;				//���S�t���O

	//�E�������Ă��邩
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
}
void Player::Update(const bool isBack, const bool isAttack, const bool isStart) {

	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	pmBullet_->SetCamera(camera_);

	if (!isStart)//�X�^�[�g���o���͉������Ȃ�
	{
		if (isAlive_)UpdateAlive(isBack, isAttack);		//������
		else if (isBreak_)UpdateBreak();				//���j��
		else if (isGoal_)UpdateGoal();					//�S�[����
	}
	//�X�V
	camera_->Update();		//�J����
	UpdateWorldMatrix();	//�s��
	pmFire_->Update();		//�p�[�e�B�N���}�l�[�W���[(��)
	pmSmoke_->Update();		//�p�[�e�B�N���}�l�[�W���[(��)
	collider_->Update();	//�R���C�_�[
	
	//���n����
	Landing(COLLISION_ATTR_LANDSHAPE);
	
	//���C�t�o�[�̃C�[�W���O
	easelifeBarSize_.ease_in_cubic();
	lifeBarDamageSize_.x = easelifeBarSize_.num_X;//�T�C�Y���Z�b�g
	spriteLifeBar_->SetTextureSize({ lifeBarDamageSize_.x * life_,lifeBarDamageSize_.y });
	spriteLifeBar_->SetSize({ lifeBarDamageSize_.x * life_,lifeBarDamageSize_.y });
	//���C�t�����̒l�ɂ܂ŉ���������s���`���ƒm�点��
	const int dangerLifeZone = 3;
	//�s���`���̓��C�t�o�[��Ԃ����A����ȊO�͗΂�
	if (life_ <= dangerLifeZone) { spriteLifeBar_->SetColor(red_); }
	else { spriteLifeBar_->SetColor(green_); }
	//�X�v���C�g�X�V
	spriteLifeBar_->Update();
	spriteHit_->Update();
	spriteExplosion_->Update();
}

void Player::Draw() { if (!isExplosion_)Object3d::Draw(); }//�`��

void Player::DrawSprite()
{
	if (!isBreak_)//�����Ă���
	{
		if (isAlive_)spriteLifeBar_->Draw();
		spriteHit_->Draw();
	}
	else//���ꂽ��
	{
		spriteExplosion_->Draw();
	}
}

void Player::DrawParticle() {
	//�e�p�[�e�B�N���`��
	pmSmoke_->Draw();
	pmFire_->Draw();
}

//�ړ�����
void Player::Move() {
	//�U��������߂̃C�[�W���O
	easeRotateRightY_.ease_out_cubic();
	//�l�̃Q�b�^�[
	XMFLOAT3 move = Object3d::GetPosition();
	XMFLOAT3 rot = Object3d::GetRotation();
	XMFLOAT3 cmove = camera_->GetEye();
	XMFLOAT3 tmove = camera_->GetTarget();
	
	//�p�[�e�B�N��
	const XMFLOAT3 startPosRight = { position_.x - 2.0f,position_.y + 1.0f ,position_.z };
	const XMFLOAT3 startPosLeft = { position_.x + 2.0f,position_.y + 1.0f ,position_.z };
	const ParticleManager::Preset fire =	//���v���Z�b�g
	{
		particleFire_.get(),
		position_,//�g��Ȃ�
		{ 0.0f ,2.0f,0.0f },
		{ 3.0f,0.3f,0.3f },
		{ 0.0f,0.001f,0.0f },
		2,
		{ 1.0f, 0.0f },
		{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
		{0.0f,0.0f,0.0f,1.0f}
	};
	const XMFLOAT3 reverseParticleVel = { -fire.vel.x,-fire.vel.y, -fire.vel.z };
	const int32_t walkParticleNum = 1;
	const XMFLOAT4 walkStartColor = { MyMath::RandomMTFloat(0.9f,1.0f),0.6f,0.6f,1.0f };
	//�L�[�{�[�h���͂ɂ��ړ�����
	XMMATRIX matTrans = XMMatrixIdentity();

	//�_�b�V��
	if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
	{
		//���Ƀ_�b�V���ړ�
		if (input_->PushKey(DIK_A)) {
			if (isRight_)
			{
				easeRotateRightY_.Standby(true);
				rot.y = easeRotateRightY_.end;
			}
			if(onGround_)model_ = modelMove_;
			isRight_ = false;
			pmFire_->ActiveX(fire.particle, startPosLeft, fire.pos, fire.vel,
				fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
			move.x -= moveSpeed * dashSpeed;
			cmove.x -= moveSpeed * dashSpeed;
			tmove.x -= moveSpeed * dashSpeed;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x -= moveSpeed * dashSpeed;
		}
		//�E�Ƀ_�b�V���ړ�
		else if (input_->PushKey(DIK_D)) {
			if (!isRight_)
			{
				easeRotateRightY_.Standby(false);
				rot.y = easeRotateRightY_.start;
			}
			if (onGround_)model_ = modelMove_;
			isRight_ = true;
			pmFire_->ActiveX(fire.particle, startPosRight, fire.pos, reverseParticleVel,
				fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
			move.x += moveSpeed * dashSpeed;
			cmove.x += moveSpeed * dashSpeed;
			tmove.x += moveSpeed * dashSpeed;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x += moveSpeed * dashSpeed;
		}
	}
	else//�ʏ�ړ�
	{
		//���ɒʏ�ړ�
		if (input_->PushKey(DIK_A)) {
			if (isRight_)
			{
				easeRotateRightY_.Standby(true);
				rot.y = easeRotateRightY_.end;
			}
			if (onGround_)model_ = modelMove_;
			isRight_ = false;
			pmFire_->ActiveX(fire.particle, startPosLeft, fire.pos, fire.vel,
				fire.acc, walkParticleNum, fire.scale, walkStartColor, fire.endColor);
			move.x -= moveSpeed;
			cmove.x -= moveSpeed;
			tmove.x -= moveSpeed;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x -= moveSpeed;
		}
		//�E�ɒʏ�ړ�
		else if (input_->PushKey(DIK_D)) {
			if (!isRight_)
			{

				easeRotateRightY_.Standby(false);
				rot.y = easeRotateRightY_.start;
			}
			if (onGround_)model_ = modelMove_;
			isRight_ = true;
			pmFire_->ActiveX(fire.particle, startPosRight, fire.pos, reverseParticleVel,
				fire.acc, walkParticleNum, fire.scale, walkStartColor, fire.endColor);
			move.x += moveSpeed;
			cmove.x += moveSpeed;
			tmove.x += moveSpeed;
			rot.y = easeRotateRightY_.num_X;
			if (isShake_)hitMove_.x += moveSpeed;
		}
	}

	//�l�̍X�V
	Object3d::SetPosition(move);
	Object3d::SetRotation(rot);
	camera_->SetEye(cmove);
	camera_->SetTarget(tmove);
}

void Player::FallAndJump()
{
	//�W�����v�͋����A�C�e��������Ă��邩���Ȃ����ŃW�����v�͂��ς��
	const float jumpPowerUp = 3.0f;			//������
	const float jumpPowerDefault = 2.0f;	//�ʏ펞
	if (isGetJumpItem_)
	{
		if (onGround_)jumpVYFist_ = jumpPowerUp;
		jumpPowerUpcount_++;
	}
	else
	{
		if (onGround_)jumpVYFist_ = jumpPowerDefault;
	}

	if (!onGround_)//�W�����v��
	{
		//���f����ύX
		model_ = modelJump_;
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
	else if (input_->TriggerKey(DIK_SPACE))//�n�ʂɒ����Ă���Ƃ��ɃX�y�[�X�L�[�ŃW�����v
	{
		onGround_ = false;
		const XMFLOAT3 startJumpVec = { 0.0f,jumpVYFist_,0.0f };
		fallVec_ = startJumpVec;
	}
	
	//�����A�C�e�����擾���Ă���Ƃ����̎��Ԃ���������W�����v�͂����Z�b�g�����
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

	//�x�W�F�Ȑ��̒l
	const XMFLOAT3 startBezier3Pos = { position_.x,jumpBackPos_.y - offsetPosY,-60.0f };
	const XMFLOAT3 point1Bezier3Pos = { position_.x,jumpBackPos_.y + JumpBackPosY,-40.0f };
	const XMFLOAT3 point2Bezier3Pos = { position_.x,jumpBackPos_.y + JumpBackPosY,-20.0f };
	const XMFLOAT3 endBezier3Pos = { position_.x,jumpBackPos_.y - offsetPosY,0.0f };

	//����_
	start_ = startBezier3Pos;
	point1_ = point1Bezier3Pos;
	point2_ = point2Bezier3Pos;
	end_ = endBezier3Pos;

	if (onGround_)//�n�ʂɒ����Ă�����
	{
		if (!isJumpBack_)
		{
			if (input_->TriggerKey(DIK_W))//�����փW�����v
			{
				if (isBack_)return;
				startCount_ = std::chrono::steady_clock::now();
				jumpBackPos_ = position_;
				isBack_ = true;
				isJumpBack_ = true;
			}
			if (input_->TriggerKey(DIK_S))//��O���փW�����v
			{
				if (!isBack_)return;
				startCount_ = std::chrono::steady_clock::now();
				jumpBackPos_ = position_;
				isBack_ = false;
				isJumpBack_ = true;
			}
		}
	}
	if (isJumpBack_)//�����A��O���W�����v��
	{

		//���ݎ��Ԃ��擾����
		nowCount_ = std::chrono::steady_clock::now();
		//�O��L�^����̌o�ߎ��Ԃ��擾����
		elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

		const float micro = 1'000'000.0f;
		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//�}�C�N���b��b�ɒP�ʕϊ�
		//0~1
		const float timeRateMax = 1.0f;
		timeRate_ = min(elapsed / maxTime_, timeRateMax);

		if (isBack_)//�����֍s���ꍇ
		{
			//�x�W�F�Ȑ��Ŕ��ł���
			position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);
			if (position_.z >= end_.z)isJumpBack_ = false;
		}
		else//��O���֍s���ꍇ
		{
			//�x�W�F�Ȑ��Ŕ��ł���
			position_ = Bezier3(end_, point2_, point1_, start_, timeRate_);
			if (position_.z <= start_.z)isJumpBack_ = false;
		}
	}

}

void Player::Landing(const unsigned short attribute)
{
	//���R���C�_�[�̎擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	assert(sphereCollider);

	//���@��p�N�G���[�R�[���o�b�N�N���X��`
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		//�R���X�g���N�^
		PlayerQueryCallback(Sphere* sphere) :sphere_(sphere) {};

		//�Փˎ��̃R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info)
		{
			//���[���h�����
			const XMVECTOR up = { 0.0f,1.0f,0.0f,0.0f };
			//�r�˕���
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//������Ɣr�˕����̊p�x���̃R�T�C���l
			float cos = XMVector3Dot(rejectDir, up).m128_f32[XYZ_X];

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
		//��
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
	position_.x += callback.move.m128_f32[XYZ_X];
	position_.y += callback.move.m128_f32[XYZ_Y];
	//position_.z += callback.move.m128_f32[2];
	
	//���_�ƒ����_���Q�b�g
	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();
	//X������������
	eyepos.x += callback.move.m128_f32[XYZ_X];
	tarpos.x += callback.move.m128_f32[XYZ_X];

	//�X�V
	UpdateWorldMatrix();			//�s��X�V
	camera_->SetEye(eyepos);		//���_�Z�b�g
	camera_->SetTarget(tarpos);		//�����_�Z�b�g
	collider_->Update();			//�R���C�_�[�X�V

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[XYZ_Y] += sphereCollider->GetRadius();
	const XMVECTOR rayDir = { 0.0f,-1.0f,0.0f,0.0f };
	ray.dir = rayDir;
	RaycastHit raycastHit;

	//���a�@X�@2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//������ԂɂȂ�Y�l�̃X�s�[�h
	const float fallSpeedY = 0.0f;
	//�ڒn���
	if (onGround_)
	{
		//�X���[�Y�ɍ�����邽�߂̋z������
		const float adsDistance = 0.2f;
		//�ڒn���ێ�
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum + adsDistance))
		{
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
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
	else if (fallVec_.y <= fallSpeedY)
	{
		if (colManager_->RayCast(ray, attribute, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum))
		{
			//���n���Ƀ��f���𒅒n���f����
			model_ = modelPlayer_;
			//���n
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
			//�s��X�V
			Object3d::Update();
		}
	}

	//�I�u�W�F�N�g�X�V
	Object3d::Update();
}

//�U������
void Player::Attack() {

	if (input_->TriggerKey(DIK_L)) {
		//�n�ʂɂ���Ƃ�,�ʏ�̗����G�̂Ƃ�����Ń��f����ς���
		if (model_ == modelPlayer_ && onGround_) model_ = modelAttack_;
		//�e�̑��x
		const float bulletSpeed = 1.0f;
		XMFLOAT3 velocity;
		//�����ɂ���ăX�s�[�h���ς��
		const XMFLOAT3 velLeft = { -bulletSpeed, 0.0f, 0.0f };
		const XMFLOAT3 velRight = { bulletSpeed, 0.0f, 0.0f };
		//�����Ă�������𓯊�
		if (isRight_)velocity = velRight;
		else velocity = velLeft;
		//�s��ɑ��x�l��n��
		const XMFLOAT4 velosityMoveMatrix = { velocity.x,velocity.y,velocity.z,0.0f };
		XMMATRIX matVec = XMMatrixIdentity();
		matVec.r[XYZW_X].m128_f32[XYZW_X] = velosityMoveMatrix.x;
		matVec.r[XYZW_X].m128_f32[XYZW_Y] = velosityMoveMatrix.y;
		matVec.r[XYZW_X].m128_f32[XYZW_Z] = velosityMoveMatrix.z;
		matVec.r[XYZW_X].m128_f32[XYZW_W] = velosityMoveMatrix.w;
		matVec *= Object3d::GetWorld();

		//���L�����̍��W���R�s�[
		XMFLOAT3 pos = Object3d::GetPosition();

		//�e�𐶐���������
		const XMFLOAT3 bulletPositionOffsetRight = { position_.x+0.8f,position_.y + 3.0f,position_.z };
		const XMFLOAT3 bulletPositionOffsetLeft = { position_.x - 0.8f,position_.y + 3.0f,position_.z };
		std::unique_ptr<PlayerBullet> newBullet;
		if(isRight_)newBullet = PlayerBullet::Create(bulletPositionOffsetRight, velocity, modelBullet_,particleBullet_.get(), pmBullet_.get());
		else newBullet = PlayerBullet::Create(bulletPositionOffsetLeft, velocity, modelBullet_, particleBullet_.get(), pmBullet_.get());
		newBullet->SetCamera(camera_);
		newBullet->Update();

		//�e��o�^
		gameScene_->AddPlayerBullet(std::move(newBullet));
		//���郁�����̏��L��������unique_ptr�͂�����������݂ł��Ȃ�
		//���̏��L�������n���邽�߂̋@�\�� std::move()
	}
}

//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
void Player::OnCollision([[maybe_unused]] const CollisionInfo& info, 
	const unsigned short attribute, const unsigned short subAttribute) {

	//�_���[�W�Ǘ��̍\����
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

	//���v���Z�b�g
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

	if (attribute == COLLISION_ATTR_ENEMYS)//�G�̏ꍇ
	{
		//�q�b�g��(���G���Ԏ�)�͌����Ȃ�
		if (isShake_)return;
		if (isHit_)return;
		//�_���[�W�v�Z
		if (subAttribute == SUBCOLLISION_ATTR_NONE)life_ -= damege.enemyNone;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_POWER)life_ -= damege.enemyPower;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_GUARD)life_ -= damege.enemyGuard;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_SPEED)life_ -= damege.enemySpeed;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_DEATH)life_ -= damege.enemyDeath;
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_ISDEAD)return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_ -= damege.enemyBullet;

		//�q�b�g���o
		pmFire_->ActiveZ(fire);
		pmFire_->Update();
		
		//model_ = modelHit_;
		isHit_ = true;
	}

	else if (attribute == COLLISION_ATTR_GIMMICK)//�M�~�b�N�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_GIMMICK_SPIKE)//�g�Q
		{
			//�q�b�g��(���G���Ԏ�)�͌����Ȃ�
			if (isShake_)return;
			if (isHit_)return;
			//�_���[�W�v�Z
			life_ -= damege.GimmickSpike;

			//�q�b�g���o
			pmFire_->ActiveZ(fire.particle, fire.startPos, fire.pos, fire.vel,
				fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
			pmFire_->Update();

			//model_ = modelHit_;
			isHit_ = true;
		}

	}
	else if (attribute == COLLISION_ATTR_GOAL)//�S�[���̏ꍇ
	{
		//���d�q�b�g��h�~
		if (isGoal_)return;
		//���@�̃X�P�[�����C�[�W���O�ŕύX
		scale_ = { easeChangeScaleStageClear_[XYZ_X].start,easeChangeScaleStageClear_[XYZ_Y].start ,easeChangeScaleStageClear_[XYZ_Z].start };
		for (int i = 0; i < XYZ_Num; i++)easeChangeScaleStageClear_[i].Standby(false);
		//�������Ȃ��悤�ɂ���
		stopPos_ = position_;
		isGoal_ = true;
		isAlive_ = false;
	}
	else if (attribute == COLLISION_ATTR_ITEM)//�A�C�e���̏ꍇ
	{
		
		if (subAttribute == SUBCOLLISION_ATTR_ITEM_JUMP)//�W�����v�����A�C�e��
		{
			if (isGetJumpItem_)
			{
				jumpPowerUpcount_ = 0;//���Ԃ����Z�b�g
				return; //���d�q�b�g�h�~
			}
				
			isGetJumpItem_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_ITEM_HEAL)//���C�t�񕜃A�C�e��
		{
			//�񕜂���
			const int heal = 1;
			life_ += heal;
			
		}
		
	}
}

const XMFLOAT3 Player::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	//�O�_�x�W�F�Ȑ��̎�
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
	//�ړ�����
	Move();
	//�W�����v����
	FallAndJump();
	//��O�B�����W�����v����
	if (isBack)JumpBack();
	//�U������
	if (isAttack)Attack();

	if (life_ <= 0)//���C�t��0�ɂȂ����玀��
	{
		//���ݎ��_�ƒ����_���L�^
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();

		const XMFLOAT3 offset = { -10.0f,position_.y,85.0f + position_.z };
		easeOffset_ = offset;//�ŏ��ɃI�t�Z�b�g�𑫂��Ȃ��ƉE�J�����A1�񑫂��ƒ����J�����A2�񑫂����獶�J����
		const float easeTime = 1.0f;
		//�E�J�������_[i]
		easeDeadCameraEye_[XYZ_X].SetEasing(nowEye_.x, nowEye_.x + easeOffset_.x, easeTime);
		easeDeadCameraEye_[XYZ_Y].SetEasing(nowEye_.y, nowEye_.y + easeOffset_.y, easeTime);
		easeDeadCameraEye_[XYZ_Z].SetEasing(nowEye_.z, nowEye_.z + easeOffset_.z, easeTime);
		//�E�J���������_[i]
		easeDeadCameraTarget_[XYZ_X].SetEasing(nowTarget_.x, nowTarget_.x + easeOffset_.x, easeTime);
		easeDeadCameraTarget_[XYZ_Y].SetEasing(nowTarget_.y, nowTarget_.y + easeOffset_.y, easeTime);
		easeDeadCameraTarget_[XYZ_Z].SetEasing(nowTarget_.z, nowTarget_.z + easeOffset_.z, easeTime);
		//�C�[�W���O�X�^���o�C
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraEye_[i].Standby(false);
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraTarget_[i].Standby(false);
		//���f����؂�ւ��Ď��S���o��
		model_ = modelHit_;
		isBreak_ = true;
		isAlive_ = false;
	}
	//���̈ʒu�܂ŗ����Ă�����
	const float deadPosY = -60.0;
	if (position_.y <= deadPosY)isDead_ = true;

	if (isHit_)//�G�̍U��������������
	{
		nowEye_ = camera_->GetEye();
		nowTarget_ = camera_->GetTarget();
		easeHit_.Standby(false);
		isShake_ = true;
		isHit_ = false;
	}
	if (isShake_)
	{
		//����������V�F�C�N������
		const int32_t shakeCount = 1;
		//���_�V�F�C�N
		XMFLOAT3 Eye = nowEye_ + hitMove_;
		const XMFLOAT3 hitEye = { 0.5f,0.5f,0.5f };
		camera_->ShakeEye(Eye, shakeCount, { Eye.x - hitEye.x,Eye.y - hitEye.y,Eye.z - hitEye.z },
			{ Eye.x + hitEye.x,Eye.y + hitEye.y,Eye.z + hitEye.z });

		//�����_�V�F�C�N
		XMFLOAT3 Target = nowTarget_ + hitMove_;
		const XMFLOAT3 hitTarget = { 0.5f,0.5f,0.5f };
		camera_->ShakeTarget(Target, shakeCount, { Target.x - hitTarget.x,Target.y - hitTarget.y,Target.z - hitTarget.z },
			{ Target.x + hitTarget.x,Target.y + hitTarget.y,Target.z + hitTarget.z });
		camera_->Update();

		//+���ăC�[�W���O
		easeHit_.ease_in_out_cubic();
		spriteHit_->SetColor({ hitColor_.x,hitColor_.y,hitColor_.z ,easeHit_.num_X });
		//���G���ԃJ�E���g��i�߂�
		mutekiCount_++;
	}
	//���G���ԃJ�E���g�����̎��Ԃɂ܂ŒB������V�F�C�N�I��
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
	if (isExplosion_)//�X�v���C�g�������I�������
	{
		//�C�[�W���O�ŏ��X�ɔ����X�v���C�g�𔖂����Ă����ŏI�I�ɂ͓����ɂ���
		easeExplosionSizeAndAlpha_[XYW_W].ease_out_sine();
		spriteExplosion_->SetColor({ asIsColor_.x,asIsColor_.y,asIsColor_.z,easeExplosionSizeAndAlpha_[XYW_W].num_X });
		//���v���Z�b�g
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
		//�唚��
		pmFire_->ActiveY(fire);

		//���v���Z�b�g
		const ParticleManager::Preset smoke =
		{
			particleSmoke_.get(),
			{position_.x,position_.y + 5.0f,position_.z},
			{ 25.0f ,10.0f,15.0f },
			{ MyMath::RandomMTFloat(0.0f,0.3f),MyMath::RandomMTFloat(0.5f,3.0f),0.3f },
			{ 0.0f,0.001f,0.0f },
			5,
			{ 0.0f,3.0f },
			{1.0f,1.0f,1.0f,1.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};

		//��������
		pmSmoke_->ActiveY(smoke);

		//�J�������x
		XMFLOAT3 cameraSppedEyeTarget;
		const XMFLOAT2 shakeEyeTargetMinMax = { -0.1f,0.1f };//���_�A�����_��XY���W�̓V�F�C�N������
		//�V�F�C�N�̒l�̓����_��
		cameraSppedEyeTarget.x = MyMath::RandomMTFloat(shakeEyeTargetMinMax.x, shakeEyeTargetMinMax.y);
		cameraSppedEyeTarget.y = MyMath::RandomMTFloat(shakeEyeTargetMinMax.x, shakeEyeTargetMinMax.y);

		const float speedChangePosZ = -30.0f;//��O���A�����ɕ������̒l(������傫�������牜��)
		const XMFLOAT2 cameraSpeedZ = { 1.0f,0.5f };//�����Ǝ�O���̎��_�A�����_��Z���ړ��̑��x
		//�����Ǝ�O���ŃY�[���A�E�g���x��ς���
		//����
		if (position_.z >= speedChangePosZ) cameraSppedEyeTarget.z = cameraSpeedZ.x;
		//��O��
		else cameraSppedEyeTarget.z = cameraSpeedZ.y;

		//���X�ɃJ�����̓Y�[���A�E�g
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
		//Z���_�����̈ʒu�ɒB������Q�[���I�[�o�[
		if (camera_->GetEye().z <= -cameraEyeChangeGameover_) isDead_ = true;
	}
	else if (isCameraEnd_)//�J�����ړ��I������Ƃ�
	{
		//����
		for (int i = 0; i < XY_Num; i++)easeExplosionSizeAndAlpha_[i].ease_out_expo();
		//�X�v���C�g�Ŕ�����\��
		spriteExplosion_->SetSize({ easeExplosionSizeAndAlpha_[XYW_X].num_X,easeExplosionSizeAndAlpha_[XYW_Y].num_X });
		//�T�C�Y���C�[�W���O�I���T�C�Y�܂ŒB������p�[�e�B�N���̔����A���̏o��
		if (spriteExplosion_->GetSize().x == easeExplosionSizeAndAlpha_[XYW_X].end)
		{
			easeExplosionSizeAndAlpha_[XYW_W].Standby(false);//�J���[����
			isExplosion_ = true;
			isCameraEnd_ = false;
		}
	}
	else//���C�t��0�ɂȂ����u��
	{
		//�J�����ړ�
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraEye_[i].ease_out_cubic();
		for (int i = 0; i < XYZ_Num; i++)easeDeadCameraTarget_[i].ease_out_cubic();

		camera_->SetEye({ easeDeadCameraEye_[XYZ_X].num_X,easeDeadCameraEye_[XYZ_Y].num_X, easeDeadCameraEye_[XYZ_Z].num_X });
		camera_->SetTarget({ easeDeadCameraTarget_[XYZ_X].num_X,easeDeadCameraTarget_[XYZ_Y].num_X, easeDeadCameraTarget_[XYZ_Z].num_X });
		//�J�����̃C�[�W���O���I������甚��
		if (camera_->GetEye().z == easeDeadCameraEye_[XYZ_Z].end)
		{
			for (int i = 0; i < XY_Num; i++)easeExplosionSizeAndAlpha_[i].Standby(false);//�T�C�Y����
			isCameraEnd_ = true;
		}
	}

}

void Player::UpdateGoal()
{
	//���W���Œ肵�ăX�P�[�����C�[�W���O
	position_ = stopPos_;
	for (int i = 0; i < XYZ_Num; i++)easeChangeScaleStageClear_[i].ease_out_cubic();
	scale_ = { easeChangeScaleStageClear_[XYZ_X].num_X,easeChangeScaleStageClear_[XYZ_Y].num_X ,easeChangeScaleStageClear_[XYZ_Z].num_X };

}
