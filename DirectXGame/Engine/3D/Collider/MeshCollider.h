#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/*

*	MeshCollider.h

*	���b�V���Փ˔���I�u�W�F�N�g

*/
class MeshCollider:public BaseCollider
{
public:
	//�R���X�g���N�^�Ƀ��b�V���`����Z�b�g
	MeshCollider() { shapeType = COLLISIONSHAPE_MESH; }
	~MeshCollider()=default;

	//�O�p�`�̔z����\�z
	void ConstructTriangles(Model* model);
	//�X�V
	void Update()override;
	//���Ƃ̓����蔻��
	bool CheckCollisionSphere(const Sphere& sphere,
		DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
	//���C�Ƃ̓����蔻��
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

private:
	std::vector<Triangle> triangles;
	//���[���h�s��̋t�s��
	DirectX::XMMATRIX invMatWorld;

};
