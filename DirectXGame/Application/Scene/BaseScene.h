#pragma once
#include <DirectXMath.h>
//�O���錾
//�V�[���}�l�[�W���[
class SceneManager;

/*

*	BaseScene.h (cpp X)

*	�V�[�����

*/
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

protected://�p�������o�ϐ�
	//���݃X�e�[�W�̃|�W�V����
	DirectX::XMFLOAT2 stageInfoNowPos_ = { 300.0f,100.0f };
	//���[�f�B���O���̃|�W�V����
	DirectX::XMFLOAT2 loadPos_ = { 350.0f,600.0f };
	//�F�T���v��
	DirectX::XMFLOAT3 white_ = { 1.0f,1.0f,1.0f };	//��
	DirectX::XMFLOAT3 black_ = { 0.0f,0.0f,0.0f };	//��
	DirectX::XMFLOAT3 deepRed_ = { 0.2f,0.0f,0.0f };	//�Z����

private://�����o�ϐ�
	//�V�[���}�l�[�W���[(�؂�Ă���̂ł�����delete�̓_���[�b�^�C)
	SceneManager* sceneManager_ = nullptr;

public://�A�N�Z�b�T�u����
	virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager_ = sceneManager; }
};
