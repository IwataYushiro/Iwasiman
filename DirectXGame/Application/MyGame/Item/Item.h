#pragma once
#include "Camera.h"
#include "Easing.h"
#include "ParticleManager.h"
#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include "Sprite.h"
#include <list>
#include <memory>

//前方宣言
//自機クラス
class Player;
//コリジョンマネージャー
class CollisionManager;

/*

*	Item.h

*	アイテム管理

*/
class Item :public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://定数
	//最大効果時間
	const float MAX_TIME = 200.0f;
public:
	~Item();
	
	//生成(使用モデル、プレイヤー、サブ属性)
	static std::unique_ptr<Item> Create(Model* model = nullptr, Player* player = nullptr
		, unsigned short subAttribute = 0b1000000000000001);
	//初期化
	bool Initialize()override;
	//リセット
	void Reset();
	//更新
	void Update()override;
	//更新(ジャンプ強化アイテム)
	void UpdateJumpPowerup();

	//転送
	void Trans();

	//ワールド座標を取得
	XMFLOAT3 GetWorldPosition();

	//描画
	void Draw();
	//パーティクル描画
	void DrawParticle();
	//スプライト描画
	void DrawSprite();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, unsigned short attribute,unsigned short subAttribute)override;

private:
	//コリジョンマネージャー
	static CollisionManager* colManager_;
	//取得時
	bool isGet_ = false;
	//ジャンプアイテムを取得したか
	bool isGetJump_ = false;
	//半径
	float radius_ = 3.0f;
	//プレイヤー
	Player* player_ = nullptr;

	//イージング
	//アイテム取得した時の現在時間
	float count_ = 0.0f;
	//タイマーの値まで来たら効果終了
	const float timer_ = MAX_TIME / 60.0f;
	//取得UIに使うイージング
	Easing ease_ = Easing(1.0f, 0.0f, timer_);

	//パーティクル
	Particle* p_ = nullptr;
	//パーティクルマネージャー
	ParticleManager* pm_ = nullptr;

public: //アクセッサ、インライン関数
	//ジャンプアイテムを取得したかどうか
	bool IsGetJump() const { return isGetJump_; }

	//プレイヤーセット
	void SetPlayer(Player* player) { player_ = player; }

private:
	//スプライト基盤クラス
	SpriteCommon* spCommon_ = SpriteCommon::GetInstance();
	//ジャンプアイテム取得時のUIスプライト
	Sprite* spriteItemJumpBar_ = new Sprite();

private:
	//スプライトのロード
	void LoadSprite();
};

