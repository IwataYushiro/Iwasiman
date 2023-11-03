#include "MeshCollider.h"
#include "Collision.h"
#include "XYZ.h"

using namespace DirectX;

/*

*	MeshCollider.cpp

*	メッシュ衝突判定オブジェクト

*/

void MeshCollider::ConstructTriangles(Model* model)
{
	//三角形リストをクリア
	triangles_.clear();
	//モデルの持つメッシュリストを取得
	const std::vector<Mesh*>& meshes = model->GetMeshes();
	//現在のメッシュの開始三角形番号を入れておく変数(0で初期化)
	int start = 0;
	//全メッシュについて順に処理する
	std::vector<Mesh*>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it)
	{
		Mesh* mesh = *it;
		const std::vector<Mesh::VertexPosNormalUv>& vertices = mesh->GetVertices();
		const std::vector<unsigned short>& indices = mesh->GetIndices();
		//インデックスは三角形の数ｘ3個あるので
		//そこからメッシュ内の三角形の数を逆算
		const int32_t triangleIndex = 3;
		size_t triangleNum = indices.size() / triangleIndex;
		//現在のメッシュの三角形の数だけ三角形リストにスペースを追加
		triangles_.resize(triangles_.size() + triangleNum);
		//インデックス参照用
		enum TriangleIndexNum
		{
			TIN_Index0=0,
			TIN_Index1 = 1,
			TIN_Index2 = 2,
		};
		//W座標のマジックナンバーを修正する用の定数
		const float trianglePositionW = 1.0f;
		//全三角形について順に処理する
		for (int i = 0; i < triangleNum; i++)
		{
			//今から計算する三角形の参照
			Triangle& tri = triangles_[start + i];
			int idx0 = indices[i * triangleIndex + TIN_Index0];
			int idx1 = indices[i * triangleIndex + TIN_Index1];
			int idx2 = indices[i * triangleIndex + TIN_Index2];
			//三角形の3頂点の座標を代入
			tri.p0 = { vertices[idx0].pos.x,vertices[idx0].pos.y,vertices[idx0].pos.z,trianglePositionW };
			tri.p1 = { vertices[idx1].pos.x,vertices[idx1].pos.y,vertices[idx1].pos.z,trianglePositionW };
			tri.p2 = { vertices[idx2].pos.x,vertices[idx2].pos.y,vertices[idx2].pos.z,trianglePositionW };
			//3頂点から法線を計算
			tri.ComputeNormal();
		}
	
		//次のメッシュは今までの三角形番号の次から使う
		start += (int)triangleNum;
	}


}

void MeshCollider::Update()
{
	//ワールド行列の逆行列を計算
	invMatWorld_ = XMMatrixInverse(nullptr, GetObject3d()->GetWorld());
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere,
	DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	//オブジェクトのローカル座標形での球を得る(半径はXスケールを参照)
	Sphere localSphere;
	localSphere.center = XMVector3Transform(sphere.center, invMatWorld_);
	localSphere.radius *= XMVector3Length(invMatWorld_.r[XYZW_X]).m128_f32[XYZW_X];

	//ローカル座標系で交差チェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it)
	{
		const Triangle& triangle = *it;

		//球と三角形の当たり判定
		if (Collision::ChackSphere2Triangle(localSphere, triangle, inter,reject))
		{
			if (inter)
			{
				const XMMATRIX& matWorld = GetObject3d()->GetWorld();
				//ワールド座標系での交点を得る
				*inter = XMVector3Transform(*inter, matWorld);
			}
			if (reject)
			{
				const XMMATRIX& matWorld = GetObject3d()->GetWorld();
				//ワールド座標系での排斥ベクトルに変換
				*reject = XMVector3TransformNormal(*reject, matWorld);
			}

			return true;
		}
	}
	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, DirectX::XMVECTOR* inter)
{
	//オブジェクトのローカル座標形でのレイを得る(半径はXスケールを参照)
	Ray localRay;
	localRay.start = XMVector3Transform(ray.start, invMatWorld_);
	localRay.dir = XMVector3TransformNormal(ray.dir, invMatWorld_);

	//ローカル座標系で交差チェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it)
	{
		const Triangle& triangle = *it;
		XMVECTOR tempInter;

		//レイと三角形の当たり判定
		if (Collision::ChackRay2Triangle(localRay, triangle, nullptr,&tempInter))
		{
			const XMMATRIX& matWorld = GetObject3d()->GetWorld();
			//ワールド座標系での交点を得る
			tempInter = XMVector3Transform(tempInter, matWorld);
			if (distance)
			{
				//交点とレイ始点の距離を計算
				XMVECTOR sub = tempInter - ray.start;
				*distance = XMVector3Dot(sub, ray.dir).m128_f32[XYZW_X];
			}
			if (inter)
			{
				*inter = tempInter;
			}
			return true;
		}
	}
	return false;
}
