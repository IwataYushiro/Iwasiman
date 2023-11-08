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
#include "EnumList.h"
#include "XYZ.h"

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
	//�X�e�[�W�Z���N�g�p�e�N�X�`���C���f�b�N�X
	enum StageSelectSceneTextureIndex
	{
		SSSTI_MenuTex = 0,
		SSSTI_MenuTutorialTex = 1,
		SSSTI_Menustage1Tex = 2,
		SSSTI_Menustage2Tex = 3,
		SSSTI_MenuDoneTex = 4,
		SSSTI_BackTitleTex = 5,
		SSSTI_FadeInOutTex = 6,
		SSSTI_LoadingTex = 7,
		SSSTI_StageInfoNowTex = 8,
	};

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
	DirectX::XMFLOAT3 rot_;

	//���j���[�����p�̗񋓑�
	enum StageSelectMenuEasingNum
	{
		SSMEN_Menu = 0,				//���j���[
		SSMEN_Tutorial = 1,			//�`���[�g���A��
		SSMEN_Stage1_Sky = 2,		//�X�e�[�W1
		SSMEN_Stage2_Tower = 3,		//�X�e�[�W2
		SSMEN_SelectSpace = 4,		//�X�y�[�X�őI��
		SSMEN_Quit = 5,				//�߂�
		SSMEN_Num = 6,				//�z��p
	};
	//���j���[�|�W�V����
	const std::array<float, SSMEN_Num> menuPosY_ = { 0.0f,150.0f,300.0f,450.0f,600.0f,50.0f };

	//�C�[�W���O��
	//���j���[��ʏo���C�[�W���O�̃v���Z�b�g
	const Easing presetEaseMenuPosX_[SSMEN_Num] =
	{
		{1300.0f, 0.0f, 1.0f},					//���j���[
		{1300.0f, 100.0f, 1.2f},				//�`���[�g���A��
		{1300.0f, 100.0f, 1.4f},				//�X�e�[�W1
		{1300.0f, 100.0f, 1.6f},				//�X�e�[�W2
		{1300.0f, 425.0f, 1.8f},				//�X�y�[�X�őI��
		{1300.0f, 900.0f, 2.0f}					//�߂�
	};
	//���j���[��ʏo���C�[�W���O
	Easing easeMenuPosX_[SSMEN_Num] =
	{
		presetEaseMenuPosX_[SSMEN_Menu],					//���j���[
		presetEaseMenuPosX_[SSMEN_Tutorial],				//�`���[�g���A��
		presetEaseMenuPosX_[SSMEN_Stage1_Sky],				//�X�e�[�W1
		presetEaseMenuPosX_[SSMEN_Stage2_Tower],			//�X�e�[�W2
		presetEaseMenuPosX_[SSMEN_SelectSpace],				//�X�y�[�X�őI��
		presetEaseMenuPosX_[SSMEN_Quit]						//�߂�
	};


	//�I�񂾃X�e�[�W��^�񒆂Ɉړ�������C�[�W���O
	const Easing presetEaseStartStagePosX_[SSSMI_Num] =
	{
		{100.0f, 350.0f, 1.5f},				//�`���[�g���A����
		{100.0f, 550.0f, 1.5f},				//�X�e�[�W1��
		{100.0f, 500.0f, 1.5f}				//�X�e�[�W2��
	};
	//�I�񂾃X�e�[�W��^�񒆂Ɉړ�������C�[�W���O
	Easing easeStartStagePosX_[SSSMI_Num] =
	{
		presetEaseStartStagePosX_[SSSMI_StageTutorial_Tutorial],		//�`���[�g���A����
		presetEaseStartStagePosX_[SSSMI_Stage1_SkyStage],				//�X�e�[�W1��
		presetEaseStartStagePosX_[SSSMI_Stage2_TowerStage]				//�X�e�[�W2��
	};

	//�I�񂾃X�e�[�W����Ɉړ�������C�[�W���O�̃v���Z�b�g
	const Easing presetEaseStartStagePosY_[SSSMI_Num] =
	{
		{menuPosY_[SSMEN_Tutorial], 0.0f, 1.5f},				//�`���[�g���A����
		{menuPosY_[SSMEN_Stage1_Sky], 0.0f, 1.5f},				//�X�e�[�W1��
		{menuPosY_[SSMEN_Stage2_Tower], 0.0f, 1.5f}				//�X�e�[�W2��
	};
	//�I�񂾃X�e�[�W����Ɉړ�������C�[�W���O
	Easing easeStartStagePosY_[SSSMI_Num] =
	{
		presetEaseStartStagePosY_[SSSMI_StageTutorial_Tutorial],		//�`���[�g���A����
		presetEaseStartStagePosY_[SSSMI_Stage1_SkyStage],				//�X�e�[�W1��
		presetEaseStartStagePosY_[SSSMI_Stage2_TowerStage]				//�X�e�[�W2��
	};

	//�J�ڎ������ɋN������X�e�[�W�Z���N�g���_�J�������[�N�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseEyeStageSelect_[XYZ_Num] =
	{
		{-22.0f, 144.0f, 1.0f},					//X
		{-1.0f, 45.0f, 1.0f},					//Y
		{-60.0f, -98.0f, 1.0f}					//Z
	};
	//�J�ڎ������ɋN������X�e�[�W�Z���N�g���_�J�������[�N�C�[�W���O
	Easing easeEyeStageSelect_[XYZ_Num] =
	{
		presetEaseEyeStageSelect_[XYZ_X],				//X
		presetEaseEyeStageSelect_[XYZ_Y],				//Y
		presetEaseEyeStageSelect_[XYZ_Z]				//Z
	};

	//�J�ڎ������ɋN������X�e�[�W�Z���N�g�����_�J�������[�N�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseTargetStageSelect_[XYZ_Num] =
	{
		{50.0f, 132.0f, 1.0f},					//X
		{-8.0f, 39.0f, 1.0f},					//Y
		{-57.0f, -52.0f, 1.0f}					//Z
	};
	//�J�ڎ������ɋN������X�e�[�W�Z���N�g�����_�J�������[�N�C�[�W���O
	Easing easeTargetStageSelect_[XYZ_Num] =
	{
		presetEaseTargetStageSelect_[XYZ_X],				//X
		presetEaseTargetStageSelect_[XYZ_Y],				//Y
		presetEaseTargetStageSelect_[XYZ_Z]					//Z
	};

	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseEyeDoneMenu_[XYZ_Num] =
	{
		{144.0f, 127.0f, 1.0f},					//X
		{45.0f, 52.0f, 1.0f},					//Y
		{-98.0f, -60.0f, 1.0f}					//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeDoneMenu_[XYZ_Num] =
	{
		presetEaseEyeDoneMenu_[XYZ_X],					//X
		presetEaseEyeDoneMenu_[XYZ_Y],					//Y
		presetEaseEyeDoneMenu_[XYZ_Z]					//Z
	};

	//�X�e�[�W���莞�̒����_�J�������[�N�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseTargetDoneMenu_[XYZ_Num] =
	{
		{132.0f, 192.0f, 1.0f},					//X
		{39.0f, 33.0f, 1.0f},					//Y
		{-52.0f, -60.0f, 1.0f}					//Z
	};
	//�X�e�[�W���莞�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetDoneMenu_[XYZ_Num] =
	{
		presetEaseTargetDoneMenu_[XYZ_X],					//X
		presetEaseTargetDoneMenu_[XYZ_Y],					//Y
		presetEaseTargetDoneMenu_[XYZ_Z]					//Z
	};

	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓����_�J�������[�N�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseEyeGameStart_[XYZ_Num] =
	{
		{127.0f, 227.0f, 2.0f},					//X
		{52.0f, 55.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓����_�J�������[�N�C�[�W���O
	Easing easeEyeGameStart_[XYZ_Num] =
	{
		presetEaseEyeGameStart_[XYZ_X],					//X
		presetEaseEyeGameStart_[XYZ_Y],					//Y
		presetEaseEyeGameStart_[XYZ_Z]					//Z
	};

	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓������_�J�������[�N�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseTargetGameStart_[XYZ_Num] =
	{
		{192.0f, 292.0f, 2.0f},					//X
		{33.0f, 33.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓������_�J�������[�N�C�[�W���O
	Easing easeTargetGameStart_[XYZ_Num] =
	{
		presetEaseTargetGameStart_[XYZ_X],					//X
		presetEaseTargetGameStart_[XYZ_Y],					//Y
		presetEaseTargetGameStart_[XYZ_Z]					//Z
	};

	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̎��@�ړ��C�[�W���O�̃v���Z�b�g
	const Easing presetEasePlayerStartMove_[XYZ_Num] =
	{
		{150.0f, 250.0f, 2.0f},					//X
		{40.0f, 40.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̎��@�ړ��C�[�W���O
	Easing easePlayerStartMove_[XYZ_Num] =
	{
		presetEasePlayerStartMove_[XYZ_X],				//X
		presetEasePlayerStartMove_[XYZ_Y],				//Y
		presetEasePlayerStartMove_[XYZ_Z]				//Z
	};

	//�^�C�g���ɖ߂�Ƃ��̎��@�ړ��C�[�W���O�̃v���Z�b�g
	const Easing presetEasePlayerQuitMove_[XYZ_Num] =
	{
		{150.0f, 0.0f, 2.0f},					//X
		{40.0f, -8.0f, 2.0f},					//Y
		{-60.0f, -60.0f, 2.0f}					//Z
	};
	//�^�C�g���ɖ߂�Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerQuitMove_[XYZ_Num] =
	{
		presetEasePlayerQuitMove_[XYZ_X],					//X
		presetEasePlayerQuitMove_[XYZ_Y],					//Y
		presetEasePlayerQuitMove_[XYZ_Z]					//Z
	};

	//�t�F�[�h�C���A�E�g�̃v���Z�b�g
	const Easing presetEaseFadeInOut_ = { 1.0f, 0.0f, 1.0f };
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;

	//�I�����Ă��郁�j���[�\��
	int menuCount_ = SSSMI_StageTutorial_Tutorial;
	//�F��ς���X�s�[�h
	DirectX::XMFLOAT3 selectColor_;
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

public:
	//�F���ς�鏈��
	void UpdateChangeColor();
};
