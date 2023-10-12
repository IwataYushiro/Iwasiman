#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/*

*	SceneManager.h

*	�V�[���}�l�[�W���[

*/
class SceneManager final
{
public://�V���O���g���C���X�^���X
	static SceneManager* GetInstance();

public://�����o�֐�
	//���̃V�[����\��
	void ChangeScene(const std::string& sceneName, int stagenum = 0);
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
	SceneManager() = default;
	~SceneManager() = default;
public:
	SceneManager(const SceneManager& obj) = delete;
	SceneManager& operator=(const SceneManager& obj) = delete;
};
