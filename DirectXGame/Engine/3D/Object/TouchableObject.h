#pragma once
#include "Object3d.h"
class TouchableObject:public Object3d
{
public:
	TouchableObject() = default;
	~TouchableObject() = default;

	//�I�u�W�F�N�g�̐���
	static TouchableObject* Create(Model* model = nullptr);
	//������
	bool Initialize(Model* model);

};
