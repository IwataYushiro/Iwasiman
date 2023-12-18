#include "Item.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "EnumList.h"
#include "MyMath.h"

using namespace DirectX;
using namespace IwasiEngine;
using namespace MyMath;

/*

*	Item.cpp

*	アイテム管理

*/

Item::~Item()
{
}

std::unique_ptr<Item> Item::Create(const Model* model, const Player* player, const unsigned short subAttribute)
{
	//インスタンス生成
	std::unique_ptr<Item> ins = std::make_unique<Item>();

	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	//自機のセット
	if (player)ins->SetPlayer(player);
	if (subAttribute)ins->collider_->SetSubAttribute(subAttribute);//どのアイテムかは引数で決める

	return ins;
}

bool Item::Initialize()
{
	//初期化
	if (!Object3d::Initialize()) return false;
	//シングルトンインスタンス
	spCommon_ = SpriteCommon::GetInstance();

	//コライダー追加
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//アイテム
	collider_->SetAttribute(COLLISION_ATTR_ITEM);

	//取得したか
	isGet_ = false;
	isGetJump_ = false;

	//パーティクル
	p_ = Particle::LoadFromParticleTexture("particle3.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(p_.get());
	//スプライト
	LoadSprite();

	return true;

}

void Item::Reset()
{
	//取得したかどうかだけ
	isGet_ = false;
	isGetJump_ = false;
}

void Item::Update()
{
	//パーティクルマネージャーにカメラをセット
	pm_->SetCamera(camera_);
	//属性によって効果を変える
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP) UpdateJumpPowerup();//ジャンプ力強化アイテムの場合
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_HEAL)//ライフ回復アイテムの場合
	{
		//取得したら
		if (isGetHeal_)
		{
			//属性を取得済みアイテムにして取れないように設定
			collider_->SetSubAttribute(SUBCOLLISION_ATTR_ITEM_GET_ONCE);
			isGetHeal_ = false;
		}
	}
	//アイテムは常時回っている
	const float rotSpeedY = 2.0f;
	rotation_.y += rotSpeedY;
	//座標を転送
	Trans();
	//更新
	camera_->Update();		//カメラ
	pm_->Update();			//パーティクルマネージャー
	Object3d::Update();		//3Dオブジェクト
	
	//ジャンプバー更新
	spriteItemJumpBar_->Update();
}

void Item::UpdateJumpPowerup()
{
	
	const XMFLOAT3 asIsColor = { 1.0f,1.0f,1.0f };//素材そのままの色
	if (isGetJump_)//取得したら
	{
		//イージングで少しずつ透明にする
		ease_.ease_out_cubic();
		spriteItemJumpBar_->SetColor({ asIsColor.x, asIsColor.y,asIsColor.z, ease_.num_X });
		//効果時間を進める
		count_++;
	}
	else
	{
		//アルファ値を戻す
		spriteItemJumpBar_->SetColor({ asIsColor.x, asIsColor.y, asIsColor.z,ease_.start });
	}
	//効果時間が一定の時間に達したら強化解除
	if (count_ >= MAX_TIME)
	{
		const float countReset = 0.0f;
		count_ = countReset;
		isGet_ = false;
		isGetJump_ = false;

	}


}

void Item::Trans()
{
	//ワールド座標
	XMMATRIX world;
	//行列更新
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//合成
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	Object3d::SetWorld(world);
}

const XMFLOAT3 Item::GetWorldPosition()const
{
	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void Item::Draw()
{
	if (!isGet_)Object3d::Draw();//ゲットしたら消える
}

void Item::DrawParticle()
{
	pm_->Draw();//パーティクル描画
}

void Item::DrawSprite()
{
	if (isGetJump_)spriteItemJumpBar_->Draw();//ジャンプアイテムをゲットしたら表示される
}

void Item::OnCollision([[maybe_unused]] const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)
{
	if (isGet_)return;//多重ヒットを防止
	//パーティクルプリセット
	const ParticleManager::Preset itemGet =
	{
		p_.get(),
		position_,
		{ 8.0f ,8.0f,0.0f },
		{ 0.5f,4.0f,0.1f },
		{ 0.0f,0.001f,0.0f },
		30,
		{3.0f, 0.0f },
		{0.3f,1.0f,0.3f,1.0f},
		{0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_PLAYERS)//自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)//自機本体が触れたら効果発動
		{
			//ゲットできたことを知らせるパーティクル
			pm_->ActiveY(itemGet.particle, itemGet.startPos, itemGet.pos, itemGet.vel,
				itemGet.acc, itemGet.num, itemGet.scale, itemGet.startColor, itemGet.endColor);
			//アイテムごとに効果は違う
			if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_JUMP)//ジャンプ力強化アイテムの場合
			{
				//イージングをスタンバイ
				ease_.Standby(false);
				//ゲット
				isGetJump_ = true;
			}
			else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ITEM_HEAL)//ライフ回復アイテムの場合
			{
				//ゲット
				isGetHeal_ = true;
			}
			//アイテムをゲットしたことを伝えるフラグ
			isGet_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;//弾が触れても何も起こらない
	}
}

void Item::LoadSprite()
{
	//アイテム関係のスプライト
	//ジャンプバー
	spCommon_->LoadTexture(GPSITI_ItemJumpBar, "itemtex/itemjumpbar.png");
	spriteItemJumpBar_->Initialize(spCommon_, GPSITI_ItemJumpBar);
	const XMFLOAT2 jumpBarPos = {800.0f,65.0f };
	spriteItemJumpBar_->SetPosition(jumpBarPos);

	spriteItemJumpBar_->Update();
}
