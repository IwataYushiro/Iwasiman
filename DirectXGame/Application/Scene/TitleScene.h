#pragma once
#include "Sprite.h"

//�^�C�g���V�[��
class TitleScene
{
public://�����o�֐�
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw();
	//�I��
	void Finalize();

private://�ÓI�����o�ϐ�
	//�X�v���C�g���
	static SpriteCommon* spCommon_;

private://�����o�ϐ�
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
};
