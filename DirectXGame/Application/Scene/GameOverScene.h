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
	//�Q�[���I�[�o�[��
	void UpdateIsGameOver();
	//�R���e�B�j���[��
	void UpdateIsContinue();
	//�X�e�[�W�Z���N�g�J�ڎ�
	void UpdateIsQuitStageSelect();
	//�^�C�g���J�ڎ�
	void UpdateIsQuitTitle();
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;
	//���x���f�[�^�ǂݍ���(�X�e�[�W�t�@�C���p�X)
	void LoadLVData(const std::string& stagePath);
	//�t�F�[�h�A�E�g
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//�F���ς�鏈��
	void UpdateChangeColor();
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

	//�X�e�[�W�ԍ�
	int stageNum_;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();			//�Q�[���I�[�o�[�X�v���C�g
	Sprite* spriteContinue_ = new Sprite();			//�R���e�B�j���[�\���X�v���C�g
	Sprite* spriteStageSelect_ = new Sprite();		//�X�e�[�W�Z���N�g�\���X�v���C�g
	Sprite* spriteTitle_ = new Sprite();			//�^�C�g���\���X�v���C�g
	Sprite* spriteDone_ = new Sprite();				//����X�v���C�g
	Sprite* spriteFadeInOut_ = new Sprite();		//�t�F�[�h�C���A�E�g�X�v���C�g
	Sprite* spriteLoad_ = new Sprite();				//���[�h�X�v���C�g
	Sprite* spriteStageInfoNow_ = new Sprite();		//���݃X�e�[�W�X�v���C�g

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;
	
	//���f��
	Model* modelPlayer_ = nullptr;				//���@���f��
	Model* modelGoal_ = nullptr;				//�S�[�����f��
	Model* modelStageTutorial_ = nullptr;		//�`���[�g���A���X�e�[�W���f��(�V��)
	Model* modelStage1_ = nullptr;				//�X�e�[�W1���f��(�V��)
	Model* modelStage2_ = nullptr;				//�X�e�[�W2���f��(�V��)
	
	std::vector<Object3d*> objPlayers_;			//���@�I�u�W�F�N�g�z��
	std::vector<Object3d*> objGoals_;			//�S�[���I�u�W�F�N�g�z��
	std::vector<Object3d*> objStages_;			//�X�e�[�W�I�u�W�F�N�g�z��

	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot_ = { 0.0f,0.0f,0.0f };

	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;
	//�I�u�W�F�N�g�z��
	std::vector<Object3d*> objects_;

	//�t���O��
	bool isGameover_ = true;					//���j���[���t���O
	bool isContinue_ = false;					//�R���e�B�j���[����ꍇ
	bool isQuitStageSelect_ = false;			//�X�e�[�W�Z���N�g�ɖ߂�ꍇ
	bool completeRotate_ = false;				//���I������� 
	bool isQuitTitle_ = false;					//�^�C�g���ɖ߂�ꍇ
	bool isFadeOut_ = false;					//�t�F�[�h�C���A�E�g
	
	//���j���[�\���p�̃C�[�W���O
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),			//���j���[
		Easing(1300.0f, 0.0f, 1.2f),			//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),			//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),			//�^�C�g����
		Easing(1300.0f, 0.0f, 1.8f),			//�X�y�[�X�őI��
	};
	//�R���e�B�j���[����Ƃ��̎��_�C�[�W���O
	Easing easeEyeContinue_[3]
	{
		Easing(-6.0f, -21.0f, 1.0f),				//X
		Easing(-8.0f, -5.0f, 1.0f),					//Y
		Easing(-110.0f, -60.0f, 1.0f),				//Z
	};
	//�R���e�B�j���[����Ƃ��̒����_�C�[�W���O
	Easing easeTargetContinue_[3]
	{
		Easing(-32.0f, 90.0f, 1.0f),				//X
		Easing(-24.0f, -22.0f, 1.0f),				//Y
		Easing(-10.0f, -61.0f, 1.0f),				//Z
	};
	//�R���e�B�j���[����Ƃ��̎��@��]�C�[�W���O
	Easing easePlayerRotateContinue_[3]
	{
		Easing(90.0f, 0.0f, 1.0f),					//X
		Easing(-90.0f, 90.0f, 1.0f),				//Y
		Easing(0.0f, 0.0f, 1.0f),					//Z
	};
	//�R���e�B�j���[����Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveContinue_[3]
	{
		Easing(0.0f, 90.0f, 1.0f),					//X
		Easing(-8.0f, -8.0f, 1.0f),					//Y
		Easing(-60.0f, -60.0f, 1.0f),				//Z
	};

	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��_�C�[�W���O
	Easing easeEyeQuitStageSelect_[3]
	{
		Easing(-6.0f, -21.0f, 1.0f),				//X
		Easing(-8.0f, -5.0f, 1.0f),					//Y
		Easing(-110.0f, -60.0f, 1.0f),				//Z
	};
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̒����_�C�[�W���O
	Easing easeTargetQuitStageSelect_[3]
	{
		Easing(-32.0f, 90.0f, 1.0f),				//X
		Easing(-24.0f, -22.0f, 1.0f),				//Y
		Easing(-10.0f, -61.0f, 1.0f),				//Z
	};
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@��]�C�[�W���O
	Easing easePlayerRotateQuitStageSelect_[3]
	{
		Easing(90.0f, 0.0f, 1.0f),					//X
		Easing(-90.0f, 90.0f, 1.0f),				//Y
		Easing(0.0f, 0.0f, 1.0f),					//Z
	};
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveQuitStageSelect_[3]
	{
		Easing(0.0f, 150.0f, 1.0f),					//X
		Easing(-8.0f, 20.0f, 1.0f),					//Y
		Easing(-60.0f, -60.0f, 1.0f),				//Z
	};

	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb
	//�I�����Ă��郁�j���[�\��
	int menuCount_ = 0;
	//�F���]�t���O
	bool isColorReverse_ = false;

	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pm1_ = nullptr;
	

};