#pragma once
#include "Easing.h"
#include "Input.h"
#include "Sprite.h"
#include "Model.h"
#include "Object3d.h"
#include "PlayerBullet.h"
#include "ParticleManager.h"
#include "Easing.h"

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
public:
	//�f�X�g���N�^
	~Player();
	//����(�g�p���f���A�g�p�e���f���A�Q�[���v���C�V�[��)
	static std::unique_ptr<Player> Create(Model* model = nullptr, Model* bullet = nullptr,
		GamePlayScene* gamescene = nullptr);
	//������
	bool Initialize() override;
	//���Z�b�g����
	void Reset();

	//�X�V(��O�Ɣw�ʃW�����v�̗L�����A�U�������̗L�����A�J�n�����o���ƑS���얳��)
	void Update(bool isBack = true, bool isAttack = true, bool isStart = false);
	//�v���C���[�̈ړ�����
	void Move();

	//�����鏈���ƃW�����v����
	void FallAndJump();
	//���ֈړ�
	void JumpBack();
	//���n(�����w��)
	void Landing(unsigned short attribute);
	//�v���C���[�̍U������
	void Attack();

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();
	//�X�v���C�g�`��
	void DrawSprite();
	//�p�[�e�B�N���`��
	void DrawParticle();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

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
	//�X�v���C�g
	SpriteCommon* spCommon_ = nullptr;//���

	Sprite* spriteLifeBar_ = new Sprite();			//���C�t�o�[
	const XMFLOAT2 lifeBarPos_ = { 800.0f,25.0f };	//���C�t�o�[�̍��W
	const XMFLOAT4 green_ = { 0.2f,1.0f,0.2f,1.0f };//��
	const XMFLOAT4 red_ = { 1.0f,0.2f,0.2f,1.0f };	//��
	const float lifeBarDamageSize_ = 32.0f;			//�_���[�W�𕉂��Ƃ��̒l���T�C�Y���k��

	Sprite* spriteHit_ = new Sprite();				//�q�b�g�G�t�F�N�g

	int mutekiCount_ = 0;							//���G����
	const float hitTimer_ = MUTEKI_COUNT / 60.0f;	//�C�[�W���O���ʎ���
	Easing easeHit_ = Easing(1.0f, 0.0f, hitTimer_);//�C�[�W���O
	const XMFLOAT4 hitColor_ = { 0.5f,0.0f,0.0f,0.0f };	//�Ԃ�����

	//���f��
	Model* modelBullet_ = nullptr;

	//�C���v�b�g
	Input* input_ = nullptr;

	//�A���O��
	XMFLOAT3 angle_;

	//�W�����v���Ă邩
	bool onGround_ = true;
	//�W�����v��
	float jumpVYFist_ = 2.0f;
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
	float	maxTime_ = 1.0f;							//�S�̎���
	float	timeRate_;									//�ǂꂭ�炢���Ԃ��i�񂾂�
	//����_
	XMFLOAT3 start_;									//�ŏ��_
	XMFLOAT3 point1_;									//���ԓ_1
	XMFLOAT3 point2_;									//���ԓ_2
	XMFLOAT3 end_;										//�ŏI�_

	//���a
	float radius_ = 2.0f;

	//���S���o�ڍs�t���O
	bool isBrack_ = false;

	//���S�t���O
	bool isDead_ = false;

	//���C�t
	int life_ = 10;
	//�G�̍U����g�Q���ɓ���������
	bool isHit_ = false;
	//�V�F�C�N���p�̃J�������[�u
	bool cameramove_ = true;
	//�p�[�e�B�N��
	Particle* particleDash_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pmDash_ = nullptr;

	//�Q�[���V�[��
	GamePlayScene* gameScene_ = nullptr;
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

public: //�A�N�Z�b�T�A�C�����C���֐�
	//���񂾂��ǂ���
	bool IsDead() const { return isDead_; }
	//�����Ă邩�ǂ���
	bool OnGround()const { return onGround_; }
	//��������̃Z�b�g
	void SetOnGround(bool og) { this->onGround_ = og; }

	//�W�����v�̓Z�b�g
	void SetJumpVYFist(float jumpFist) { this->jumpVYFist_ = jumpFist; }
	//���C�t�Z�b�g
	void SetLife(int life) { this->life_ = life; }
	//���C�t�Q�b�g
	const int& GetLife()const { return life_; }
	//�Q�[���V�[��
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }

private://�J�v�Z���������o�֐�
	void UpdateAlive(bool isBack = true, bool isAttack = true);

};
