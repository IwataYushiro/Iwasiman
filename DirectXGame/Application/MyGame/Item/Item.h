#pragma once
#include "Camera.h"
#include "Easing.h"
#include "ParticleManager.h"
#include "Model.h"
#include "Object3d.h"
#include "Vector2.h"
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
class Item :public IwasiEngine::Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::を省略
	using Sprite = IwasiEngine::Sprite;
	using SpriteCommon = IwasiEngine::SpriteCommon;
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using Easing = IwasiEngine::Easing;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;
	using CollisionInfo = IwasiEngine::CollisionInfo;
	using Vector2 = IwasiEngine::Vector2;

public://定数
	//最大効果時間
	const float MAX_TIME = 210.0f;
public:
	~Item();
	
	//生成(使用モデル、プレイヤー、サブ属性)
	static std::unique_ptr<Item> Create(const Model* model = nullptr, const Player* player = nullptr
		,const unsigned short subAttribute = 0b1000000000000001);
	//初期化
	bool Initialize()override;
	//リセット
	void Reset();
	//更新
	void Update()override;
	//更新(ジャンプ強化アイテム)
	void UpdateJumpPowerup();
	//描画
	void Draw();
	//パーティクル描画
	void DrawParticle();
	//スプライト描画
	void DrawSprite();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute,const unsigned short subAttribute)override;

private:
	//取得時
	bool isGet_ = false;
	//ジャンプアイテムを取得したか
	bool isGetJump_ = false;
	//回復アイテムを取得したか
	bool isGetHeal_ = false;
	//半径
	const float radius_ = 3.0f;
	//プレイヤー
	const Player* player_ = nullptr;

	//イージング
	//アイテム取得した時の現在時間
	float count_;
	//タイマーの値まで来たら効果終了
	const float timer_ = MAX_TIME / 60.0f;
	//取得UIに使うイージング
	Easing ease_;

	//パーティクル
	std::unique_ptr<Particle> p_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pm_ = nullptr;

public: //アクセッサ、インライン関数
	//ジャンプアイテムを取得したかどうか
	bool IsGetJump() const { return isGetJump_; }

	//プレイヤーセット
	void SetPlayer(const Player* player) { player_ = player; }

private:
	//スプライト基盤クラス
	SpriteCommon* spCommon_ = nullptr;
	//ジャンプアイテム取得時のUIスプライト
	std::unique_ptr<Sprite> spriteItemJumpBar_ = std::make_unique<Sprite>();

private:
	//スプライトのロード
	void LoadSprite();
};

