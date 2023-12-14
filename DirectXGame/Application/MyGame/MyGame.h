#pragma once
#include "Framework.h"
#include "PostEffect.h"
#include <list>
#include <memory>

/*

*	MyGame.h

*	�Q�[���S��

*/
class MyGame :public IwasiEngine::Framework
{
private:
	using PostEffect = IwasiEngine::PostEffect;
public:

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//�I��
	void Finalize() override;
	
private:
	//�|�X�g�G�t�F�N�g
	std::unique_ptr<PostEffect> pe_;

private:
	void PostInitialize();	//�|�X�g�G�t�F�N�g������
	void PostDraw();		//�|�X�g�G�t�F�N�g�`��
		
};

