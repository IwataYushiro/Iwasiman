#pragma once
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "LevelLoaderJson.h"

#include "ParticleManager.h"
#include "Sprite.h"
#include "EnumList.h"
#include "XYZ.h"

#include <map>
#include <memory>

#include "SceneManager.h"

/*

*	TitleScene.h

*	�^�C�g���V�[��

*/
class TitleScene :public BaseScene
{
public://�����o�֐�(�X�e�[�W�ԍ�)
	TitleScene(const int stagenum);
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//��ԍX�V(�Q�[���J�n����Ƃ�)
	void UpdateIsStartGame();
	//��ԍX�V(�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ�)
	void UpdateIsStageSelect();
	//��ԍX�V(�X�^�[�g�ɖ߂����Ƃ�)
	void UpdateIsBack();
	//��ԍX�V(���j���[�̂Ƃ�) 
	void UpdateIsMenu();
	//�t�F�[�h�A�E�g(�F)
	void FadeIn(const DirectX::XMFLOAT3& color) override;
	//�`��
	void Draw() override;
	//�|�X�g�G�t�F�N�g�`��
	void DrawPostEffect() override;
	//�I��
	void Finalize() override;
	//���x���f�[�^�ǂݍ���
	void LoadLVData(const std::string& stagePath);
	//�C�[�W���O�̃��[�h
	void LoadEasing() override;
	//�|�X�g�G�t�F�N�g�e�X�g�p
	void TestPostEffect(const std::string& path);
private://��Ճ����o�ϐ�
	//DirectX���
	DirectXCommon* dxCommon_ = nullptr;
	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;
	//�C���v�b�g
	Input* input_ = nullptr;
	//�I�[�f�B�I
	Audio* audio_ =nullptr;
	//�V�[���}�l�[�W���[
	SceneManager* sceneManager_ = nullptr;
	//imgui
	ImGuiManager* imguiManager_ = nullptr;


private://�����o�ϐ�
	//�^�C�g���p�e�N�X�`���C���f�b�N�X
	enum TitleSceneTextureIndex
	{
		TSTI_TitleTex = 0,
		TSTI_MenuTex = 1,
		TSTI_MenuTutorialTex = 2,
		TSTI_MenuStageSerectTex = 3,
		TSTI_MenuDoneTex = 4,
		TSTI_BackTitleTex = 5,
		TSTI_FadeInOutTex = 6,
		TSTI_LoadingTex = 7,
		TSTI_StageInfoNowTex = 8,
		TSTI_CursorTex = 9,
		TSTI_TitleBackTex = 10,
		TSTI_StageNameTex = 11,
		TSTI_MenuUITex = 12,
	};

	
	//�J����
	std::unique_ptr<Camera> camera_ = nullptr;

	//�I�����Ă��郁�j���[�\��
	int menuCount_;

	//Sprite
	std::unique_ptr<Sprite> spriteTitle_ = std::make_unique<Sprite>();				//�^�C�g����ʃX�v���C�g
	std::unique_ptr<Sprite> spriteMenu_ = std::make_unique<Sprite>();				//�^�C�g�����j���[��ʃX�v���C�g
	std::unique_ptr<Sprite> spriteMenuTutorial_ = std::make_unique<Sprite>();		//�`���[�g���A���\���X�v���C�g
	std::unique_ptr<Sprite> spriteMenuStageSelect_ = std::make_unique<Sprite>();	//�X�e�[�W�Z���N�g�\���X�v���C�g
	std::unique_ptr<Sprite> spriteMenuDone_ = std::make_unique<Sprite>();			//�^�C�g�����j���[����\�����X�L�b�v�L�[�\���X�v���C�g
	std::unique_ptr<Sprite> spriteBack_ = std::make_unique<Sprite>();				//�^�C�g�����j���[���^�C�g���ڍs�̃X�v���C�g
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//�t�F�[�h�C���A�E�g�X�v���C�g
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//���[�h�X�v���C�g
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//���݃X�e�[�W�X�v���C�g
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//�J�[�\���X�v���C�g
	std::unique_ptr<Sprite> spriteTitleBack_ = std::make_unique<Sprite>();			//�^�C�g����ʃX�v���C�g(�^�C�g���̌��)
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();			//�X�e�[�W���X�v���C�g
	std::unique_ptr<Sprite> spriteMenuUI_ = std::make_unique<Sprite>();				//���j���[������@�X�v���C�g

	//���f��
	std::unique_ptr<Model> modelPlayer_ = nullptr;					//���@���f��
	std::unique_ptr<Model> modelSkydome_ = nullptr;					//�`���[�g���A���X�e�[�W���f��(�V��)
	std::unique_ptr<Model> modelSkydomeStage1_ = nullptr;			//�X�e�[�W1���f��(�V��)
	std::unique_ptr<Model> modelSkydomeStage2_ = nullptr;			//�X�e�[�W2���f��(�V��)
	std::unique_ptr<Model> modelGround_ = nullptr;					//�����f��
	std::unique_ptr<Model> modelGoal_ = nullptr;					//�S�[�����f��

	std::vector<std::unique_ptr<Object3d>> objPlayers_;					//���@�I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objSkydomes_;				//�V���I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objGrounds_;					//���I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objGoals_;					//�S�[���I�u�W�F�N�g�z��


	//�t���O��
	bool isMenu_ = false;				//�^�C�g�����j���[��ʂɂ���Ƃ�
	bool isBack_ = false;				//�^�C�g�����j���[����^�C�g���ɖ߂�Ƃ�
	bool isStartGame_ = false;			//�Q�[���J�n����Ƃ�
	bool isStageSelect_ = false;		//�X�e�[�W�Z���N�g�ɍs���Ƃ�
	bool isFadeIn_ = false;				//�t�F�[�h�C��

	
	//�ŏ��̉�ʂ�Y�l
	const float startTitlePosY_ = 50.0f;

	//�^�C�g�����^�C�g�����j���[�̃C�[�W���O
	Easing easeTitlePosX_;


	//���j���[�����p�̗񋓑�
	enum TitleMenuEasingNum
	{
		TMEN_Menu = 0,				//���j���[
		TMEN_Tutorial = 1,			//�`���[�g���A����
		TMEN_StageSelect = 2,		//�X�e�[�W�Z���N�g��
		TMEN_SelectSpace = 3,		//�X�y�[�X�őI��
		TMEN_UI = 4,				//������@
		TMEN_Quit = 5,				//�߂�
		TMEN_Num = 6,				//�z��p
	};
	//���j���[��Y�l
	const std::array<float, TMEN_Num> menuPosY_ = { 50.0f,200.0f,350.0f,600.0f,300.0f,50.0f };
	
	//�^�C�g�����j���[�̏o���C�[�W���O
	Easing easeMenuPosX_[TMEN_Num];
	//�^�C�g�����j���[�̒ʉ߃C�[�W���O
	Easing easeMenuEndPosX_[TMEN_Num];
	//�J�[�\��X�l�̃C�[�W���O
	Easing easeCursorPosX_;
	//�I�񂾃X�e�[�W��^�񒆂Ɉړ�������C�[�W���O
	Easing easeStartStagePosX_;//�`���[�g���A����
	//�I�񂾃X�e�[�W����Ɉړ�������C�[�W���O
	Easing easeStartStagePosY_;//�`���[�g���A����

	//�^�C�g�����^�C�g�����j���[�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeMenu_[XYZ_Num];
	//�^�C�g�����^�C�g�����j���[�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetMenu_[XYZ_Num];
	//�^�C�g�����j���[���Q�[���J�n�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeGameStart_[XYZ_Num];
	//�^�C�g�����j���[���Q�[���J�n�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetGameStart_[XYZ_Num];
	//�^�C�g�����j���[���X�e�[�W�Z���N�g�̎��@�ړ��C�[�W���O
	Easing easePlayerMove_[XYZ_Num];
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_;

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_;//xyz=rgb

	//�^�C�g�����j���[���^�C�g���֖߂�X�v���C�g�̐F
	const DirectX::XMFLOAT4 backTitleColor_ = { 0.0f,0.0f,0.1f,1.0f };

	//�F���]�t���O
	bool isColorReverse_ = false;
	//���C�g
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;//��
	std::unique_ptr<Particle> particleGoal_ = nullptr;//�S�[��
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;//��
	std::unique_ptr<ParticleManager> pmGoal_ = nullptr;//�S�[��

	//�J�n���̃|�W�V����
	DirectX::XMFLOAT3 startPos_;

	//�ǂ̃X�e�[�W�ɂ���̂����󂯎�邽�߂̕ϐ�
	int stageNum_;
	//�_�[�e�B�t���O
	bool dirty_ = false;
public:
	//�F���ς�鏈��
	void UpdateChangeColor();
	//�ۉe�Z�b�g
	void SetUpCircleShadow(const DirectX::XMFLOAT3& pos);
};
