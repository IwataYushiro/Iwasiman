#pragma once
#include "Framework.h"
#include "PostEffect.h"

class MyGame :public Framework
{
public:

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//�I��
	void Finalize() override;
	
private:
	PostEffect* pe1_ = nullptr;
	PostEffect* pe2_ = nullptr;
	PostEffect* pe3_ = nullptr;

	//�؂�ւ��p�J�E���g
	int postCount = 0;

private:
	void PostInitialize();
	void PostDraw();
	void PostDelete();
		
};

