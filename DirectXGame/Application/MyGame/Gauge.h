#pragma once
#include "Sprite.h"
#include "Easing.h"

class Gauge {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;

public: //定数
		//デフォルトテクスチャサイズ
	const float kDefaultTexSize = 64.0f;

public: //メンバ関数
	void Initialize();
	void Update ();
	void Draw();
	void Finalize();

	//ゲージ変動
	void Fluctuation();

	//変動量決め
	void DecisionFluctuation();

	//配置の再設定
	void ResetSize();

	//配置の再設定
	void ResetArrangement();

public: //アクセッサ
		//サイズの取得
	const XMFLOAT2& GetSize() const {
		return sizeGauge_;
	};
	//サイズのセット
	void SetSize(XMFLOAT2 const size) {
		sizeGauge_ = size;
		ResetSize();
	};

	//位置(左上角)の取得
	const XMFLOAT2& GetPosition() const {
		return positionGauge_;
	};
	//位置(左上角)のセット
	void SetPosition(XMFLOAT2 const position) {
		positionGauge_ = position;
		ResetArrangement();
	};

	//残量スプライトの取得
	Sprite* GetRestSprite() {
		return spriteRest_;
	};
	//左スプライトの取得
	Sprite* GetLeftSprite() {
		return spriteLeft_;
	};
	//右スプライトの取得
	Sprite* GetRightSprite() {
		return spriteRight_;
	};
	//ゲージスプライトの取得
	Sprite* GetGaugeSprite() {
		return spriteGauge_;
	};

	//変動フラグの取得
	bool IsFluct() {
		return isFluct_;
	};

	//変動フラグのセット
	void SetIsFluct(bool isFluct) {
		isFluct_ = isFluct;
	}

	//残量の取得
	float GetRest() {
		return rest_;
	}

	//残量上限のセット
	void SetRestMax(float restMax) {
		restMax_ = restMax;
	}

	//残量のセット
	void SetRest(float rest) {
		rest_ = rest;
	}

	//最大時間の取得
	float GetMaxTime() {
		return maxTime_;
	}

	//最大時間のセット
	void SetMaxTime(float maxTime) {
		maxTime_ = maxTime;
	}

private: //メンバ変数
		 //残量
	float rest_ = 0.0f;

	//残量の上限
	float restMax_ = 0.0f;

	//ゲージのサイズ倍率
	XMFLOAT2 sizeGauge_ = {
		1.0f,
		1.0f 
	};

	//ゲージの位置(左上角)
	XMFLOAT2 positionGauge_ = {
		0,
		0 
	};

	//ゲージ左端用スプライト
	Sprite* spriteLeft_ = nullptr;
	//ゲージ右端用スプライト
	Sprite* spriteRight_ = nullptr;
	//ゲージ用スプライト
	Sprite* spriteGauge_ = nullptr;
	//残量用スプライト
	Sprite* spriteRest_ = nullptr;

	//位置イージング始発点
	XMFLOAT2 startEasePosition_{};
	//位置イージング終着点
	XMFLOAT2 endEasePosition_{};

	//サイズイージング始発点
	XMFLOAT2 startEaseSize_{};
	//サイズイージング終着点
	XMFLOAT2 endEaseSize_{};

	//イージング最大時間
	float maxTime_ = 30.0f;
	//イージング経過時間
	float nowTime_ = 0;

	//イージング実行フラグ
	bool isFluct_ = false;

private:
	//開始時と終了時早い補間
	const float EaseInOut(
		const float& start,
		const float& end,
		const float t);
};