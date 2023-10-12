#pragma once
#include <DirectXMath.h>

class Object3d;
class BaseCollider;

/*

*	QueryCallBack.h (cpp X)

*	クエリーによる情報を得るための構造体

*/
struct QueryHit
{
	//衝突相手のオブジェクト
	Object3d* object = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	DirectX::XMVECTOR inter;
	//排斥ベクトル
	DirectX::XMVECTOR reject;

};

//クエリーで交差を検出した時の動作を規定するクラス
class QueryCallback
{
public:
	//コンストラクタ
	QueryCallback() = default;
	//デストラクタ
	virtual ~QueryCallback() = default;

	//交差時のコールバック関数
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};