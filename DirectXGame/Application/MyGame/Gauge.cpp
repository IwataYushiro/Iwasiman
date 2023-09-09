#include "Gauge.h"

void Gauge::Initialize() {
	float texSize = 64.0f;
	SpriteCommon::GetInstance()->
		LoadTexture(20, "texture/gauge.png");

#pragma region 残量スプライト
	spriteRest_ = new Sprite();
	spriteRest_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteRest_->SetTextureSize({ texSize, texSize });
	//ゲージ画像の右下に当たる
	spriteRest_->SetTextureLeftTop({ 64,64 });
	spriteRest_->SetColor({ 0.2f,0.7f,0.2f,1.0f });
#pragma endregion

#pragma region 中腹枠スプライト
	spriteGauge_ = new Sprite();
	spriteGauge_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteGauge_->SetTextureSize({ texSize / 2, texSize });
	//ゲージ画像の左上から、やや右にズレた箇所に当たる
	spriteGauge_->SetTextureLeftTop({ 16,64 });
#pragma endregion

#pragma region 左端スプライト
	spriteLeft_ = new Sprite();
	spriteLeft_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteLeft_->SetTextureSize({ texSize, texSize });
	//ゲージ画像の左上に当たる
	spriteLeft_->SetTextureLeftTop({ 0,0 });
#pragma endregion

#pragma region 右端枠スプライト
	spriteRight_ = new Sprite();
	spriteRight_->Initialize(SpriteCommon::GetInstance(), 20);
	spriteRight_->SetTextureSize({ texSize, texSize });
	//ゲージ画像の右上に当たる
	spriteRight_->SetTextureLeftTop({ 64,0 });
#pragma endregion

#pragma region 各アンカーポイント
	//※残量スプライトだけ違うのは苦渋の決断によるもの
	spriteRest_->SetAnchorPoint({ 0.0f,0.5f });
	spriteGauge_->SetAnchorPoint({ 0.5f,0.5f });
	spriteLeft_->SetAnchorPoint({ 0.5f,0.5f });
	spriteRight_->SetAnchorPoint({ 0.5f,0.5f });
#pragma endregion

	ResetSize();
	ResetArrangement();
}

void Gauge::Update() {
	//フラグ立ってるならHPゲージを変動させる
	if (isFluct_) {
		Fluctuation();

		//終了時間に到達したら、タイマーリセット
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
	//ライフが0以下なら
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
	//アンカーが他とは違う上で、ゲージ全体の左上に位置を合わせる
	spriteRest_->SetPosition({
		positionGauge_.x - ((kDefaultTexSize / 2) * sizeGauge_.x),
		positionGauge_.y
		});

	//ゲージ中腹は、左端から2枚目の位置が、左上位置とする
	spriteGauge_->SetPosition({
		positionGauge_.x + (kDefaultTexSize * sizeGauge_.x * 2),
		positionGauge_.y
		});

	//ゲージ左端はゲージの左上位置に合わせる
	spriteLeft_->SetPosition({
		positionGauge_.x ,
		positionGauge_.y
		});

	//ゲージの右端は一番右に配置されるように合わせる
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
