#pragma once
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
	StageClearScene(const int stagenum);
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
	void FadeIn(const DirectX::XMFLOAT3& color)override;
	//�I��
	void Finalize() override;

	//���x���f�[�^�ǂݍ���
	void LoadLVData(const std::string& stagePath);

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
		SCSTI_CursorTex = 8,
		SCSTI_StageNameTex = 9,
		SCSTI_MenuUITex = 10,
	};
	//�X�e�[�W�N���A�p���j���[�C���f�b�N�X
	enum StageClearSceneMenuIndex
	{
		SCSMI_NextStage = 0,
		SCSMI_StageSelect = 1,
		SCSMI_Title = 2,
	};

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
	std::unique_ptr<Sprite> spriteDone_ =std::make_unique<Sprite>();			//����\�����X�L�b�v�L�[�\���̃X�v���C�g
	std::unique_ptr<Sprite> spriteFadeInOut_ =std::make_unique<Sprite>();		//�t�F�[�h�C���A�E�g�̃X�v���C�g
	std::unique_ptr<Sprite> spriteLoad_ =std::make_unique<Sprite>();			//���[�h�X�v���C�g
	std::unique_ptr<Sprite> spriteStageInfoNow_ =std::make_unique<Sprite>();	//���݃X�e�[�W�X�v���C�g
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();			//�J�[�\���X�v���C�g
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();		//�X�e�[�W���X�v���C�g
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();			//���j���[������@�X�v���C�g

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

	//�t���O��
	bool isFinalStage_ = false;					//�ŏI�ʂ̏ꍇ
	bool isNextStage_ = false;					//���̃X�e�[�W�֍s���ꍇ
	bool isStageSelect_ = false;				//�X�e�[�W�Z���N�g�֍s���ꍇ
	bool isQuitTitle_ = false;					//�^�C�g���ɖ߂�ꍇ
	bool isFadeIn_ = false;						//�t�F�[�h�C��

	//���j���[�����p�̗񋓑�
	enum StageClearMenuEasingNum
	{
		SCMEN_Menu = 0,					//���j���[
		SCMEN_NextStage = 1,			//�R���e�B�j���[��
		SCMEN_StageSelect = 2,			//�X�e�[�W�Z���N�g��
		SCMEN_Title = 3,				//�^�C�g����
		SCMEN_SelectSpace = 4,			//�X�y�[�X�őI��
		SCMEN_UI = 5,					//������@
		SCMEN_Num = 6,					//�z��p
	};
	//�C�[�W���O��
	//���j���[�\���p�̃C�[�W���O
	Easing easeMenuPosX_[SCMEN_Num];
	//���j���[�I���p�̃C�[�W���O
	Easing easeMenuEndPosX_[SCMEN_Num];
	//�J�[�\��X�l�̃C�[�W���O
	Easing easeCursorPosX_;

	//���j���[�|�W�V����
	const std::array<float, SCMEN_Num> menuPosY_ = { 50.0f,150.0f,300.0f,450.0f,550.0f,300.0f };

	//���̃X�e�[�W�֍s���Ƃ��̎��_�C�[�W���O
	Easing easeEyeStageClear_[XYZ_Num];
	//���̃X�e�[�W�֍s���Ƃ��̒����_�C�[�W���O
	Easing easeTargetStageClear_[XYZ_Num];
	//�X�e�[�W�Z���N�g�֍s���Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveStageSelect_[XYZ_Num];
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_;

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb

	//�F���]�t���O
	bool isColorReverse_ = false;
	//���C�g
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleClear_ = nullptr;	//�N���A���o(�S�[���G�t�F�N�g�ɂ��g���܂킷)
	std::unique_ptr<Particle> particleFire_ = nullptr;	//��
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmClear_ = nullptr;	//�N���A���o(�S�[���G�t�F�N�g�ɂ��g���܂킷)
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;		//��

	//�J�n���̃|�W�V����
	DirectX::XMFLOAT3 startPos_;

private:
	//�F���ς�鏈��
	void UpdateChangeColor();
	//�X�e�[�W���̃X�v���C�g�����[�h
	void LoadStageNameSprite();
	//�ۉe�Z�b�g
	void SetUpCircleShadow(const DirectX::XMFLOAT3& pos);
};
