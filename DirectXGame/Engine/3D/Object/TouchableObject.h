#pragma once
#include "Object3d.h"
//�ڐG�\�I�u�W�F�N�g
class TouchableObject:public Object3d
{
public:
	TouchableObject() = default;
	~TouchableObject() = default;

	//�I�u�W�F�N�g�̐���
	static TouchableObject* Create(Model* model = nullptr, bool Touth = true);
	//������
	bool Initialize(Model* model, bool Touth);

};
