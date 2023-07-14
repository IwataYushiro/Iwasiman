#pragma once
#include "CollisionPrimitive.h"
#include "RayCastHit.h"

#include <forward_list>
#include <d3d12.h>
class BaseCollider;

class CollisionManager
{
public://�ÓI�����o�֐�
	static CollisionManager* GetInstance();

public://�����o�֐�
	//�R���C�_�[�̒ǉ�
	inline void AddCollider(BaseCollider* collider) { colliders.push_front(collider); }
	//�R���C�_�[�̍폜
	inline void RemoveCollider(BaseCollider* collider) { colliders.remove(collider); }
	//�S�Ă̏Փ˃`�F�b�N
	void CheckAllCollisions();
/// <summary>
/// ���C�L���X�g
/// </summary>
///	<param names="ray">���C</param>
///	<param names="hitInfo">�Փˏ��</param>
/// <param names="maxDistance">�ő勗��</param>
/// <returns>���C���C�ӂ̃R���C�_�[�ƌ����ꍇtrue,����ȊO��false</returns>
	bool RayCast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
	//�R���C�_�[�̃��X�g
	std::forward_list<BaseCollider*>colliders;
};

