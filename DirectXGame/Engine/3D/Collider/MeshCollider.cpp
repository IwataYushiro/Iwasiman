#include "MeshCollider.h"
#include "Collision.h"
#include "XYZ.h"

using namespace DirectX;

/*

*	MeshCollider.cpp

*	���b�V���Փ˔���I�u�W�F�N�g

*/

void MeshCollider::ConstructTriangles(Model* model)
{
	//�O�p�`���X�g���N���A
	triangles_.clear();
	//���f���̎����b�V�����X�g���擾
	const std::vector<Mesh*>& meshes = model->GetMeshes();
	//���݂̃��b�V���̊J�n�O�p�`�ԍ������Ă����ϐ�(0�ŏ�����)
	int start = 0;
	//�S���b�V���ɂ��ď��ɏ�������
	std::vector<Mesh*>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it)
	{
		Mesh* mesh = *it;
		const std::vector<Mesh::VertexPosNormalUv>& vertices = mesh->GetVertices();
		const std::vector<unsigned short>& indices = mesh->GetIndices();
		//�C���f�b�N�X�͎O�p�`�̐���3����̂�
		//�������烁�b�V�����̎O�p�`�̐����t�Z
		const int32_t triangleIndex = 3;
		size_t triangleNum = indices.size() / triangleIndex;
		//���݂̃��b�V���̎O�p�`�̐������O�p�`���X�g�ɃX�y�[�X��ǉ�
		triangles_.resize(triangles_.size() + triangleNum);
		//�C���f�b�N�X�Q�Ɨp
		enum TriangleIndexNum
		{
			TIN_Index0=0,
			TIN_Index1 = 1,
			TIN_Index2 = 2,
		};
		//W���W�̃}�W�b�N�i���o�[���C������p�̒萔
		const float trianglePositionW = 1.0f;
		//�S�O�p�`�ɂ��ď��ɏ�������
		for (int i = 0; i < triangleNum; i++)
		{
			//������v�Z����O�p�`�̎Q��
			Triangle& tri = triangles_[start + i];
			int idx0 = indices[i * triangleIndex + TIN_Index0];
			int idx1 = indices[i * triangleIndex + TIN_Index1];
			int idx2 = indices[i * triangleIndex + TIN_Index2];
			//�O�p�`��3���_�̍��W����
			tri.p0 = { vertices[idx0].pos.x,vertices[idx0].pos.y,vertices[idx0].pos.z,trianglePositionW };
			tri.p1 = { vertices[idx1].pos.x,vertices[idx1].pos.y,vertices[idx1].pos.z,trianglePositionW };
			tri.p2 = { vertices[idx2].pos.x,vertices[idx2].pos.y,vertices[idx2].pos.z,trianglePositionW };
			//3���_����@�����v�Z
			tri.ComputeNormal();
		}
	
		//���̃��b�V���͍��܂ł̎O�p�`�ԍ��̎�����g��
		start += (int)triangleNum;
	}


}

void MeshCollider::Update()
{
	//���[���h�s��̋t�s����v�Z
	invMatWorld_ = XMMatrixInverse(nullptr, GetObject3d()->GetWorld());
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere,
	DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	//�I�u�W�F�N�g�̃��[�J�����W�`�ł̋��𓾂�(���a��X�X�P�[�����Q��)
	Sphere localSphere;
	localSphere.center = XMVector3Transform(sphere.center, invMatWorld_);
	localSphere.radius *= XMVector3Length(invMatWorld_.r[XYZW_X]).m128_f32[XYZW_X];

	//���[�J�����W�n�Ō����`�F�b�N
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it)
	{
		const Triangle& triangle = *it;

		//���ƎO�p�`�̓����蔻��
		if (Collision::ChackSphere2Triangle(localSphere, triangle, inter,reject))
		{
			if (inter)
			{
				const XMMATRIX& matWorld = GetObject3d()->GetWorld();
				//���[���h���W�n�ł̌�_�𓾂�
				*inter = XMVector3Transform(*inter, matWorld);
			}
			if (reject)
			{
				const XMMATRIX& matWorld = GetObject3d()->GetWorld();
				//���[���h���W�n�ł̔r�˃x�N�g���ɕϊ�
				*reject = XMVector3TransformNormal(*reject, matWorld);
			}

			return true;
		}
	}
	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, DirectX::XMVECTOR* inter)
{
	//�I�u�W�F�N�g�̃��[�J�����W�`�ł̃��C�𓾂�(���a��X�X�P�[�����Q��)
	Ray localRay;
	localRay.start = XMVector3Transform(ray.start, invMatWorld_);
	localRay.dir = XMVector3TransformNormal(ray.dir, invMatWorld_);

	//���[�J�����W�n�Ō����`�F�b�N
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it)
	{
		const Triangle& triangle = *it;
		XMVECTOR tempInter;

		//���C�ƎO�p�`�̓����蔻��
		if (Collision::ChackRay2Triangle(localRay, triangle, nullptr,&tempInter))
		{
			const XMMATRIX& matWorld = GetObject3d()->GetWorld();
			//���[���h���W�n�ł̌�_�𓾂�
			tempInter = XMVector3Transform(tempInter, matWorld);
			if (distance)
			{
				//��_�ƃ��C�n�_�̋������v�Z
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
