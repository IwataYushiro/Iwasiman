#include "SceneManager.h"

Scenemanager::~Scenemanager()
{
	Finalize();
}

void Scenemanager::Update()
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
		
		//�؂�ւ����V�[����������
		scene_->Initialize();
	}
	 
	//���s���̃V�[�����X�V
	scene_->Update();
}

void Scenemanager::Draw()
{
	scene_->Draw();
}

void Scenemanager::Finalize()
{
	scene_->Finalize();
	delete scene_;
}
