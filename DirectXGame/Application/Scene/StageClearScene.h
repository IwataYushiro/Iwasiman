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

#include <map>

#include "SceneManager.h"

//json���x���f�[�^
struct LevelData;

/*

*	StageClearScene.h

*	�X�e�[�W�N���A�V�[��

*/
class StageClearScene :public BaseScene
{
public://�����o�֐�
	//�R���X�g���N�^(�X�e�[�W�ԍ�)
	StageClearScene(int stagenum);
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//��ԍX�V(�Q�[���J�n����Ƃ�)
	void UpdateIsNextStage();
	//��ԍX�V(�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ�)
	void UpdateIsStageSelect();
	//��ԍX�V(�^�C�g���֖߂�Ƃ�)
	void UpdateIsQuitTitle();
	//��ԍX�V(���j���[�̂Ƃ�) 
	void UpdateIsMenu();
	//�`��
	void Draw() override;
	//�t�F�[�h�A�E�g
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//�I��
	void Finalize() override;

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
	//�X�e�[�W�N���A�p�e�N�X�`���C���f�b�N�X
	enum StageClearSceneTextureIndex
	{
		SCSTI_MenuTex = 0,
		SCSTI_NextStageTex = 1,
		SCSTI_StageSelectTex = 2,
		SCSTI_TitleTex = 3,
		SCSTI_MenuDoneTex = 4,
		SCSTI_FadeInOutTex = 5,
		SCSTI_LoadingTex = 6,
		SCSTI_StageInfoNowTex = 7,
	};
	//�X�e�[�W�N���A�p���j���[�C���f�b�N�X
	enum StageClearSceneMenuIndex
	{
		SCSMI_NextStage = 0,
		SCSMI_StageSelect = 1,
		SCSMI_Title = 2,
	};

	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;
	//�J����
	std::unique_ptr<Camera> camera_ = nullptr;
	
	//�X�e�[�W�ԍ�
	int stageNum_;
	//���j���[�ԍ�
	int menuCount_ = SCSMI_NextStage;
	//Sprite
	std::unique_ptr<Sprite> spriteStageClear_ =std::make_unique<Sprite>();		//�X�e�[�W�N���A���̃X�v���C�g
	std::unique_ptr<Sprite> spriteNextStage_ =std::make_unique<Sprite>();		//���̃X�e�[�W�\���̃X�v���C�g
	std::unique_ptr<Sprite> spriteStageSelect_ =std::make_unique<Sprite>();		//�X�e�[�W�Z���N�g�\���̃X�v���C�g
	std::unique_ptr<Sprite> spriteTitle_ =std::make_unique<Sprite>();			//�^�C�g���\���̃X�v���C�g
	std::unique_ptr<Sprite> spriteDone_ =std::make_unique<Sprite>();			//����\���̃X�v���C�g
	std::unique_ptr<Sprite> spriteFadeInOut_ =std::make_unique<Sprite>();		//�t�F�[�h�C���A�E�g�̃X�v���C�g
	std::unique_ptr<Sprite> spriteLoad_ =std::make_unique<Sprite>();			//���[�h�X�v���C�g
	std::unique_ptr<Sprite> spriteStageInfoNow_ =std::make_unique<Sprite>();	//���݃X�e�[�W�X�v���C�g

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;			

	//���f��
	std::unique_ptr<Model> modelPlayer_ = nullptr;				//���@���f��
	std::unique_ptr<Model> modelSkydome_ = nullptr;				//�`���[�g���A���X�e�[�W���f��(�V��)
	std::unique_ptr<Model> modelSkydomeStage1_ = nullptr;		//�X�e�[�W1���f��(�V��)
	std::unique_ptr<Model> modelSkydomeStage2_ = nullptr;		//�X�e�[�W2���f��(�V��)
	std::unique_ptr<Model> modelGround_ = nullptr;				//���̃��f��
	std::unique_ptr<Model> modelGoal_ = nullptr;				//�S�[�����f��

	std::vector<std::unique_ptr<Object3d>> objPlayers_;			//���@�z��
	std::vector<std::unique_ptr<Object3d>> objSkydomes_;		//�V���z��
	std::vector<std::unique_ptr<Object3d>> objGrounds_;			//���z��
	std::vector<std::unique_ptr<Object3d>> objGoals_;			//�S�[���z��

	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;

	//�t���O��
	bool isFinalStage_ = false;					//�ŏI�ʂ̏ꍇ
	bool isNextStage_ = false;					//���̃X�e�[�W�֍s���ꍇ
	bool isStageSelect_ = false;				//�X�e�[�W�Z���N�g�֍s���ꍇ
	bool isQuitTitle_ = false;					//�^�C�g���ɖ߂�ꍇ
	bool isFadeOut_ = false;					//�t�F�[�h�C���A�E�g

	//���j���[�����p�̗񋓑�
	enum StageClearMenuEasingNum
	{
		SCMEN_Menu = 0,					//���j���[
		SCMEN_NextStage = 1,			//�R���e�B�j���[��
		SCMEN_StageSelect = 2,			//�X�e�[�W�Z���N�g��
		SCMEN_Title = 3,				//�^�C�g����
		SCMEN_SelectSpace = 4,			//�X�y�[�X�őI��
		SCMEN_Num = 5,					//�z��p
	};
	//�C�[�W���O��
	//���j���[�\���p�̃C�[�W���O�̃v���Z�b�g
	const Easing presetEaseMenuPosX_[SCMEN_Num]
	{
		{1300.0f, 200.0f, 1.0f},			//���j���[
		{1300.0f, 0.0f, 1.2f},				//���̃X�e�[�W��
		{1300.0f, 0.0f, 1.4f},				//�X�e�[�W�Z���N�g��
		{1300.0f, 0.0f, 1.6f},				//�^�C�g����
		{1300.0f, 425.0f, 2.0f}				//�X�y�[�X�őI��
	};
	//���j���[�\���p�̃C�[�W���O
	Easing easeMenuPosX_[SCMEN_Num]
	{
		presetEaseMenuPosX_[SCMEN_Menu],				//���j���[
		presetEaseMenuPosX_[SCMEN_NextStage],			//���̃X�e�[�W��
		presetEaseMenuPosX_[SCMEN_StageSelect],			//�X�e�[�W�Z���N�g��
		presetEaseMenuPosX_[SCMEN_Title],				//�^�C�g����
		presetEaseMenuPosX_[SCMEN_SelectSpace],			//�X�y�[�X�őI��
	};
	//���j���[�|�W�V����
	const std::array<float, SCMEN_Num> menuPosY_ = { 50.0f,150.0f,300.0f,450.0f,550.0f };

	//���̃X�e�[�W�֍s���Ƃ��̎��_�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseEyeStageClear_[XYZ_Num]
	{
		{0.0f, -22.0f, 1.8f},				//X
		{1.0f, -1.0f, 1.8f},				//Y
		{-110.0f, -60.0f, 1.8f},			//Z
	};
	//���̃X�e�[�W�֍s���Ƃ��̎��_�C�[�W���O
	Easing easeEyeStageClear_[XYZ_Num]
	{
		presetEaseEyeStageClear_[XYZ_X],			//X
		presetEaseEyeStageClear_[XYZ_Y],			//Y
		presetEaseEyeStageClear_[XYZ_Z]				//Z
	};

	//���̃X�e�[�W�֍s���Ƃ��̒����_�C�[�W���O�̃v���Z�b�g
	const Easing presetEaseTargetStageClear_[XYZ_Num]
	{
		{0.0f, 50.0f, 1.8f},				//X
		{0.0f, -8.0f, 1.8f},				//Y
		{-10.0f, -57.0f, 1.8f},				//Z
	};
	//���̃X�e�[�W�֍s���Ƃ��̒����_�C�[�W���O
	Easing easeTargetStageClear_[XYZ_Num]
	{
		 presetEaseTargetStageClear_[XYZ_X],			//X
		 presetEaseTargetStageClear_[XYZ_Y],			//Y
		 presetEaseTargetStageClear_[XYZ_Z]				//Z
	};

	//�X�e�[�W�Z���N�g�֍s���Ƃ��̎��@�ړ��C�[�W���O�̃v���Z�b�g
	const Easing presetEasePlayerMoveStageSelect_[XYZ_Num]
	{
		{0.0f, 150.0f, 2.0f},				//X
		{-8.0f, 40.0f, 2.0f},				//Y
		{-60.0f, -60.0f, 2.0f},				//Z
	};
	//�X�e�[�W�Z���N�g�֍s���Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveStageSelect_[XYZ_Num]
	{
		presetEasePlayerMoveStageSelect_[XYZ_X],			//X
		presetEasePlayerMoveStageSelect_[XYZ_Y],			//Y
		presetEasePlayerMoveStageSelect_[XYZ_Z],			//Z
	};

	//�t�F�[�h�C���A�E�g�̃v���Z�b�g
	const Easing presetEaseFadeInOut_ = { 1.0f, 0.0f, 1.0f };
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = presetEaseFadeInOut_;

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb

	//�F���]�t���O
	bool isColorReverse_ = false;
	//���C�g
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleClear_ = nullptr;
	std::unique_ptr<Particle> particleSmoke_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmClear_ = nullptr;
	std::unique_ptr<ParticleManager> pmSmoke_ = nullptr;

	//�J�n���̃|�W�V����
	DirectX::XMFLOAT3 startPos_;

public:
	//�F���ς�鏈��
	void UpdateChangeColor();

};
