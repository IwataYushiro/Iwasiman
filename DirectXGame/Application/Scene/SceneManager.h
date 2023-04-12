#pragma once
#include "BaseScene.h"

class Scenemanager
{
public://�����o�֐�
	//�f�X�g���N�^
	~Scenemanager();
	//�X�V
	void Update();

	//�`��
	void Draw();

	//�I��
	void Finalize();

private://�����o�ϐ�
	//���݃V�[��
	BaseScene* scene_ = nullptr;
	//���̃V�[��
	BaseScene* nextScene_ = nullptr;

public://�A�N�Z�b�T�u����
	//���̃V�[����\��
	void SetNextScene(BaseScene* nextScene) { this->nextScene_ = nextScene; }

};
