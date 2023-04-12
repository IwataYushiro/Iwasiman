#pragma once
#include "Sprite.h"
#include "BaseScene.h"

//�^�C�g���V�[��
class TitleScene :public BaseScene
{
public://�����o�֐�
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;

private://�ÓI�����o�ϐ�
	//�X�v���C�g���
	static SpriteCommon* spCommon_;

private://�����o�ϐ�
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
};
