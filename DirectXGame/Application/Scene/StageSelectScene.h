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

*	StageSelectScene.h

*	�X�e�[�W�Z���N�g�V�[��

*/
class StageSelectScene :public BaseScene
{
public://�����o�֐�
	//�R���X�g���N�^(�X�e�[�W�ԍ���V���w�i�ƘA�������l)
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
	
	//�C���v�b�g
	static Input* input_;
	//�I�[�f�B�I
	static Audio* audio_;
	//�V�[���}�l�[�W���[
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;
	
private://�����o�ϐ�

	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;
	//�J����
	Camera* camera_ = nullptr;

	//Sprite
	Sprite* spriteMenu_ = new Sprite();				//���j���[��ʃX�v���C�g
	Sprite* spriteTutorial_ = new Sprite();			//�`���[�g���A���X�e�[�W�X�v���C�g
	Sprite* spriteStage1_ = new Sprite();			//�X�e�[�W1�X�v���C�g
	Sprite* spriteStage2_ = new Sprite();			//�X�e�[�W2�X�v���C�g
	Sprite* spriteDone_ = new Sprite();				//����\���X�v���C�g
	Sprite* spriteBack_ = new Sprite();				//�^�C�g���ɖ߂�X�v���C�g
	Sprite* spriteFadeInOut_ = new Sprite();		//�t�F�[�h�C���A�E�g�X�v���C�g
	Sprite* spriteLoad_ = new Sprite();				//���[�h�X�v���C�g
	Sprite* spriteStageInfoNow_ = new Sprite();		//���݃X�e�[�W�X�v���C�g

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;

	//�I�u�W�F�N�g
	Object3d* objStage_ = nullptr;				//�X�e�[�W�I�u�W�F�N�g
	std::vector<Object3d*> objPlayers_;			//���@�I�u�W�F�N�g�z��
	std::vector<Object3d*> objGoals_;			//�S�[���I�u�W�F�N�g�z��
	std::vector<Object3d*> objGrounds_;			//���I�u�W�F�N�g�z��

	//���f��
	Model* modelPlayer_ = nullptr;				//���@���f��
	Model* modelGoal_ = nullptr;				//�S�[�����f��
	Model* modelGround_ = nullptr;				//�����f��
	Model* modelStageTutorial_ = nullptr;		//�`���[�g���A���X�e�[�W���f��(�V��)
	Model* modelStage1_ = nullptr;				//�X�e�[�W1���f��(�V��)
	Model* modelStage2_ = nullptr;				//�X�e�[�W2���f��(�V��)
	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;		
	
	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	//�C�[�W���O��
	//���j���[��ʏo���C�[�W���O
	Easing easeMenuPosX_[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),				//���j���[
		Easing(1300.0f, 100.0f, 1.2f),				//�`���[�g���A��
		Easing(1300.0f, 100.0f, 1.4f),				//�X�e�[�W1
		Easing(1300.0f, 100.0f, 1.6f),				//�X�e�[�W2
		Easing(1300.0f, 0.0f, 1.8f),				//�X�y�[�X�őI��
		Easing(1300.0f, 900.0f, 2.0f),				//�߂�
	};
	//�I�񂾃X�e�[�W��^�񒆂Ɉړ�������C�[�W���O
	Easing easeStartStagePosX_[3] =
	{
		Easing(100.0f, 200.0f, 1.5f),				//�`���[�g���A����
		Easing(100.0f, 400.0f, 1.5f),				//�X�e�[�W1��
		Easing(100.0f, 350.0f, 1.5f),				//�X�e�[�W2��
	};
	//�I�񂾃X�e�[�W����Ɉړ�������C�[�W���O
	Easing easeStartStagePosY_[3] =
	{
		Easing(150.0f, 0.0f, 1.5f),				//�`���[�g���A����
		Easing(300.0f, 0.0f, 1.5f),				//�X�e�[�W1��
		Easing(450.0f, 0.0f, 1.5f),				//�X�e�[�W2��
	};
	//�J�ڎ������ɋN������X�e�[�W�Z���N�g���_�J�������[�N�C�[�W���O
	Easing easeEyeStageSelect_[3]
	{
		Easing(-22.0f, 144.0f, 1.0f),				//X
		Easing(-1.0f, 45.0f, 1.0f),					//Y
		Easing(-60.0f, -98.0f, 1.0f),				//Z
	};
	//�J�ڎ������ɋN������X�e�[�W�Z���N�g�����_�J�������[�N�C�[�W���O
	Easing easeTargetStageSelect_[3]
	{
		Easing(50.0f, 132.0f, 1.0f),				//X
		Easing(-8.0f, 39.0f, 1.0f),					//Y
		Easing(-57.0f, -52.0f, 1.0f),				//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeDoneMenu_[3]
	{
		Easing(144.0f, 127.0f, 1.0f),				//X
		Easing(45.0f, 52.0f, 1.0f),					//Y
		Easing(-98.0f, -60.0f, 1.0f),				//Z
	};
	//�X�e�[�W���莞�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetDoneMenu_[3]
	{
		Easing(132.0f, 192.0f, 1.0f),				//X
		Easing(39.0f, 33.0f, 1.0f),					//Y
		Easing(-52.0f, -60.0f, 1.0f),				//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓����_�J�������[�N�C�[�W���O
	Easing easeEyeGameStart_[3]
	{
		Easing(127.0f, 227.0f, 2.0f),				//X
		Easing(52.0f, 55.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓������_�J�������[�N�C�[�W���O
	Easing easeTargetGameStart_[3]
	{
		Easing(192.0f, 292.0f, 2.0f),				//X
		Easing(33.0f, 33.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̎��@�ړ��C�[�W���O
	Easing easePlayerStartMove_[3] =
	{
		Easing(150.0f, 250.0f, 2.0f),				//X
		Easing(40.0f, 40.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};
	//�^�C�g���ɖ߂�Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerQuitMove_[3] =
	{
		Easing(150.0f, 0.0f, 2.0f),					//X
		Easing(40.0f, -8.0f, 2.0f),					//Y
		Easing(-60.0f, -60.0f, 2.0f),				//Z
	};

	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//�I�����Ă��郁�j���[�\��
	int menuCount_ = 0;
	//�F��ς���X�s�[�h
	float speedColor_ = 0.0f;
	//�F���]�t���O
	bool isColorReverse_ = false;

	//�J�ڃt���O��
	bool isStageSelect_ = true;		//�X�e�[�W�Z���N�g��
	bool isStart_ = false;			//�Q�[���X�^�[�g��
	bool isDone_ = false;			//�X�e�[�W���莞
	bool outStageSelect_ = false;	//�X�e�[�W�Z���N�g���甲����
	bool isFadeOut_ = false;		//�t�F�[�h�A�E�g

	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pm1_ = nullptr;

};
