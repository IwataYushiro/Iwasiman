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

//json���x���f�[�^
struct LevelData;

/*

*	GameOverScene.h

*	�Q�[���I�[�o�[�V�[��

*/
class GameOverScene :public BaseScene
{
public://�����o�֐�
	GameOverScene(int stagenum);
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;
	//���x���f�[�^�ǂݍ���
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
	int stageNum;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();
	Sprite* spriteContinue_ = new Sprite();
	Sprite* spriteStageSelect_ = new Sprite();
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteDone_ = new Sprite();

	//FBX
	//ModelFbx* modelF_ = nullptr;
	//ObjectFbx* objF_ = nullptr;

	//���f��
	//Model* modelPlayer_ = nullptr;
	//Object3d* object3DPlayer_ = nullptr;

	LevelData* levelData_ = nullptr;

	Object3d* objStage_ = nullptr;

	Model* modelStageTutorial_ = nullptr;
	Model* modelStage1_ = nullptr;
	Model* modelStage2_ = nullptr;

	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	std::map<std::string, Model*> models_;
	std::vector<Object3d*> objects_;

	bool isStart_ = false;
	bool isMenu_ = false;
	Easing easeTitlePosX_ = Easing(0.0f, -1300.0f, 1.0f);

	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//���j���[
		Easing(1300.0f, 0.0f, 1.2f),//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),//�^�C�g����
		Easing(1300.0f, 0.0f, 1.8f),//�X�y�[�X�őI��
	};
	int menuCount_ = 0;
	//�F��ς���X�s�[�h
	float speedColor_ = 0.0f;
	//�F���]�t���O
	bool isColorReverse_ = false;

	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//���f���ǂݍ���
	//void LoadModel();

};