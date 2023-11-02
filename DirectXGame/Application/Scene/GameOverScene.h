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
#include "XYZ.h"
#include "EnumList.h"

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

	//�Q�[���I�[�o�[�p�e�N�X�`���C���f�b�N�X
	enum GameOverSceneTextureIndex
	{
		GOSTI_MenuTex = 0,
		GOSTI_ContinueTex = 1,
		GOSTI_StageSelectTex = 2,
		GOSTI_TitleTex = 3,
		GOSTI_MenuDoneTex = 4,
		GOSTI_FadeInOutTex = 5,
		GOSTI_LoadingTex = 6,
		GOSTI_StageInfoNowTex = 7,
	};

	//�Q�[���I�[�o�[�p���j���[�C���f�b�N�X
	enum GameOverSceneMenuIndex
	{
		GOSMI_Continue = 0,
		GOSMI_StageSelect = 1,
		GOSMI_Title = 2,
	};
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
	DirectX::XMFLOAT3 rot_;

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

	//���j���[�����p�̗񋓑�
	enum GameOverMenuEasingNum
	{
		GOMEN_Menu = 0,				//���j���[
		GOMEN_Continue = 1,			//�R���e�B�j���[��
		GOMEN_StageSelect = 2,		//�X�e�[�W�Z���N�g��
		GOMEN_Title = 3,			//�^�C�g����
		GOMEN_SelectSpace = 4,		//�X�y�[�X�őI��
		GOMEN_Num = 5,				//�z��p
	};
	//���j���[�|�W�V����
	const std::array<float,GOMEN_Num> menuPosY_= { 0.0f,150.0f,300.0f,450.0f,550.0f };

	//���j���[�\���p�̃C�[�W���O�̃v���Z�b�g
	const Easing presetEaseMenuPosX_[GOMEN_Num]
	{
		{1300.0f, 0.0f, 1.0f},			//���j���[
		{1300.0f, 0.0f, 1.2f},			//�R���e�B�j���[��
		{1300.0f, 0.0f, 1.4f},			//�X�e�[�W�Z���N�g��
		{1300.0f, 0.0f, 1.6f},			//�^�C�g����
		{1300.0f, 0.0f, 1.8f}				//�X�y�[�X�őI��
	};
	//���j���[�\���p�̃C�[�W���O
	Easing easeMenuPosX_[GOMEN_Num]
	{
		presetEaseMenuPosX_[GOMEN_Menu],				//���j���[
		presetEaseMenuPosX_[GOMEN_Continue],			//�R���e�B�j���[��
		presetEaseMenuPosX_[GOMEN_StageSelect],			//�X�e�[�W�Z���N�g��
		presetEaseMenuPosX_[GOMEN_Title],				//�^�C�g����
		presetEaseMenuPosX_[GOMEN_SelectSpace],			//�X�y�[�X�őI��
	};
	
	//�R���e�B�j���[������^�񒆂Ɉړ�������C�[�W���O�̃v���Z�b�g
	const Easing presetEaseContinuePosX_ = Easing(0.0f, 380.0f, 1.0f);
	//�R���e�B�j���[������^�񒆂Ɉړ�������C�[�W���O
	Easing easeContinuePosX_ = presetEaseContinuePosX_;

	//�R���e�B�j���[��������Ɉړ�������C�[�W���O�̃v���Z�b�g
	const Easing presetEaseContinuePosY_ = Easing(150.0f, 0.0f, 1.0f);
	//�R���e�B�j���[��������Ɉړ�������C�[�W���O
	Easing easeContinuePosY_ = presetEaseContinuePosY_;

	//�R���e�B�j���[����Ƃ��̎��_�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseEyeContinue_[XYZ_Num]
	{
		{-6.0f, -21.0f, 1.0f},				//X
		{-8.0f, -5.0f, 1.0f},				//Y
		{-110.0f, -60.0f, 1.0f}				//Z
	};
	//�R���e�B�j���[����Ƃ��̎��_�C�[�W���O
	Easing easeEyeContinue_[XYZ_Num]
	{
		presetEaseEyeContinue_[XYZ_X],				//X
		presetEaseEyeContinue_[XYZ_Y],				//Y
		presetEaseEyeContinue_[XYZ_Z]				//Z
	};

	//�R���e�B�j���[����Ƃ��̒����_�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseTargetContinue_[XYZ_Num]
	{
		{-32.0f, 90.0f, 1.0f},				//X
		{-24.0f, -22.0f, 1.0f},				//Y
		{-10.0f, -61.0f, 1.0f}				//Z
	};

	//�R���e�B�j���[����Ƃ��̒����_�C�[�W���O
	Easing easeTargetContinue_[XYZ_Num]
	{
		presetEaseTargetContinue_[XYZ_X],				//X
		presetEaseTargetContinue_[XYZ_Y],				//Y
		presetEaseTargetContinue_[XYZ_Z]				//Z
	};

	//�R���e�B�j���[����Ƃ��̎��@��]�C�[�W���O�̃v���Z�b�g
	const Easing presetEasePlayerRotateContinue_[XYZ_Num]
	{
		{90.0f, 0.0f, 1.0f},				//X
		{-90.0f, 90.0f, 1.0f},				//Y
		{0.0f, 0.0f, 1.0f}					//Z
	};
	//�R���e�B�j���[����Ƃ��̎��@��]�C�[�W���O
	Easing easePlayerRotateContinue_[XYZ_Num]
	{
		presetEasePlayerRotateContinue_[XYZ_X],				//X
		presetEasePlayerRotateContinue_[XYZ_Y],				//Y
		presetEasePlayerRotateContinue_[XYZ_Z]				//Z
	};

	//�R���e�B�j���[����Ƃ��̎��@�ړ��C�[�W���O�̃v���Z�b�g
	const Easing presetEasePlayerMoveContinue_[XYZ_Num]
	{
		{0.0f, 90.0f, 1.0f},				//X
		{-8.0f, -8.0f, 1.0f},				//Y
		{-60.0f, -60.0f, 1.0f}				//Z
	};
	//�R���e�B�j���[����Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveContinue_[XYZ_Num]
	{
		presetEasePlayerMoveContinue_[XYZ_X],				//X
		presetEasePlayerMoveContinue_[XYZ_Y],				//Y
		presetEasePlayerMoveContinue_[XYZ_Z]				//Z
	};

	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��_�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseEyeQuitStageSelect_[XYZ_Num]
	{
		{-6.0f, -21.0f, 1.0f},				//X
		{-8.0f, -5.0f, 1.0f},				//Y
		{-110.0f, -60.0f, 1.0f}				//Z
	};
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��_�C�[�W���O
	Easing easeEyeQuitStageSelect_[XYZ_Num]
	{
		presetEaseEyeQuitStageSelect_[XYZ_X],				//X
		presetEaseEyeQuitStageSelect_[XYZ_Y],				//Y
		presetEaseEyeQuitStageSelect_[XYZ_Z]				//Z
	};

	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̒����_�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseTargetQuitStageSelect_[XYZ_Num]
	{
		{-32.0f, 90.0f, 1.0f},				//X
		{-24.0f, -22.0f, 1.0f},				//Y
		{-10.0f, -61.0f, 1.0f}				//Z
	};
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̒����_�C�[�W���O
	Easing easeTargetQuitStageSelect_[XYZ_Num]
	{
		presetEaseTargetQuitStageSelect_[XYZ_X],				//X
		presetEaseTargetQuitStageSelect_[XYZ_Y],				//Y
		presetEaseTargetQuitStageSelect_[XYZ_Z]					//Z
	};

	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@��]�C�[�W���O�̃v���Z�b�g
	const Easing presetEasePlayerRotateQuitStageSelect_[XYZ_Num]
	{
		{90.0f, 0.0f, 1.0f},				//X
		{-90.0f, 90.0f, 1.0f},				//Y
		{0.0f, 0.0f, 1.0f}					//Z
	};
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@��]�C�[�W���O
	Easing easePlayerRotateQuitStageSelect_[XYZ_Num]
	{
		presetEasePlayerRotateQuitStageSelect_[XYZ_X],				//X
		presetEasePlayerRotateQuitStageSelect_[XYZ_Y],				//Y
		presetEasePlayerRotateQuitStageSelect_[XYZ_Z]				//Z
	};

	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@�ړ��C�[�W���O�̃v���Z�b�g
	const Easing presetEasePlayerMoveQuitStageSelect_[XYZ_Num]
	{
		{0.0f, 150.0f, 1.0f},					//X
		{-8.0f, 20.0f, 1.0f},					//Y
		{-60.0f, -60.0f, 1.0f}					//Z
	};
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveQuitStageSelect_[XYZ_Num]
	{
		presetEasePlayerMoveQuitStageSelect_[XYZ_X],					//X
		presetEasePlayerMoveQuitStageSelect_[XYZ_Y],					//Y
		presetEasePlayerMoveQuitStageSelect_[XYZ_Z]						//Z
	};

	//�t�F�[�h�C���A�E�g�̃v���Z�b�g
	const Easing presetEaseFadeInOut_ = { 1.0f, 0.0f, 1.0f };
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb
	//�I�����Ă��郁�j���[�\��
	int menuCount_ = GOSMI_Continue;
	//�F���]�t���O
	bool isColorReverse_ = false;

	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pm1_ = nullptr;

};