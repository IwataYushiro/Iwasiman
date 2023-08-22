#pragma once
//�O���錾
class SceneManager;

//�V�[�����
class BaseScene
{
public://�����o�֐�
	virtual~BaseScene() = default;
	
	//������
	virtual void Initialize() = 0;
	//�X�V
	virtual void Update() = 0;
	//�`��
	virtual void Draw() = 0;
	//�I������
	virtual void Finalize() = 0;

private://�����o�ϐ�
	//�V�[���}�l�[�W���[(�؂�Ă���̂ł�����delete�̓_���[�b�^�C)
	SceneManager* sceneManager_ = nullptr;

public://�A�N�Z�b�T�u����
	virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager_ = sceneManager; }
};
