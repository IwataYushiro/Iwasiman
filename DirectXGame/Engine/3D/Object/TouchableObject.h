#pragma once
#include "Object3d.h"

/*

*	TouchableObject.h

*	�ڐG�\�I�u�W�F�N�g

*/
class TouchableObject:public Object3d
{
public:
	//�R���X�g���N�^
	TouchableObject() = default;
	//�f�X�g���N�^
	~TouchableObject() = default;

	//�I�u�W�F�N�g�̐���(�g�p���f���A�ڐG�\��)
	static TouchableObject* Create(Model* model = nullptr, bool Touth = true);
	//������(�g�p���f���A�ڐG�\��)
	bool Initialize(Model* model, bool Touth);

};
