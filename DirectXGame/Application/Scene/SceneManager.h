#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "EnumList.h"

/*

*	SceneManager.h

*	�V�[���}�l�[�W���[

*/
class SceneManager final
{
public://�V���O���g���C���X�^���X
	static SceneManager* GetInstance();

public://�����o�֐�
	//���̃V�[����\��(�V�[�����A�X�e�[�W�ԍ�)
	void ChangeScene(const std::string& sceneName, int stagenum = SL_Default);
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

private:
	//�R���X�g���N�^�i�V���O���g���p�^�[���j
	SceneManager() = default;
	//�f�X�g���N�^�i�V���O���g���p�^�[���j
	~SceneManager() = default;
public:
	//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
	SceneManager(const SceneManager& obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	SceneManager& operator=(const SceneManager& obj) = delete;
};
