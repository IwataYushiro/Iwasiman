#pragma once
#include "Framework.h"
#include "GamePlayScene.h"
#include "TitleScene.h"

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
};

