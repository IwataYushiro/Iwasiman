#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "EnumList.h"

#include <memory>
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
	void ChangeScene(const std::string& sceneName, const int stagenum = SL_Default);
	//�X�V
	void Update();

	//�`��
	void Draw();
	//�|�X�g�G�t�F�N�g�`��
	void DrawPostEffect();
	//�I��
	void Finalize();

private://�����o�ϐ�
	//���݃V�[��
	std::unique_ptr<BaseScene> scene_ = nullptr;
	//���̃V�[��
	std::unique_ptr<BaseScene> nextScene_ = nullptr;
	//�؂�Ă���V�[���t�@�N�g���[
	const AbstractSceneFactory* sceneFactory_ = nullptr;

public://�A�N�Z�b�T�u����
	//�V�[���t�@�N�g���[�̃Z�b�^�[
	void SetSceneFactory(const AbstractSceneFactory* sceneFactory) { this->sceneFactory_ = sceneFactory; }

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
