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

//�X�e�[�W�Z���N�g�V�[��
class StageSelectScene :public BaseScene
{
public://�����o�֐�
	StageSelectScene(int count);
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//�X�e�[�W�Z���N�g��
	void UpdateIsStageSelect();
	//���莞
	void UpdateIsDone();
	//���肩��̃Q�[���X�^�[�g
	void UpdateIsGameStart();
	//�^�C�g���֖߂�Ƃ� 
	void UpdateIsQuitTitle();

	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;
	//�t�F�[�h�A�E�g
	void FadeOut(DirectX::XMFLOAT3 rgb);
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

	//�t�F�[�h�C���A�E�g�Z�b�g
	Sprite* spriteFadeInOut_ = new Sprite();
	bool isFadeOut_ = false;
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	LevelData* levelData_ = nullptr;

	//�I�u�W�F�N�g
	Object3d* objStage_ = nullptr;
	std::vector<Object3d*> objPlayers_;
	std::vector<Object3d*> objGoals_;
	std::vector<Object3d*> objGrounds_;

	//���f��
	Model* modelPlayer_ = nullptr;
	Model* modelGoal_ = nullptr;
	Model* modelGround_ = nullptr;
	Model* modelStageTutorial_ = nullptr;
	Model* modelStage1_ = nullptr;
	Model* modelStage2_ = nullptr;
	std::map<std::string, Model*> models_;

	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	
	std::vector<Object3d*> objects_;

	Easing easeMenuPosX_[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//���j���[
		Easing(1300.0f, 0.0f, 1.2f),//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),//�X�y�[�X�őI��
		Easing(1300.0f, 0.0f, 1.8f),//�X�y�[�X�őI��
		Easing(1300.0f, 900.0f, 2.0f),//�߂�
	};


	Easing easeEyeStageSelect_[3]
	{
		Easing(-22.0f, 144.0f, 1.0f),//X
		Easing(-1.0f, 45.0f, 1.0f),//Y
		Easing(-60.0f, -98.0f, 1.0f),//Z
	};
	Easing easeTargetStageSelect_[3]
	{
		Easing(50.0f, 132.0f, 1.0f),//X
		Easing(-8.0f, 39.0f, 1.0f),//Y
		Easing(-57.0f, -52.0f, 1.0f),//Z
	};

	Easing easeEyeDoneMenu_[3]
	{
		Easing(144.0f, 127.0f, 1.0f),//X
		Easing(45.0f, 52.0f, 1.0f),//Y
		Easing(-98.0f, -60.0f, 1.0f),//Z
	};
	Easing easeTargetDoneMenu_[3]
	{
		Easing(132.0f, 192.0f, 1.0f),//X
		Easing(39.0f, 33.0f, 1.0f),//Y
		Easing(-52.0f, -60.0f, 1.0f),//Z
	};

	Easing easeEyeGameStart_[3]
	{
		Easing(127.0f, 227.0f, 2.0f),//X
		Easing(52.0f, 55.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};
	Easing easeTargetGameStart_[3]
	{
		Easing(192.0f, 292.0f, 2.0f),//X
		Easing(33.0f, 33.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};

	Easing easePlayerStartMove_[3] =
	{
		Easing(150.0f, 250.0f, 2.0f),//X
		Easing(40.0f, 40.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};

	Easing easePlayerQuitMove_[3] =
	{
		Easing(150.0f, 0.0f, 2.0f),//X
		Easing(40.0f, -8.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
	};

	int menuCount_ = 0;
	//�F��ς���X�s�[�h
	float speedColor_ = 0.0f;
	//�F���]�t���O
	bool isColorReverse_ = false;

	//�J�ڃt���O��
	//�X�e�[�W�Z���N�g��
	bool isStageSelect_ = true;
	//�Q�[���X�^�[�g��
	bool isStart_ = false;
	//�X�e�[�W���莞
	bool isDone_ = false;
	//�X�e�[�W�Z���N�g���甲����
	bool outStageSelect_ = false;

	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//���f���ǂݍ���
	//void LoadModel();

};
