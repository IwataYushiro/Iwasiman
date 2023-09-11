#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include "Easing.h"
#include "Sprite.h"

#include <DirectXMath.h>
#include <list>
#include <memory>
#include "Gauge.h"

class CollisionManager;

class Earth :public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	const float MUTEKI_COUNT = 60.0f;
public:
	~Earth();
	static std::unique_ptr<Earth> Create(Model* model = nullptr);

	//初期化
	bool Initialize()override;
	void Reset();
	//更新
	void Update()override;

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();

	//UI描画
	void DrawSprite();

	//後始末
	void Finalize();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//ポジション
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isHit_ = false;
	bool isDead_ = false;
	int mutekiCount = 0;
	int life_ = 3;
	int maxLife_ = 0;

	float radius_ = 50.0f;

	//イージング
	float count = 0.0f;
	const float timer = MUTEKI_COUNT / 60.0f;
	Easing ease = Easing(1.0f, 0.0f, timer);
	//スプライト
	SpriteCommon* spCommon_ = SpriteCommon::GetInstance();
	Sprite* spriteHit_ = new Sprite();

	//HPゲージ
	Gauge* hpGauge_ = {};

	//HPゲージの位置(左上角)
	XMFLOAT2 positionHPGauge_ = {0,0};

	//HP用イージング最大時間
	float maxTimeHP_ = 30.0f;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
	//ライフ
	void SetLife(int life) { this->life_ = life; }
	const int& GetLife()const { return life_; }

	//HPゲージの取得
	Gauge* GetHPGauge() const {
		return hpGauge_;
	}
};
#pragma once
