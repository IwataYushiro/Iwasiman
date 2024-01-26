#pragma once
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
	StageSelectScene(const int count);
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
	void FadeIn(const DirectX::XMFLOAT3& color) override;
	//���x���f�[�^�ǂݍ���
	void LoadLVData(const std::string& stagePath);
	//�C�[�W���O�̃��[�h
	void LoadEasing() override;

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
		SSSTI_CursorTex = 9,
		SSSTI_StageNameTex = 10,
		SSSTI_MenuUITex = 11,
	};

	//�J����
	std::unique_ptr<Camera> camera_ = nullptr;

	//Sprite
	std::unique_ptr<Sprite> spriteMenu_ = std::make_unique<Sprite>();				//���j���[��ʃX�v���C�g
	std::unique_ptr<Sprite> spriteTutorial_ = std::make_unique<Sprite>();			//�`���[�g���A���X�e�[�W�X�v���C�g
	std::unique_ptr<Sprite> spriteStage1_ = std::make_unique<Sprite>();				//�X�e�[�W1�X�v���C�g
	std::unique_ptr<Sprite> spriteStage2_ = std::make_unique<Sprite>();				//�X�e�[�W2�X�v���C�g
	std::unique_ptr<Sprite> spriteDone_ = std::make_unique<Sprite>();				//����\�����X�L�b�v�L�[�X�v���C�g
	std::unique_ptr<Sprite> spriteBack_ = std::make_unique<Sprite>();				//�^�C�g���ɖ߂�X�v���C�g
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//�t�F�[�h�C���A�E�g�X�v���C�g
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//���[�h�X�v���C�g
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//���݃X�e�[�W�X�v���C�g
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//�J�[�\���X�v���C�g
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();			//�X�e�[�W���X�v���C�g
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();				//���j���[������@�X�v���C�g

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;

	//�I�u�W�F�N�g
	std::unique_ptr<Object3d> objStage_ = nullptr;				//�X�e�[�W�I�u�W�F�N�g
	std::vector<std::unique_ptr<Object3d>> objPlayers_;			//���@�I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objGoals_;			//�S�[���I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objGrounds_;			//���I�u�W�F�N�g�z��

	//���f��
	std::unique_ptr<Model> modelPlayer_ = nullptr;				//���@���f��
	std::unique_ptr<Model> modelGoal_ = nullptr;				//�S�[�����f��
	std::unique_ptr<Model> modelGround_ = nullptr;				//�����f��
	std::unique_ptr<Model> modelStageTutorial_ = nullptr;		//�`���[�g���A���X�e�[�W���f��(�V��)
	std::unique_ptr<Model> modelStage1_ = nullptr;				//�X�e�[�W1���f��(�V��)
	std::unique_ptr<Model> modelStage2_ = nullptr;				//�X�e�[�W2���f��(�V��)
	
	//�I�u�W�F�N�g��]�p
	DirectX::XMFLOAT3 rot_;

	//���j���[�����p�̗񋓑�
	enum StageSelectMenuEasingNum
	{
		SSMEN_Menu = 0,				//���j���[
		SSMEN_Tutorial = 1,			//�`���[�g���A��
		SSMEN_Stage1_Sky = 2,		//�X�e�[�W1
		SSMEN_Stage2_Space = 3,		//�X�e�[�W2
		SSMEN_SelectSpace = 4,		//�X�y�[�X�őI��
		SSMEN_UI = 5,				//������@
		SSMEN_Quit = 6,				//�߂�
		SSMEN_Num = 7,				//�z��p
	};
	//���j���[�|�W�V����
	const std::array<float, SSMEN_Num> menuPosY_ = { 0.0f,150.0f,300.0f,450.0f,600.0f,300.0f,50.0f };

	//�C�[�W���O��
	//���j���[��ʏo���C�[�W���O
	Easing easeMenuPosX_[SSMEN_Num];
	//���j���[��ʒʉ߃C�[�W���O
	Easing easeMenuEndPosX_[SSMEN_Num];
	//�J�[�\��X�l�̃C�[�W���O
	Easing easeCursorPosX_;
	//�I�񂾃X�e�[�W��^�񒆂Ɉړ�������C�[�W���O
	Easing easeStartStagePosX_[SSSMI_Num];
	//�I�񂾃X�e�[�W����Ɉړ�������C�[�W���O
	Easing easeStartStagePosY_[SSSMI_Num];
	//�J�ڎ������ɋN������X�e�[�W�Z���N�g���_�J�������[�N�C�[�W���O
	Easing easeEyeStageSelect_[XYZ_Num];
	//�J�ڎ������ɋN������X�e�[�W�Z���N�g�����_�J�������[�N�C�[�W���O
	Easing easeTargetStageSelect_[XYZ_Num];
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeDoneMenu_[XYZ_Num];
	//�X�e�[�W���莞�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetDoneMenu_[XYZ_Num];
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓����_�J�������[�N�C�[�W���O
	Easing easeEyeGameStart_[XYZ_Num];
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̓˓������_�J�������[�N�C�[�W���O
	Easing easeTargetGameStart_[XYZ_Num];
	//�X�e�[�W���莞�̎��_�J�������[�N�C�[�W���O���I�������̎��@�ړ��C�[�W���O
	Easing easePlayerStartMove_[XYZ_Num];
	//�^�C�g���ɖ߂�Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerQuitMove_[XYZ_Num];
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_;

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
	bool isFadeIn_ = false;			//�t�F�[�h�C��

	//���C�g
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;//��
	std::unique_ptr<Particle> particleGoal_ = nullptr;//�S�[��
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;//��
	std::unique_ptr<ParticleManager> pmGoal_ = nullptr;//�S�[��

public:
	//�F���ς�鏈��
	void UpdateChangeColor();
	//�ۉe�Z�b�g
	void SetUpCircleShadow(const DirectX::XMFLOAT3& pos);
};
