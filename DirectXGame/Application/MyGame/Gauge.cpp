#include "Gauge.h"

void Gauge::Initialize() {
	float texSize = 64.0f;
	SpriteCommon::GetInstance()->
		LoadTexture(20, "texture/gauge.png");

#pragma region �c�ʃX�v���C�g
	spriteRest_ = new Sprite();
	spriteRest_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteRest_->SetTextureSize({ texSize, texSize });
	//�Q�[�W�摜�̉E���ɓ�����
	spriteRest_->SetTextureLeftTop({ 64,64 });
	spriteRest_->SetColor({ 0.2f,0.7f,0.2f,1.0f });
#pragma endregion

#pragma region �����g�X�v���C�g
	spriteGauge_ = new Sprite();
	spriteGauge_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteGauge_->SetTextureSize({ texSize / 2, texSize });
	//�Q�[�W�摜�̍��ォ��A���E�ɃY�����ӏ��ɓ�����
	spriteGauge_->SetTextureLeftTop({ 16,64 });
#pragma endregion

#pragma region ���[�X�v���C�g
	spriteLeft_ = new Sprite();
	spriteLeft_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteLeft_->SetTextureSize({ texSize, texSize });
	//�Q�[�W�摜�̍���ɓ�����
	spriteLeft_->SetTextureLeftTop({ 0,0 });
#pragma endregion

#pragma region �E�[�g�X�v���C�g
	spriteRight_ = new Sprite();
	spriteRight_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteRight_->SetTextureSize({ texSize, texSize });
	//�Q�[�W�摜�̉E��ɓ�����
	spriteRight_->SetTextureLeftTop({ 64,0 });
#pragma endregion

#pragma region �e�A���J�[�|�C���g
	//���c�ʃX�v���C�g�����Ⴄ�̂͋�a�̌��f�ɂ�����
	spriteRest_->SetAnchorPoint({ 0.0f,0.5f });
	spriteGauge_->SetAnchorPoint({ 0.5f,0.5f });
	spriteLeft_->SetAnchorPoint({ 0.5f,0.5f });
	spriteRight_->SetAnchorPoint({ 0.5f,0.5f });
#pragma endregion

	ResetSize();
	ResetArrangement();
}

void Gauge::Update() {
	//�t���O�����Ă�Ȃ�HP�Q�[�W��ϓ�������
	if (isFluct_) {
		Fluctuation();

		//�I�����Ԃɓ��B������A�^�C�}�[���Z�b�g
		if (nowTime_ >= maxTime_) {
			isFluct_ = false;
		}
		else {
			nowTime_++;
		}
	}
	else {
		nowTime_ = 0;
	}

	spriteGauge_->Update();
	spriteLeft_->Update();
	spriteRight_->Update();
	spriteRest_->Update();
}

void Gauge::Draw() {
	spriteRest_->Draw();
	spriteGauge_->Draw();
	spriteLeft_->Draw();
	spriteRight_->Draw();
}

void Gauge::Finalize() {
	delete spriteLeft_;
	delete spriteRight_;
	delete spriteGauge_;
	delete spriteRest_;
}

void Gauge::Fluctuation() {
	XMFLOAT2 reSize = {
		EaseInOut(
		startEaseSize_.x,
		endEaseSize_.x,
		nowTime_ / maxTime_),

		EaseInOut(
			startEaseSize_.y,
			endEaseSize_.y,
			nowTime_ / maxTime_),
	};

	XMFLOAT2 move = {
		EaseInOut(
		startEasePosition_.x,
		endEasePosition_.x,
		nowTime_ / maxTime_),

		EaseInOut(
			startEasePosition_.y,
			endEasePosition_.y,
			nowTime_ / maxTime_),
	};

	spriteRest_->SetSize({ reSize.x,reSize.y });
	spriteRest_->SetPosition({ move.x,move.y });
}

void Gauge::DecisionFluctuation() {
	//���C�t��0�ȉ��Ȃ�
	if (rest_ <= 0) {
		startEaseSize_ = {
			spriteRest_->GetSize().x,
			spriteRest_->GetSize().y
		};
		endEaseSize_ = {
			0,
			(kDefaultTexSize * sizeGauge_.y)
		};

		startEasePosition_ = {
			spriteRest_->GetPosition().x - sizeGauge_.x,
			spriteRest_->GetPosition().y
		};

		endEasePosition_ = {
			positionGauge_.x - ((kDefaultTexSize / 2) * sizeGauge_.x),
			positionGauge_.y
		};
	}
	else {
		startEaseSize_ = {
			spriteRest_->GetSize().x,
			spriteRest_->GetSize().y
		};
		endEaseSize_ = {
			(kDefaultTexSize * sizeGauge_.x * 5) * (rest_ / restMax_),
			(kDefaultTexSize * sizeGauge_.y)
		};

		startEasePosition_ = {
			spriteRest_->GetPosition().x,
			spriteRest_->GetPosition().y
		};

		endEasePosition_ = {
			positionGauge_.x - ((kDefaultTexSize / 2) * sizeGauge_.x),
			positionGauge_.y
		};
	}
}

void Gauge::ResetSize() {
	spriteRest_->SetSize({
		(kDefaultTexSize * sizeGauge_.x * 5) * (rest_ / restMax_),
		kDefaultTexSize * sizeGauge_.y
		});
	spriteRest_->SetColor({
		0.2f,0.7f,0.2f,1.0f
		});
	spriteGauge_->SetSize({
		kDefaultTexSize * sizeGauge_.x * 4,
		kDefaultTexSize * sizeGauge_.y
		});
	spriteLeft_->SetSize({
		kDefaultTexSize * sizeGauge_.x,
		kDefaultTexSize * sizeGauge_.y
		});
	spriteRight_->SetSize({
		kDefaultTexSize * sizeGauge_.x,
		kDefaultTexSize * sizeGauge_.y
		});
}

void Gauge::ResetArrangement() {
	//�A���J�[�����Ƃ͈Ⴄ��ŁA�Q�[�W�S�̂̍���Ɉʒu�����킹��
	spriteRest_->SetPosition({
		positionGauge_.x - ((kDefaultTexSize / 2) * sizeGauge_.x),
		positionGauge_.y
		});

	//�Q�[�W�����́A���[����2���ڂ̈ʒu���A����ʒu�Ƃ���
	spriteGauge_->SetPosition({
		positionGauge_.x + (kDefaultTexSize * sizeGauge_.x * 2),
		positionGauge_.y
		});

	//�Q�[�W���[�̓Q�[�W�̍���ʒu�ɍ��킹��
	spriteLeft_->SetPosition({
		positionGauge_.x ,
		positionGauge_.y
		});

	//�Q�[�W�̉E�[�͈�ԉE�ɔz�u�����悤�ɍ��킹��
	spriteRight_->SetPosition({
		positionGauge_.x + (kDefaultTexSize * sizeGauge_.x * 4),
		positionGauge_.y
		});
}

const float Gauge::EaseInOut(
	const float& start,
	const float& end,
	const float t) {
	float a = t * t;
	float b = 3.0f - (2.0f * t);

	//ease-in,ease-out
	float y = a * b;
	return start * (1.0f - y) + end * y;
}
