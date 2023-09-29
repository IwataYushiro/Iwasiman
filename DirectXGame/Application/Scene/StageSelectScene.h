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

//�^�C�g���V�[��
class StageSelectScene :public BaseScene
{
public://�����o�֐�
	StageSelectScene(int count);
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
	//Sprite
	Sprite* spriteMenu_ = new Sprite();
	Sprite* spriteTutorial_ = new Sprite();
	Sprite* spriteStage1_ = new Sprite();
	Sprite* spriteStage2_ = new Sprite();
	Sprite* spriteDone_ = new Sprite();
	Sprite* spriteBack_ = new Sprite();

	//FBX
	ModelFbx* modelF = nullptr;
	ObjectFbx* objF = nullptr;

	//���f��
	//Model* modelPlayer_ = nullptr;
	//Object3d* object3DPlayer_ = nullptr;

	LevelData* levelData = nullptr;

	Object3d* objStage = nullptr;
	
	Model* modelStageTutorial = nullptr;
	Model* modelStage1 = nullptr;
	Model* modelStage2 = nullptr;

	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot = { 0.0f,0.0f,0.0f };

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	bool isStart = false;
	bool isMenu = false;
	Easing easeTitlePosX = Easing(0.0f, -1300.0f, 1.0f);

	Easing easeMenuPosX[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//���j���[
		Easing(1300.0f, 0.0f, 1.2f),//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),//�X�y�[�X�őI��
		Easing(1300.0f, 0.0f, 1.8f),//�X�y�[�X�őI��
		Easing(1300.0f, 900.0f, 2.0f),//�߂�
	};
	int MenuCount = 0;
	//�F��ς���X�s�[�h
	float speedColor = 0.0f;
	//�F���]�t���O
	bool isColorReverse_ = false;
	//�X�e�[�W�Z���N�g���甲����
	bool outStageSerect = false;

	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//���f���ǂݍ���
	//void LoadModel();

};
