#include "FallAndRiseGround.h"
#include "Player.h"
#include "MeshCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	FallAndRiseGround.cpp

*	落ちる球と昇る球

*/


std::unique_ptr<FallAndRiseGround> FallAndRiseGround::Create(Model* model, const Player* player, const unsigned short subAttribute)
{
	//インスタンス生成
	std::unique_ptr<FallAndRiseGround> ins = std::make_unique<FallAndRiseGround>();

	if (ins == nullptr) return nullptr;

	//初期化
	if (!ins->Initialize(model))
	{
		ins.release();
		assert(0);
	}
	//モデルのセット
	if (model) ins->SetModel(model);
	//自機をセット
	if (player)ins->SetPlayer(player);
	if(subAttribute)ins->collider_->SetSubAttribute(subAttribute);//どのパターンかは引数で決める

	return ins;
}

bool FallAndRiseGround::Initialize(Model* model)
{
	//初期化
	if (!Object3d::Initialize()) return false;

	//コライダー追加
	SetCollider(new MeshCollider());
	//メッシュコライダーで接触可能な三角形の配列を構築
	dynamic_cast<MeshCollider*>(collider_)->ConstructTriangles(model);
	//地形のひとつ
	collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	return true;

}

void FallAndRiseGround::Update()
{
	
	//更新時初期化
	if (!isSetStartPositionOnce_)
	{
		//最初の座標をセットする
		startPos_ = position_;
		isSetStartPositionOnce_ = true;
	}

	//属性によって効果を変える
	//もし自機が死んだら動かさないようにする
	if (!player_->IsBreak())
	{
		if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE)UpdateFallSphereReturn();				//落ちる床の場合
		else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN)UpdateFallSphereReturn();	//落ちて戻る床の場合
		else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_UPSPHERE)UpdateRiseSphere();					//昇る床の場合
		else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_UPSPHERE_RETURN)UpdateRiseSphereReturn();	//昇って戻る球の場合
	}
	//座標転送
	Trans();
	//更新
	camera_->Update();	//カメラ
	Object3d::Update();	//3Dオブジェクト

	
}

void FallAndRiseGround::UpdateFallSphere()
{
	if (isRide_)//乗ってる場合
	{
		//落ち続ける
		const float downSpeed = 0.1f;
		position_.y -= downSpeed;

		//Y座標が一定の位置まで下がったら最初の位置に戻す
		if (position_.y <= startPos_.y - resetPosY_)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallAndRiseGround::UpdateFallSphereReturn()
{
	const float speed = 0.1f;//速度

	if (isRide_)//乗ってる場合
	{
		//落ち続ける
		if (player_->OnGround())position_.y -= speed;
		else//地面から離れた場合戻るように
		{
			isReturn_ = true;
			isRide_ = false;
		}
	}
	else if (isReturn_)//離れた場合
	{
		//元の位置まで戻るようにする
		position_.y += speed;
		if (position_.y >= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
	
}

void FallAndRiseGround::UpdateRiseSphere()
{
	if (isRide_)//乗ってる場合
	{
		//昇り続ける
		const float Speed = 0.1f;
		position_.y += Speed;

		///Y座標が一定の位置まで昇ったら最初の位置に戻す
		if (position_.y >= startPos_.y + resetPosY_)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallAndRiseGround::UpdateRiseSphereReturn()
{
	const float speed = 0.1f;//速度
	if (isRide_)//乗ってる場合
	{
		//昇り続ける
		position_.y += speed;
		if (!player_->OnGround())//地面から離れた場合戻るように
		{
			isReturn_ = true;
			isRide_ = false;
		}
	}
	else if (isReturn_)//離れた場合
	{
		//元の位置まで戻るようにする
		if (!player_->OnGround())position_.y -= speed;
		if (position_.y <= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
}

void FallAndRiseGround::Draw()
{
	Object3d::Draw();//描画
}

void FallAndRiseGround::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (isRide_)return;//多重ヒットの防止

	if (attribute == COLLISION_ATTR_PLAYERS)//自機の場合
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)//自機本体の場合
		{
			//足場に乗った扱いにする
			isRide_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;//弾が当たっても特に何も起こらない

	}
}