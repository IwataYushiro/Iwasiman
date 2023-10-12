#pragma once
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"

#include <DirectXMath.h>
#include <map>

#include "SceneManager.h"

//�O���錾
//json���x���f�[�^
struct LevelData;

/*

*	GameOverScene.h

*	�Q�[���I�[�o�[�V�[��

*/
class GameOverScene :public BaseScene
{
public://�����o�֐�
	//�R���X�g���N�^(�X�e�[�W���)
	GameOverScene(int stagenum);
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;
	//���x���f�[�^�ǂݍ���(�X�e�[�W�t�@�C���p�X)
	void LoadLVData(const std::string& stagePath);

private://�ÓI�����o�ϐ�
	//DirectX���
	static DirectXCommon* dxCommon_;
	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;
	//�C���v�b�g
	static Input* input_;
	//�I�[�f�B�I
	static Audio* audio_;
	//�V�[���}�l�[�W���[
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;
	//�J����
	static Camera* camera_;

private://�����o�ϐ�
	//�X�e�[�W�ԍ�
	int stageNum_;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();			//�Q�[���I�[�o�[�X�v���C�g
	Sprite* spriteContinue_ = new Sprite();			//�R���e�B�j���[�\���X�v���C�g
	Sprite* spriteStageSelect_ = new Sprite();		//�X�e�[�W�Z���N�g�\���X�v���C�g
	Sprite* spriteTitle_ = new Sprite();			//�^�C�g���\���X�v���C�g
	Sprite* spriteDone_ = new Sprite();				//����X�v���C�g

	//FBX
	//ModelFbx* modelF_ = nullptr;
	//ObjectFbx* objF_ = nullptr;

	//���f��
	//Model* modelPlayer_ = nullptr;
	//Object3d* object3DPlayer_ = nullptr;

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;
	//�X�e�[�W�I�u�W�F�N�g
	Object3d* objStage_ = nullptr;

	Model* modelStageTutorial_ = nullptr;	//�`���[�g���A���X�e�[�W���f��(�V��)
	Model* modelStage1_ = nullptr;			//�X�e�[�W1���f��(�V��)
	Model* modelStage2_ = nullptr;			//�X�e�[�W2���f��(�V��)

	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;
	//�I�u�W�F�N�g�z��
	std::vector<Object3d*> objects_;

	//�X�^�[�g���t���O
	bool isStart_ = false;
	//���j���[���t���O
	bool isMenu_ = false;
	
	//���j���[�\���p�̃C�[�W���O
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),			//���j���[
		Easing(1300.0f, 0.0f, 1.2f),			//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),			//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),			//�^�C�g����
		Easing(1300.0f, 0.0f, 1.8f),			//�X�y�[�X�őI��
	};
	//�I�����Ă��郁�j���[�\��
	int menuCount_ = 0;
	//�F��ς���X�s�[�h
	float speedColor_ = 0.0f;
	//�F���]�t���O
	bool isColorReverse_ = false;

	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pm1_ = nullptr;
	

};