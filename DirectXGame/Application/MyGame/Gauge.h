#pragma once
#include "Sprite.h"
#include "Easing.h"

class Gauge {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;

public: //�萔
		//�f�t�H���g�e�N�X�`���T�C�Y
	const float kDefaultTexSize = 64.0f;

public: //�����o�֐�
	void Initialize();
	void Update ();
	void Draw();
	void Finalize();

	//�Q�[�W�ϓ�
	void Fluctuation();

	//�ϓ��ʌ���
	void DecisionFluctuation();

	//�z�u�̍Đݒ�
	void ResetSize();

	//�z�u�̍Đݒ�
	void ResetArrangement();

public: //�A�N�Z�b�T
		//�T�C�Y�̎擾
	const XMFLOAT2& GetSize() const {
		return sizeGauge_;
	};
	//�T�C�Y�̃Z�b�g
	void SetSize(XMFLOAT2 const size) {
		sizeGauge_ = size;
		ResetSize();
	};

	//�ʒu(����p)�̎擾
	const XMFLOAT2& GetPosition() const {
		return positionGauge_;
	};
	//�ʒu(����p)�̃Z�b�g
	void SetPosition(XMFLOAT2 const position) {
		positionGauge_ = position;
		ResetArrangement();
	};

	//�c�ʃX�v���C�g�̎擾
	Sprite* GetRestSprite() {
		return spriteRest_;
	};
	//���X�v���C�g�̎擾
	Sprite* GetLeftSprite() {
		return spriteLeft_;
	};
	//�E�X�v���C�g�̎擾
	Sprite* GetRightSprite() {
		return spriteRight_;
	};
	//�Q�[�W�X�v���C�g�̎擾
	Sprite* GetGaugeSprite() {
		return spriteGauge_;
	};

	//�ϓ��t���O�̎擾
	bool IsFluct() {
		return isFluct_;
	};

	//�ϓ��t���O�̃Z�b�g
	void SetIsFluct(bool isFluct) {
		isFluct_ = isFluct;
	}

	//�c�ʂ̎擾
	float GetRest() {
		return rest_;
	}

	//�c�ʏ���̃Z�b�g
	void SetRestMax(float restMax) {
		restMax_ = restMax;
	}

	//�c�ʂ̃Z�b�g
	void SetRest(float rest) {
		rest_ = rest;
	}

	//�ő厞�Ԃ̎擾
	float GetMaxTime() {
		return maxTime_;
	}

	//�ő厞�Ԃ̃Z�b�g
	void SetMaxTime(float maxTime) {
		maxTime_ = maxTime;
	}

private: //�����o�ϐ�
		 //�c��
	float rest_ = 0.0f;

	//�c�ʂ̏��
	float restMax_ = 0.0f;

	//�Q�[�W�̃T�C�Y�{��
	XMFLOAT2 sizeGauge_ = {
		1.0f,
		1.0f 
	};

	//�Q�[�W�̈ʒu(����p)
	XMFLOAT2 positionGauge_ = {
		0,
		0 
	};

	//�Q�[�W���[�p�X�v���C�g
	Sprite* spriteLeft_ = nullptr;
	//�Q�[�W�E�[�p�X�v���C�g
	Sprite* spriteRight_ = nullptr;
	//�Q�[�W�p�X�v���C�g
	Sprite* spriteGauge_ = nullptr;
	//�c�ʗp�X�v���C�g
	Sprite* spriteRest_ = nullptr;

	//�ʒu�C�[�W���O�n���_
	XMFLOAT2 startEasePosition_{};
	//�ʒu�C�[�W���O�I���_
	XMFLOAT2 endEasePosition_{};

	//�T�C�Y�C�[�W���O�n���_
	XMFLOAT2 startEaseSize_{};
	//�T�C�Y�C�[�W���O�I���_
	XMFLOAT2 endEaseSize_{};

	//�C�[�W���O�ő厞��
	float maxTime_ = 30.0f;
	//�C�[�W���O�o�ߎ���
	float nowTime_ = 0;

	//�C�[�W���O���s�t���O
	bool isFluct_ = false;

private:
	//�J�n���ƏI�����������
	const float EaseInOut(
		const float& start,
		const float& end,
		const float t);
};