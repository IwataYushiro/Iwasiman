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

//前方宣言
//コリジョンマネージャー
class CollisionManager;
//ゲームプレイシーン
class GamePlayScene;

/*

*	Player.h

*	自機

*/
class Player :public Object3d
{
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//最大無敵時間
	const int MUTEKI_COUNT = 60;
	//ジャンプ強化最大効果時間
	const float JUMPITEM_MAX_TIME = 200.0f;

	//引数に渡す構造体
	struct PlayerModelList
	{
		Model* playerModel = nullptr;
		Model* playerBullet = nullptr;
		Model* playerHit = nullptr;
		Model* playerMove = nullptr;
	};

public:
	//デストラクタ
	~Player();
	//生成(使用モデル、ゲームプレイシーン)
	static std::unique_ptr<Player> Create(const PlayerModelList* model = nullptr,GamePlayScene* gamescene = nullptr);
	//初期化
	bool Initialize() override;
	//リセット処理
	void Reset();

	//更新(手前と背面ジャンプの有効化、攻撃処理の有効化、開始時演出だと全操作無効)
	void Update(const bool isBack = true, const bool isAttack = true, const bool isStart = false);
	//プレイヤーの移動処理
	void Move();

	//落ちる処理とジャンプ処理
	void FallAndJump();
	//奥へ移動
	void JumpBack();
	//着地(属性指定)
	void Landing(const unsigned short attribute);
	//プレイヤーの攻撃処理
	void Attack();

	//転送
	void Trans();

	//ワールド座標を取得
	const XMFLOAT3 GetWorldPosition()const ;

	//描画
	void Draw();
	//スプライト描画
	void DrawSprite();
	//パーティクル描画
	void DrawParticle();

	//衝突を検出したら呼び出されるコールバック関数(コリジョン情報、メイン属性、サブ属性)
	void OnCollision(const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)override;

	//ベジェ曲線(最初点、中間点1、中間点2、最終点、時間の進み具合)
	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	//コリジョンマネージャー
	static CollisionManager* colManager_;
	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	//右を向いてるか
	bool isRight_ = true;
	//振り向く用のイージングのプリセット
	const Easing presetEaseRotateRightY_ = {-90.0f,90.0f,0.2f};//Y軸限定　左、右
	//振り向く用のイージング
	Easing easeRotateRightY_ = presetEaseRotateRightY_;
	//最初は方向を変えない
	bool startGameRotate_ = false;	//呼び出されるのは最初だけ
	//スプライト
	SpriteCommon* spCommon_ = nullptr;//基盤

	std::unique_ptr<Sprite> spriteLifeBar_ = std::make_unique<Sprite>();			//ライフバー
	const XMFLOAT2 lifeBarPos_ = { 800.0f,25.0f };	//ライフバーの座標
	const XMFLOAT4 green_ = { 0.2f,1.0f,0.2f,1.0f };//緑
	const XMFLOAT4 red_ = { 1.0f,0.2f,0.2f,1.0f };	//赤

	Easing easelifeBarSize_ = { 0.0f,32.0f,3.0f };			//ライフを貯めるイージング
	XMFLOAT2 lifeBarDamageSize_ = { 0.0f,32.0f };			//ダメージを負うとこの値分サイズが縮小

	std::unique_ptr<Sprite> spriteHit_ = std::make_unique<Sprite>();				//ヒットエフェクト

	std::unique_ptr<Sprite> spriteExplosion_ = std::make_unique<Sprite>();		//爆発エフェクト
	const XMFLOAT2 explosionPos_ = { WinApp::WINDOW_WIDTH / 2.0f,WinApp::WINDOW_HEIGHT / 2.0f };	//ライフバーの座標
	//爆発は中央アンカーポイント
	const XMFLOAT2 explosionAnchorPoint_ = { 0.5f,0.5f };

	int32_t mutekiCount_;							//無敵時間
	const float hitTimer_ = MUTEKI_COUNT / 60.0f;	//イージング効果時間
	Easing easeHit_ = Easing(1.0f, 0.0f, hitTimer_);//イージング
	const XMFLOAT4 hitColor_ = { 0.5f,0.0f,0.0f,0.0f };	//赤く光る

	//モデル
	const Model* modelPlayer_ = nullptr;
	const Model* modelBullet_ = nullptr;
	const Model* modelHit_ = nullptr;
	const Model* modelMove_ = nullptr;
	//インプット
	Input* input_ = nullptr;

	//アングル
	XMFLOAT3 angle_;

	//ジャンプしてるか
	bool onGround_ = true;
	//ジャンプ力
	float jumpVYFist_;
	//落下時ベクトル
	XMFLOAT3 fallVec_;

	//手前、奥側に移動
	bool isJumpBack_;
	//奥側にいる場合
	bool isBack_;
	//手前、奥側ジャンプ時のポジション
	XMFLOAT3 jumpBackPos_;

	//時間計測
	std::chrono::steady_clock::time_point startCount_;	//開始時間
	std::chrono::steady_clock::time_point nowCount_;	//現在時間
	std::chrono::microseconds elapsedCount_;			//経過時間 経過時間=現在時間-開始時間
	const float	maxTime_ = 1.0f;						//全体時間
	float	timeRate_;									//どれくらい時間が進んだか
	//制御点
	XMFLOAT3 start_;									//最初点
	XMFLOAT3 point1_;									//中間点1
	XMFLOAT3 point2_;									//中間点2
	XMFLOAT3 end_;										//最終点

	//半径
	const float radius_ = 1.0f;

	//死亡演出移行フラグ
	bool isBreak_ = false;

	//生きてる
	bool isAlive_ = true;
	//死亡フラグ
	bool isDead_ = false;

	//ライフ
	int life_;
	//敵の攻撃やトゲ等に当たったか
	bool isHit_ = false;

	//パーティクル
	std::unique_ptr<Particle> particleFire_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

	//ゲームシーン
	GamePlayScene* gameScene_;
	//シェイク機能
	bool isShake_ = false;
	//現在視点
	XMFLOAT3 nowEye_;
	//現在注視点
	XMFLOAT3 nowTarget_;
	//シェイク時にスクロールしても元のカメラ視点、注視点に戻るための座標
	XMFLOAT3 hitMove_;
	//シェイク終了時にhitMoveをリセットする用の定数
	const XMFLOAT3 resetHitMove_ = { 0.0f,0.0f,0.0f };

	//死んだときのカメラ視点イージング
	Easing easeDeadCameraEye_[XYZ_Num];		//X,Y,Z
	//死んだときのカメラ注視点イージング
	Easing easeDeadCameraTarget_[XYZ_Num];	//X,Y,Z
	//カメラ切り替え
	bool isCameraEnd_ = false;							//カメラ
	bool isExplosion_ = false;							//爆発してるか
	const float cameraEyeChangeGameover_ = 150.0f;		//視点がある位置についたらゲームオーバー

	//爆発のサイズとアルファイージングのプリセット
	const Easing presetEaseExplosionSize_[XY_Num] = { {0.0f,1500.0f,1.0f},{0.0f,1500.0f,1.0f} };//サイズ(始点、終点、かかる時間)
	const Easing presetEaseExplosionAlpha_ = { 1.0f,0.0f,2.0f };//アルファ値(始点、終点、かかる時間)
	//爆発のサイズとアルファイージング
	Easing easeExplosionSizeAndAlpha_[XYW_Num] = {
		presetEaseExplosionSize_[XY_X],		//X
		presetEaseExplosionSize_[XY_Y],		//Y
		presetEaseExplosionAlpha_			//アルファ
	};

	//ステージクリア時のプレイヤーのスケールを変更のプリセット
	const Easing presetEaseChangeScaleStageClear_[XYZ_Num] =
	{
		{1.0f,0.0f,1.5f},			//X
		{1.0f,2.0f,1.5f},			//Y
		{1.0f,0.0f,1.5f}			//Z
	};
	//ステージクリア時のプレイヤーのスケールを変更
	Easing easeChangeScaleStageClear_[XYZ_Num] = {
		presetEaseChangeScaleStageClear_[XYZ_X],	//X
		presetEaseChangeScaleStageClear_[XYZ_Y],	//Y
		presetEaseChangeScaleStageClear_[XYZ_Z]		//Z
	};

	//ゴールしたか
	bool isGoal_ = false;
	//座標の停止
	XMFLOAT3 stopPos_;

	//イージング用のオフセット
	XMFLOAT3 easeOffset_ = {};

	//素材そのままの色
	const XMFLOAT3 asIsColor_ = { 1.0f,1.0f,1.0f };
	
	//アイテム関係
	//回復アイテムゲット
	bool isGetHealItem_ = false;
	//ジャンプアイテムゲット
	bool isGetJumpItem_ = false;
	//ジャンプアイテム効果時間
	float jumpPowerUpcount_;
	

public: //アクセッサ、インライン関数
	//死んだかどうか
	bool IsDead() const { return isDead_; }
	//死亡演出移行時
	bool IsBreak()const { return isBreak_; }
	//立ってるかどうか
	bool OnGround()const { return onGround_; }
	//立ち判定のセット
	void SetOnGround(const bool og) { this->onGround_ = og; }
	//ライフセット
	void SetLife(const int life) { this->life_ = life; }
	//ライフゲット
	const int& GetLife()const { return life_; }
	//ゲームシーンセット
	void SetGameScene(GamePlayScene* gameScene) { gameScene_ = gameScene; }


private://カプセル化メンバ関数
	//生存時
	void UpdateAlive(const bool isBack = true, const bool isAttack = true);
	//破壊時
	void UpdateBreak();
	//ゴール時
	void UpdateGoal();
};
