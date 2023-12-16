#include "SceneManager.h"
#include <cassert>

/*

*	SceneManager.cpp

*	�V�[���}�l�[�W���[

*/

SceneManager* SceneManager::GetInstance()
{
	//�C���X�^���X�擾
	static SceneManager instance;
	return &instance;
}

void SceneManager::ChangeScene(const std::string& sceneName, const int stagenum)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//���̃V�[���𐶐�
	nextScene_ = sceneFactory_->CreateScene(sceneName, stagenum);
}

void SceneManager::Update()
{
	//�V�[���؂�ւ��@�\
	//���̃V�[���̗\�񂪓�������
	if (nextScene_)
	{
		//���V�[���I��
		if (scene_)
		{
			scene_->Finalize();
		}

		//�V�[���؂�ւ�
		scene_ = std::move(nextScene_);
		nextScene_ = nullptr;

		//SceneManager�̃Z�b�g
		scene_->SetSceneManager(this);
		
		//�؂�ւ����V�[����������
		scene_->Initialize();
	}
	 
	//���s���̃V�[�����X�V
	scene_->Update();
}

void SceneManager::Draw()
{
	scene_->Draw();//�V�[���`��
}

void SceneManager::Finalize()
{
	scene_->Finalize();//�I������
}
