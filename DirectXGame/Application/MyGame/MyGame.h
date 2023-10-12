#pragma once
#include "Framework.h"
#include "PostEffect.h"
#include <list>
#include <memory>

/*

*	MyGame.h

*	�Q�[���S��

*/
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
	//���v
	const int POST_NUM = 12;
	//�|�X�g�G�t�F�N�g
	PostEffect* pe;
	
	//�؂�ւ��p�J�E���g
	int postCount = 0;
	//�|�X�g�G�t�F�N�g�̃o���G�[�V����
	int Variation = 0;

private:
	void PostInitialize();
	void PostDraw();
	void PostDelete();
		
};

