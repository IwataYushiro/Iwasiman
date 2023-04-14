#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

class SceneManager
{
public://�V���O���g���C���X�^���X
	static SceneManager* GetInstance();

public://�����o�֐�
	//���̃V�[����\��
	void ChangeScene(const std::string& sceneName);
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
	//�؂�Ă���V�[���t�@�N�g���[
	AbstractSceneFactory* sceneFactory_ = nullptr;

public://�A�N�Z�b�T�u����
	//�V�[���t�@�N�g���[�̃Z�b�^�[
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory_ = sceneFactory; }

};
