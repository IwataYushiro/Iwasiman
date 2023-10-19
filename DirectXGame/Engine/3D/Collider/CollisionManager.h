#pragma once
#include "CollisionPrimitive.h"
#include "RayCastHit.h"
#include "QueryCallback.h"

#include <forward_list>
#include <d3d12.h>
class BaseCollider;

/*

*	CollisionManager.h

*	�����蔻��}�l�[�W���[

*/
class CollisionManager
{
public://�ÓI�����o�֐�
	//�C���X�^���X�擾
	static CollisionManager* GetInstance();

public://�����o�֐�
	//�R���C�_�[�̒ǉ�
	inline void AddCollider(BaseCollider* collider) { colliders_.push_front(collider); }
	//�R���C�_�[�̍폜
	inline void RemoveCollider(BaseCollider* collider) { colliders_.remove(collider); }
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

	/// <summary>
	/// ���C�L���X�g(�����w���)
	/// </summary>
	///	<param names="ray">���C</param>
	///	<param names="attribute">�Փˑ���</param>
	///	<param names="hitInfo">�Փˏ��</param>
	/// <param names="maxDistance">�ő勗��</param>
	/// <returns>���C���C�ӂ̃R���C�_�[�ƌ����ꍇtrue,����ȊO��false</returns>
	bool RayCast(const Ray& ray, unsigned short attribute, RaycastHit* hitInfo = nullptr,
		float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// ���ɂ��ՓˑS�v�f
	/// </summary>
	///	<param names="sphere">��</param>
	///	<param names="callback">�Փˎ��R�[���o�b�N</param>
	/// <param names="attribute">�Ώۂ̏Փˑ���</param>
	void QuerySphere(const Sphere& sphere, QueryCallback* callback,
		unsigned short attribute = 0xffff);

private:
	//�R���C�_�[�̃��X�g
	std::forward_list<BaseCollider*>colliders_;

private:
	//�R���X�g���N�^�i�V���O���g���p�^�[���j
	CollisionManager() = default;
	//�f�X�g���N�^�i�V���O���g���p�^�[���j
	CollisionManager(const CollisionManager&) = delete;

	//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
	~CollisionManager() = default;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	CollisionManager& operator=(const CollisionManager&) = delete;

};

