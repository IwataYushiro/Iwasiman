#pragma once
#include <DirectXMath.h>

class Object3d;
class BaseCollider;

/*

*	QueryCallBack.h (cpp X)

*	�N�G���[�ɂ����𓾂邽�߂̍\����

*/
struct QueryHit
{
	//�Փˑ���̃I�u�W�F�N�g
	Object3d* object = nullptr;
	//�Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	//�Փ˓_
	DirectX::XMVECTOR inter;
	//�r�˃x�N�g��
	DirectX::XMVECTOR reject;

};

//�N�G���[�Ō��������o�������̓�����K�肷��N���X
class QueryCallback
{
public:
	//�R���X�g���N�^
	QueryCallback() = default;
	//�f�X�g���N�^
	virtual ~QueryCallback() = default;

	//�������̃R�[���o�b�N�֐�
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};