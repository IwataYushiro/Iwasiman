#pragma once
#include "Object3d.h"
#include "Model.h"
#include <DirectXMath.h>

//前方宣言
//コリジョンマネージャー
class CollisionManager;

/*

*	PlayerBullet.h

*	自機の弾

*/
class PlayerBullet:public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//生成(初期座標、速度、使用モデル)
	static std::unique_ptr<PlayerBullet> Create
	(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model = nullptr);
	//初期化(初期座標、速度)
	bool Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity);
	//リセット処理
	void Reset();

	//更新
	void Update()override;

	//描画
	void Draw();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)override;

	//ワールド座標を取得
	const XMFLOAT3 GetWorldPosition()const;

private:
	//コリジョンマネージャー
	static CollisionManager* colManager_;

	//速度
	XMFLOAT3 velocity_;
	//寿命<frm>
	static const int32_t lifeTime_ = 60;
	//死亡時間
	int32_t deathTimer_ = lifeTime_;
	//死亡フラグ
	bool isDead_ = false;
	//半径
	float radius_ = 4.0f;

public: //アクセッサ、インライン関数
	bool IsDead() const { return isDead_; }
};