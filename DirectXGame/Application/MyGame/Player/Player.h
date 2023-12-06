#pragma once
#include "Easing.h"
#include "Input.h"
#include "Sprite.h"
#include "Model.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include "ParticleManager.h"
#include "Easing.h"
#include "XYZ.h"

#include <DirectXMath.h>
#include <list>
#include <memory>
#include <chrono>

//�O���錾
//�R���W�����}�l�[�W���[
class CollisionManager;
//�Q�[���v���C�V�[��
class GamePlayScene;

/*

*	Player.h

*	���@

*/
class Player :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�ő喳�G����
	const int MUTEKI_COUNT = 60;
	//�W�����v�����ő���ʎ���
	const float JUMPITEM_MAX_TIME = 200.0f;

	//�����ɓn���\����
	struct PlayerModelList
	{
		Model* playerModel = nullptr;
		Model* playerBullet = nullptr;
		Model* playerHit = nullptr;
		Model* playerMove = nullptr;
	};

public:
	//�f�X�g���N�^
	~Player();
	//����(�g�p���f���A�Q�[���v���C�V�[��)
	static std::unique_ptr<Player> Create(const PlayerModelList* model = nullptr,GamePlayScene* gamescene = nullptr);
	//������
	bool Initialize() override;
	//���Z�b�g����
	void Reset();

	//�X�V(��O�Ɣw�ʃW�����v�̗L�����A�U�������̗L�����A�J�n�����o���ƑS���얳��)
	void Update(const bool isBack = true, const bool isAttack = true, const bool isStart = false);
	//�v���C���[�̈ړ�����
	void Move();

	//�����鏈���ƃW�����v����
	void FallAndJump();
	//���ֈړ�
	void JumpBack();
	//���n(�����w��)
	void Landing(const unsigned short attribute);
	//�v���C���[�̍U������
	void Attack();

	//�]��
	void Trans();

	//���[���h���W���擾
	const XMFLOAT3 GetWorldPosition()const ;

	//�`��
	void Draw();
	//�X�v���C�g�`��
	void DrawSprite();
	//�p�[�e�B�N���`��
	void DrawParticle();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)override;

	//�x�W�F�Ȑ�(�ŏ��_�A���ԓ_1�A���ԓ_2�A�ŏI�_�A���Ԃ̐i�݋)
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//�e���X�g���擾
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	//�R���W�����}�l�[�W���[
	static CollisionManager* colManager_;
	//�e
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	//�E�������Ă邩
	bool isRight_ = true;
	//�U������p�̃C�[�W���O�̃v���Z�b�g
	const Easing presetEaseRotateRightY_ = {-90.0f,90.0f,0.2f};//Y������@���A�E
	//�U������p�̃C�[�W���O
	Easing easeRotateRightY_ = presetEaseRotateRightY_;
	//�ŏ��͕�����ς��Ȃ�
	bool startGameRotate_ = false;	//�Ăяo�����͍̂ŏ�����
	//�X�v���C�g
	SpriteCommon* spCommon_ = nullptr;//���

	std::unique_ptr<Sprite> spriteLifeBar_ = std::make_unique<Sprite>();			//���C�t�o�[
	const XMFLOAT2 lifeBarPos_ = { 800.0f,25.0f };	//���C�t�o�[�̍��W
	const XMFLOAT4 green_ = { 0.2f,1.0f,0.2f,1.0f };//��
	const XMFLOAT4 red_ = { 1.0f,0.2f,0.2f,1.0f };	//��

	Easing easelifeBarSize_ = { 0.0f,32.0f,3.0f };			//���C�t�𒙂߂�C�[�W���O
	XMFLOAT2 lifeBarDamageSize_ = { 0.0f,32.0f };			//�_���[�W�𕉂��Ƃ��̒l���T�C�Y���k��

	std::unique_ptr<Sprite> spriteHit_ = std::make_unique<Sprite>();				//�q�b�g�G�t�F�N�g

	std::unique_ptr<Sprite> spriteExplosion_ = std::make_unique<Sprite>();		//�����G�t�F�N�g
	const XMFLOAT2 explosionPos_ = { WinApp::WINDOW_WIDTH / 2.0f,WinApp::WINDOW_HEIGHT / 2.0f };	//���C�t�o�[�̍��W
	//�����͒����A���J�[�|�C���g
	const XMFLOAT2 explosionAnchorPoint_ = { 0.5f,0.5f };

	int32_t mutekiCount_;							//���G����
	const float hitTimer_ = MUTEKI_COUNT / 60.0f;	//�C�[�W���O���ʎ���
	Easing easeHit_ = Easing(1.0f, 0.0f, hitTimer_);//�C�[�W���O
	const XMFLOAT4 hitColor_ = { 0.5f,0.0f,0.0f,0.0f };	//�Ԃ�����

	//���f��
	const Model* modelPlayer_ = nullptr;
	const Model* modelBullet_ = nullptr;
	const Model* modelHit_ = nullptr;
	const Model* modelMove_ = nullptr;
	//�C���v�b�g
	Input* input_ = nullptr;

	//�A���O��
	XMFLOAT3 angle_;

	//�W�����v���Ă邩
	bool onGround_ = true;
	//�W�����v��
	float jumpVYFist_;
	//�������x�N�g��
	XMFLOAT3 fallVec_;

	//��O�A�����Ɉړ�
	bool isJumpBack_;
	//�����ɂ���ꍇ
	bool isBack_;
	//��O�A�����W�����v���̃|�W�V����
	XMFLOAT3 jumpBackPos_;

	//���Ԍv��
	std::chrono::steady_clock::time_point startCount_;	//�J�n����
	std::chrono::steady_clock::time_point nowCount_;	//���ݎ���
	std::chrono::microseconds elapsedCount_;			//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	const float	maxTime_ = 1.0f;						//�S�̎���
	float	timeRate_;									//�ǂꂭ�炢���Ԃ��i�񂾂�
	//����_
	XMFLOAT3 start_;									//�ŏ��_
	XMFLOAT3 point1_;									//���ԓ_1
	XMFLOAT3 point2_;									//���ԓ_2
	XMFLOAT3 end_;										//�ŏI�_

	//���a
	const float radius_ = 1.0f;

	//���S���o�ڍs�t���O
	bool isBreak_ = false;

	//�����Ă�
	bool isAlive_ = true;
	//���S�t���O
	bool isDead_ = false;

	//���C�t
	int life_;
	//�G�̍U����g�Q���ɓ���������
	bool isHit_ = false;

	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

	//�Q�[���V�[��
	GamePlayScene* gameScene_;
	//�V�F�C�N�@�\
	bool isShake_ = false;
	//���ݎ��_
	XMFLOAT3 nowEye_;
	//���ݒ����_
	XMFLOAT3 nowTarget_;
	//�V�F�C�N���ɃX�N���[�����Ă����̃J�������_�A�����_�ɖ߂邽�߂̍��W
	XMFLOAT3 hitMove_;
	//�V�F�C�N�I������hitMove�����Z�b�g����p�̒萔
	const XMFLOAT3 resetHitMove_ = { 0.0f,0.0f,0.0f };

	//���񂾂Ƃ��̃J�������_�C�[�W���O
	Easing easeDeadCameraEye_[XYZ_Num];		//X,Y,Z
	//���񂾂Ƃ��̃J���������_�C�[�W���O
	Easing easeDeadCameraTarget_[XYZ_Num];	//X,Y,Z
	//�J�����؂�ւ�
	bool isCameraEnd_ = false;							//�J����
	bool isExplosion_ = false;							//�������Ă邩
	const float cameraEyeChangeGameover_ = 150.0f;		//���_������ʒu�ɂ�����Q�[���I�[�o�[

	//�����̃T�C�Y�ƃA���t�@�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseExplosionSize_[XY_Num] = { {0.0f,1500.0f,1.0f},{0.0f,1500.0f,1.0f} };//�T�C�Y(�n�_�A�I�_�A�����鎞��)
	const Easing presetEaseExplosionAlpha_ = { 1.0f,0.0f,2.0f };//�A���t�@�l(�n�_�A�I�_�A�����鎞��)
	//�����̃T�C�Y�ƃA���t�@�C�[�W���O
	Easing easeExplosionSizeAndAlpha_[XYW_Num] = {
		presetEaseExplosionSize_[XY_X],		//X
		presetEaseExplosionSize_[XY_Y],		//Y
		presetEaseExplosionAlpha_			//�A���t�@
	};

	//�X�e�[�W�N���A���̃v���C���[�̃X�P�[����ύX�̃v���Z�b�g
	const Easing presetEaseChangeScaleStageClear_[XYZ_Num] =
	{
		{1.0f,0.0f,1.5f},			//X
		{1.0f,2.0f,1.5f},			//Y
		{1.0f,0.0f,1.5f}			//Z
	};
	//�X�e�[�W�N���A���̃v���C���[�̃X�P�[����ύX
	Easing easeChangeScaleStageClear_[XYZ_Num] = {
		presetEaseChangeScaleStageClear_[XYZ_X],	//X
		presetEaseChangeScaleStageClear_[XYZ_Y],	//Y
		presetEaseChangeScaleStageClear_[XYZ_Z]		//Z
	};

	//�S�[��������
	bool isGoal_ = false;
	//���W�̒�~
	XMFLOAT3 stopPos_;

	//�C�[�W���O�p�̃I�t�Z�b�g
	XMFLOAT3 easeOffset_ = {};

	//�f�ނ��̂܂܂̐F
	const XMFLOAT3 asIsColor_ = { 1.0f,1.0f,1.0f };
	
	//�A�C�e���֌W
	//�񕜃A�C�e���Q�b�g
	bool isGetHealItem_ = false;
	//�W�����v�A�C�e���Q�b�g
	bool isGetJumpItem_ = false;
	//�W�����v�A�C�e�����ʎ���
	float jumpPowerUpcount_;
	

public: //�A�N�Z�b�T�A�C�����C���֐�
	//���񂾂��ǂ���
	bool IsDead() const { return isDead_; }
	//���S���o�ڍs��
	bool IsBreak()const { return isBreak_; }
	//�����Ă邩�ǂ���
	bool OnGround()const { return onGround_; }
	//��������̃Z�b�g
	void SetOnGround(const bool og) { this->onGround_ = og; }
	//���C�t�Z�b�g
	void SetLife(const int life) { this->life_ = life; }
	//���C�t�Q�b�g
	const int& GetLife()const { return life_; }
	//�Q�[���V�[���Z�b�g
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }


private://�J�v�Z���������o�֐�
	//������
	void UpdateAlive(const bool isBack = true, const bool isAttack = true);
	//�j��
	void UpdateBreak();
	//�S�[����
	void UpdateGoal();
};
