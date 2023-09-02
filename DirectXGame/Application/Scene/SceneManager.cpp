#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::ChangeScene(const std::string& sceneName, int stagenum)
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
			delete scene_;
		}

		//�V�[���؂�ւ�
		scene_ = nextScene_;
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
	scene_->Draw();
}

void SceneManager::Finalize()
{
	scene_->Finalize();
	delete scene_;
}
