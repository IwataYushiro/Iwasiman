#pragma once

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
	GameOverScene(const int stagenum);
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
	//�|�X�g�G�t�F�N�g�`��
	void DrawPostEffect() override;
	//�I��
	void Finalize() override;
	//���x���f�[�^�ǂݍ���(�X�e�[�W�t�@�C���p�X)
	void LoadLVData(const std::string& stagePath);
	//�t�F�[�h�A�E�g
	void FadeIn(const DirectX::XMFLOAT3& color)override;
	//�C�[�W���O�̃��[�h
	void LoadEasing() override;
	//�F���ς�鏈��
	void UpdateChangeColor();
private://��Ճ����o�ϐ�
	//DirectX���
	DirectXCommon* dxCommon_ = nullptr;
	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;
	//�C���v�b�g
	Input* input_ = nullptr;
	//�I�[�f�B�I
	Audio* audio_ = nullptr;
	//�V�[���}�l�[�W���[
	SceneManager* sceneManager_ = nullptr;
	//imgui
	ImGuiManager* imguiManager_ = nullptr;

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
		GOSTI_CursorTex = 8,
		GOSTI_StageNameTex = 9,
		GOSTI_MenuUITex = 10,
	};

	//�Q�[���I�[�o�[�p���j���[�C���f�b�N�X
	enum GameOverSceneMenuIndex
	{
		GOSMI_Continue = 0,
		GOSMI_StageSelect = 1,
		GOSMI_Title = 2,
	};
	//�J����
	std::unique_ptr<Camera> camera_ = nullptr;

	//�X�e�[�W�ԍ�
	int32_t stageNum_;
	//Sprite
	std::unique_ptr<Sprite> spriteGameOver_ = std::make_unique<Sprite>();			//�Q�[���I�[�o�[�X�v���C�g
	std::unique_ptr<Sprite> spriteContinue_ = std::make_unique<Sprite>();			//�R���e�B�j���[�\���X�v���C�g
	std::unique_ptr<Sprite> spriteStageSelect_ = std::make_unique<Sprite>();		//�X�e�[�W�Z���N�g�\���X�v���C�g
	std::unique_ptr<Sprite> spriteTitle_ = std::make_unique<Sprite>();				//�^�C�g���\���X�v���C�g
	std::unique_ptr<Sprite> spriteDone_ = std::make_unique<Sprite>();				//���茓�X�L�b�v�L�[�X�v���C�g
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//�t�F�[�h�C���A�E�g�X�v���C�g
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//���[�h�X�v���C�g
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//���݃X�e�[�W�X�v���C�g
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//�J�[�\���X�v���C�g
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();			//�X�e�[�W���X�v���C�g
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();				//���j���[������@�X�v���C�g

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;

	//���f��
	std::unique_ptr<Model> modelPlayer_ = nullptr;				//���@���f��
	std::unique_ptr<Model> modelPlayerContinue_ = nullptr;		//���@���f���i�R���e�B�j���[�j
	std::unique_ptr<Model> modelGoal_ = nullptr;				//�S�[�����f��
	std::unique_ptr<Model> modelStageTutorial_ = nullptr;		//�`���[�g���A���X�e�[�W���f��(�V��)
	std::unique_ptr<Model> modelStage1_ = nullptr;				//�X�e�[�W1���f��(�V��)
	std::unique_ptr<Model> modelStage2_ = nullptr;				//�X�e�[�W2���f��(�V��)

	std::vector<std::unique_ptr<Object3d>> objPlayers_;			//���@�I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objGoals_;			//�S�[���I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objStages_;			//�X�e�[�W�I�u�W�F�N�g�z��

	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot_;

	//�I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objects_;

	//�t���O��
	bool isGameover_ = true;					//���j���[���t���O
	bool isContinue_ = false;					//�R���e�B�j���[����ꍇ
	bool isQuitStageSelect_ = false;			//�X�e�[�W�Z���N�g�ɖ߂�ꍇ
	bool completeRotate_ = false;				//���I������� 
	bool isQuitTitle_ = false;					//�^�C�g���ɖ߂�ꍇ
	bool isFadeIn_ = false;						//�t�F�[�h�C��

	//���j���[�����p�̗񋓑�
	enum GameOverMenuEasingNum
	{
		GOMEN_Menu = 0,				//���j���[
		GOMEN_Continue = 1,			//�R���e�B�j���[��
		GOMEN_StageSelect = 2,		//�X�e�[�W�Z���N�g��
		GOMEN_Title = 3,			//�^�C�g����
		GOMEN_SelectSpace = 4,		//�X�y�[�X�őI��
		GOMEN_UI = 5,				//������@
		GOMEN_Num = 6,				//�z��p
	};
	//���j���[�|�W�V����
	const std::array<float, GOMEN_Num> menuPosY_ = { 50.0f,150.0f,300.0f,450.0f,600.0f,300.0f };

	//���j���[�\���p�̃C�[�W���O
	Easing easeMenuPosX_[GOMEN_Num];
	//���j���[�I���p�̃C�[�W���O
	Easing easeMenuEndPosX_[GOMEN_Num];
	//�J�[�\��X�l�̃C�[�W���O
	Easing easeCursorPosX_;
	//�R���e�B�j���[������^�񒆂Ɉړ�������C�[�W���O
	Easing easeContinuePosX_;
	//�R���e�B�j���[��������Ɉړ�������C�[�W���O
	Easing easeContinuePosY_;
	//�R���e�B�j���[����Ƃ��̎��_�C�[�W���O
	Easing easeEyeContinue_[XYZ_Num];
	//�R���e�B�j���[����Ƃ��̒����_�C�[�W���O
	Easing easeTargetContinue_[XYZ_Num];
	//�R���e�B�j���[����Ƃ��̎��@��]�C�[�W���O
	Easing easePlayerRotateContinue_[XYZ_Num];
	//�R���e�B�j���[����Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveContinue_[XYZ_Num];
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��_�C�[�W���O
	Easing easeEyeQuitStageSelect_[XYZ_Num];
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̒����_�C�[�W���O
	Easing easeTargetQuitStageSelect_[XYZ_Num];
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@��]�C�[�W���O
	Easing easePlayerRotateQuitStageSelect_[XYZ_Num];
	//�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveQuitStageSelect_[XYZ_Num];
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_;

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb
	//�I�����Ă��郁�j���[�\��
	int menuCount_ = GOSMI_Continue;
	//�F���]�t���O
	bool isColorReverse_ = false;

	//���C�g
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;		//��
	std::unique_ptr<Particle> particleFall_ = nullptr;		//�������̃p�[�e�B�N��
	std::unique_ptr<Particle> particleSmoke_ = nullptr;		//��
	std::unique_ptr<Particle> particleGoal_ = nullptr;		//�S�[�����o
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;		//��(�������̃p�[�e�B�N���ɂ��g�p)
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;	//��
	std::unique_ptr<ParticleManager> pmGoal_ = nullptr;		//�S�[�����o

private:
	//�����Ă�Ƃ��̃p�[�e�B�N��(�v���C���[)
	void FallParticle(const std::unique_ptr<Object3d>& player);
	//��]�C�[�W���O�̃Z�b�g
	void EaseRotateSetUp(const DirectX::XMFLOAT3& rotation, Easing& easing, const int32_t num);

	//�X�e�[�W���̃X�v���C�g�����[�h
	void LoadStageNameSprite();
};