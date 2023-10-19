#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/*

*	MeshCollider.h

*	メッシュ衝突判定オブジェクト

*/
class MeshCollider:public BaseCollider
{
public:
	//コンストラクタにメッシュ形状をセット
	MeshCollider() { shapeType_ = COLLISIONSHAPE_MESH; }
	//デストラクタ
	~MeshCollider()=default;

	//三角形の配列を構築(モデル)
	void ConstructTriangles(Model* model);
	//更新
	void Update()override;
	//球との当たり判定(球、衝突点、排斥ベクトル)
	bool CheckCollisionSphere(const Sphere& sphere,
		DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
	//レイとの当たり判定(レイ、距離、衝突点)
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

private:
	std::vector<Triangle> triangles_;
	//ワールド行列の逆行列
	DirectX::XMMATRIX invMatWorld_;

};
