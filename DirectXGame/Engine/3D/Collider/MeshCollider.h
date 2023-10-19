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
	MeshCollider() { shapeType_ = COLLISIONSHAPE_MESH; }
	//�f�X�g���N�^
	~MeshCollider()=default;

	//�O�p�`�̔z����\�z(���f��)
	void ConstructTriangles(Model* model);
	//�X�V
	void Update()override;
	//���Ƃ̓����蔻��(���A�Փ˓_�A�r�˃x�N�g��)
	bool CheckCollisionSphere(const Sphere& sphere,
		DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
	//���C�Ƃ̓����蔻��(���C�A�����A�Փ˓_)
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

private:
	std::vector<Triangle> triangles_;
	//���[���h�s��̋t�s��
	DirectX::XMMATRIX invMatWorld_;

};
