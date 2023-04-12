#pragma once
#include "BaseScene.h"

class SceneManager
{
public://�V���O���g���C���X�^���X
	static SceneManager* GetInstance();

public://�����o�֐�
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
