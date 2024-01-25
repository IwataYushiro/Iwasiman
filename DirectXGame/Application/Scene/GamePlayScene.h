#pragma once
#include "Audio.h"
#include "Camera.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "TouchableObject.h"

#include "Goal.h"
#include "Player.h"
#include "EnemyFactory.h"
#include "GimmickFactory.h"

#include "SceneManager.h"
#include "CollisionPrimitive.h"
#include "Easing.h"

#include "Item.h"
#include "EnumList.h"
#include "XYZ.h"

#include <vector>

#include <sstream>
#include <string>

//json���x���f�[�^
struct LevelData;

class CollisionManager;
class TouchableObject;

/*

*	GamyPlayScene.h

*	�Q�[���v���C�V�[��

*/
class GamePlayScene :public BaseScene
{
private://�G�C���A�X
	using CollisionManager = IwasiEngine::CollisionManager;
	using TouchableObject = IwasiEngine::TouchableObject;

public://�����o�֐�
	//�R���X�g���N�^(�X�e�[�W���)
	GamePlayScene(const int stagenum);

	//������
	void Initialize()override;
	//�X�V
	void Update() override;
	//��ԍX�V(�Q�[���J�n��)
	void UpdateIsStartGame();
	//��ԍX�V(�Q�[���v���C��)
	void UpdateIsPlayGame();
	//��ԍX�V(�|�[�Y��ʎ�)
	void UpdateIsPause();
	//��ԍX�V(�X�e�[�W�N���A��)
	void UpdateIsStageClear();
	//��ԍX�V(�Q�[���I�[�o�[��)
	void UpdateIsGameOver();
	//��ԍX�V(�Q�[���I����)
	void UpdateIsQuitGame();
	//�`���[�g���A���X�V
	void UpdateTutorial();

	//�t�F�[�h�A�E�g(�F)
	void FadeIn(const DirectX::XMFLOAT3& color)override;
	//�t�F�[�h�C��(�F)
	void FadeOut(const DirectX::XMFLOAT3& color);
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;

	//���x���f�[�^�ǂݍ���(�X�e�[�W�t�@�C���p�X)
	void LoadLVData(const std::string& stagePath);

	//�F���ς�鏈��(�F)
	void UpdateChangeColor();

public:
	//���@�e�ǉ�(���@�̒e)
	void AddPlayerBullet(const std::unique_ptr<PlayerBullet> playerBullet);
	//�G�e�ǉ�(�G�̒e)
	void AddEnemyBullet(const std::unique_ptr<EnemyBullet> enemyBullet);

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

private:

	//�T�E���h�ǂݍ���
	Audio::SoundData sound_;

	//�X�v���C�g	
	std::unique_ptr<Sprite> spritePause_ = std::make_unique<Sprite>();				//�|�[�Y���̃X�v���C�g
	std::unique_ptr<Sprite> spritePauseInfo_ = std::make_unique<Sprite>();			//�ǂ̃L�[�Ń|�[�Y����̂����������X�v���C�g
	std::unique_ptr<Sprite> spritePauseResume_ = std::make_unique<Sprite>();		//�|�[�Y���ɃQ�[���ĊJ���邩���������X�v���C�g
	std::unique_ptr<Sprite> spritePauseHint_ = std::make_unique<Sprite>();			//�|�[�Y���Ƀq���g���m�F���邩���������X�v���C�g
	std::unique_ptr<Sprite> spriteHintInfo_ = std::make_unique<Sprite>();			//�U���̃q���g���������X�v���C�g
	std::unique_ptr<Sprite> spritePauseStageSelect_ = std::make_unique<Sprite>();	//�|�[�Y���ɃX�e�[�W�Z���N�g�֖߂邩���������X�v���C�g
	std::unique_ptr<Sprite> spritePauseTitle_ = std::make_unique<Sprite>();			//�|�[�Y���Ƀ^�C�g���֖߂邩���������X�v���C�g
	std::unique_ptr<Sprite> spriteDone_ = std::make_unique<Sprite>();				//����\���̃X�v���C�g
	std::unique_ptr<Sprite> spriteReady_ = std::make_unique<Sprite>();				//Ready�\�L�����p�̃X�v���C�g
	std::unique_ptr<Sprite> spriteGo_ = std::make_unique<Sprite>();					//Go�\�L�����p�̃X�v���C�g
	std::unique_ptr<Sprite> spriteFadeInOut_ = std::make_unique<Sprite>();			//�t�F�[�h�C���A�E�g�̃X�v���C�g
	std::unique_ptr<Sprite> spriteLoad_ = std::make_unique<Sprite>();				//���[�h�X�v���C�g
	std::unique_ptr<Sprite> spriteStageInfoNow_ = std::make_unique<Sprite>();		//���݃X�e�[�W�X�v���C�g
	std::unique_ptr<Sprite> spriteCursor_ = std::make_unique<Sprite>();				//�J�[�\���X�v���C�g
	std::unique_ptr<Sprite> spriteHowToPlayList_ = std::make_unique<Sprite>();		//�V�ѕ��������X�g�X�v���C�g
	std::unique_ptr<Sprite> spriteStageName_ = std::make_unique<Sprite>();			//�X�e�[�W���X�v���C�g
	std::unique_ptr<Sprite> spritePauseUI_ = std::make_unique<Sprite>();			//���j���[������@�X�v���C�g

	std::unique_ptr<Sprite> spriteTutorialHTPMove_ = std::make_unique<Sprite>();		//�`���[�g���A���̈ړ����@�X�v���C�g
	std::unique_ptr<Sprite> spriteTutorialHTPDash_ = std::make_unique<Sprite>();		//�`���[�g���A���̃_�b�V�����@�X�v���C�g
	std::unique_ptr<Sprite> spriteTutorialHTPJump_ = std::make_unique<Sprite>();		//�`���[�g���A���̃W�����v���@�X�v���C�g
	std::unique_ptr<Sprite> spriteTutorialHTPMoveBack_ = std::make_unique<Sprite>();	//�`���[�g���A���̎�O�A�����ړ����@�X�v���C�g
	std::unique_ptr<Sprite> spriteTutorialHTPAttack_ = std::make_unique<Sprite>();		//�`���[�g���A���̍U�����@�X�v���C�g

	std::unique_ptr<Sprite> spriteTutorialInfo1_ = std::make_unique<Sprite>();			//�`���[�g���A�����������X�v���C�g(�`���[�g���A��1��)
	std::unique_ptr<Sprite> spriteTutorialInfo2_ = std::make_unique<Sprite>();			//�`���[�g���A�����������X�v���C�g(�`���[�g���A��2��)
	std::unique_ptr<Sprite> spriteTutorialInfo3_ = std::make_unique<Sprite>();			//�`���[�g���A�����������X�v���C�g(�`���[�g���A��3��)
	std::unique_ptr<Sprite> spriteTutorialInfo4_ = std::make_unique<Sprite>();			//�`���[�g���A�����������X�v���C�g(�`���[�g���A��4��)
	

	//�`���[�g���A���p�̃C�[�W���O���
	enum GamePlaySceneSettingTutorialEasingNum
	{
		GPSSTEN_Stundby = 0,
		GPSSTEN_Active = 1,
	};

	//�`���[�g���A�������p�̗񋓑�
	enum TutorialInfoEasingNum
	{
		TIEN_Move = 0,				//�ړ����@
		TIEN_Dash = 1,				//�_�b�V�����@
		TIEN_Jump = 2,				//�W�����v���@
		TIEN_MoveBack = 3,			//��O�A�����ړ����@
		TIEN_Attack = 4,			//�U�����@
		TIEN_Info = 5,				//�Q�[����������
		TIEN_Num = 6,					//�z��p
	};
	//�`���[�g���A��������Y���̒l
	const std::array<float, TIEN_Num> tutorialInfoPosY_ = { 70.0f,120.0f,170.0f,220.0f,270.0f,50.0f };

	//�`���[�g���A���\���̃C�[�W���O
	Easing easeInfoTutorial_[TIEN_Num];

	//�`���[�g���A�����X�g�̃X�P�[���C�[�W���O
	Easing easeTutorialListScale_[XY_Num];

	//�|�[�Y���j���[�p�̗񋓑�
	enum PauseMenuEasingNum
	{
		PMEN_Menu = 0,									//���j���[
		PMEN_Resume = 1,								//�ĊJ
		PMEN_Hint = 2,									//�q���g�m�F
		PMEN_StageSelect = 3,							//�X�e�[�W�Z���N�g��
		PMEN_Title = 4,									//�^�C�g����
		PMEN_SelectSpace = 5,							//�X�y�[�X�őI��
		PMEN_HintInfo = 6,								//�q���g�̓��e
		PMEN_UI = 7,									//������@
		PMEN_Num = 8,									//�z��p
	};
	//�|�[�Y���j���[��Y�l
	const std::array<float, PMEN_Num> pausePosY_ = { 0.0f,120.0f,240.0f,360.0f,480.0f,600.0f,230.0f,400.0f };

	//�|�[�Y���j���[��ʏo���C�[�W���O
	Easing easePauseMenuPosX_[PMEN_Num];

	//�J�[�\��X�l�̃C�[�W���O
	Easing easeCursorPosX_;

	//����p�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeGameStart_[XYZ_Num];
	//����p�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetGameStart_[XYZ_Num];
	//����p�̃v���C���[�|�W�V�����C�[�W���O
	Easing easePlayerPositionGameStart_[XYZ_Num];

	//����p�̃C�[�W���O�̕\�L�͏�������
	enum XXY
	{
		XXY_X1 = 0,	//X(�p�[�g1)
		XXY_X2 = 1,	//X(�p�[�g2)
		XXY_Y = 2,	//Y
		XXY_Num = 3	//�z��p
	};
	
	//����p�̃��f�B�[�\�L�̃C�[�W���O
	Easing easeReadyPosition_[XXY_Num];

	//���f�B�[�C�[�W���O����2���n�܂�t���O
	bool isStartReadyPart2_ = false;
	//���f�B�[�C�[�W���O���I��������̃t���O
	bool isEndReady_ = false;

	//����p�̃S�[�\�L�̃C�[�W���O
	Easing easeGoSizeAndAlpha_[XYW_Num];
	//�S�[�\�L�̍��W�l
	const std::array<float, XY_Num> goPosition_ = { 640.0f,360.0f };

	//���ꎞ�̃C�[�W���O�X�^�[�g�n�_�����߂�ϐ�
	//DirectX::XMFLOAT3 startEaseCameraWorkEye_;			//���_
	//DirectX::XMFLOAT3 startEaseCameraWorkTarget_;		//�����_
	DirectX::XMFLOAT3 startEasePlayerPosition_;			//�v���C���[�|�W�V����

	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_;

	//�|�[�Y�p�̃t�F�[�h�C���A�E�g�C�[�W���O
	Easing easeFadeInOutPause_;

	//�v���C����
	bool isGamePlay_ = false;
	//�Q�[���v���C�V�[�����痣��邩
	bool isQuit_ = false;
	//�|�[�Y������
	bool isPause_ = false;
	//�N���A������
	bool isClear_ = false;
	//�Q�[���I�[�o�[�ɂȂ�����
	bool isGameOver_ = false;
	//�X�^�[�g��
	bool isStart_ = true;
	//�t�F�[�h�C��(�J�ڎ�)
	bool isFadeInScene_ = false;
	//�t�F�[�h�C��(�|�[�Y��)
	bool isFadeInPause_ = false;
	//�t�F�[�h�A�E�g(�|�[�Y��)
	bool isFadeOutPause_ = false;
	//���f��
	//���@
	std::list<std::unique_ptr<Player>> players_;						//���@���X�g
	Player::PlayerModelList modelPlayerList_;							//���@���f���\����
	std::unique_ptr<Model> modelPlayer_ = nullptr;						//���@���f��
	std::unique_ptr<Model> modelPlayerBullet_ = nullptr;				//���@�e���f��
	std::unique_ptr<Model> modelPlayerDash_ = nullptr;					//���@�_�b�V�����f��
	std::unique_ptr<Model> modelPlayerJump_ = nullptr;					//���@�W�����v���f��
	std::unique_ptr<Model> modelPlayerAttack_ = nullptr;				//���@�U�����f��
	std::unique_ptr<Model> modelPlayerHit_ = nullptr;					//���@���S���f��
	//�G
	std::unique_ptr<AbstractEnemyFactory> enemyFactory_;					//�G�̍H��
	std::list<std::unique_ptr<BaseEnemy>> enemys_;							//�G���X�g
	std::unique_ptr<Model> modelEnemy1_ = nullptr;							//�ʏ�G���f��
	std::unique_ptr<Model> modelEnemy2_ = nullptr;							//�h�b�X�����G���f��
	std::unique_ptr<Model> modelEnemyDanger_ = nullptr;						//�댯�ȓG���f��
	std::unique_ptr<Model> modelEnemyBullet_ = nullptr;						//�G�e���f��
	std::unique_ptr<Model> modelBoss1_ = nullptr;							//�X�e�[�W1�̃{�X���f��
	std::unique_ptr<Model> modelBossCore1_ = nullptr;						//�X�e�[�W1�̃{�X�̎���ɂ���R�A�̃��f��

	//�M�~�b�N
	std::unique_ptr<AbstractGimmickFactory> gimmickFactory_;					//�M�~�b�N�̍H��
	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;							//�M�~�b�N���X�g
	std::unique_ptr<Model> modelSpike_ = nullptr;								//�g�Q�̃��f��
	std::unique_ptr<Model> modelBoxUpDown_ = nullptr;							//�㉺���鏰�̃��f��

	//�S�[��
	std::list<std::unique_ptr<Goal>> goals_;					//�S�[�����X�g
	std::unique_ptr<Model> modelGoal_ = nullptr;				//�S�[�����f��

	//�A�C�e��
	std::list<std::unique_ptr<Item>> items_;					//�A�C�e�����X�g
	std::unique_ptr<Model> modelItemJump_ = nullptr;			//�W�����v�͋����A�C�e�����f��
	std::unique_ptr<Model> modelItemHeal_ = nullptr;			//���C�t�񕜃A�C�e�����f��
	std::unique_ptr<Model> modelStageT_ = nullptr;				//�`���[�g���A���X�e�[�W���f��(�V��)
	std::unique_ptr<Model> modelStage1_ = nullptr;				//�X�e�[�W1���f��(�V��)
	std::unique_ptr<Model> modelStage2_ = nullptr;				//�X�e�[�W2���f��(�V��)

	//��
	std::unique_ptr<Model> modelGround_ = nullptr;				//���̃��f��
	std::unique_ptr<Model> modelSphere_ = nullptr;				//�����f��
	std::unique_ptr<Model> modelBox_ = nullptr;					//AABB�����f��

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;

	//�I�u�W�F�N�g�z��
	std::vector<std::unique_ptr<Object3d>> objects_;
	//�V���z��
	std::vector<std::unique_ptr<Object3d>> skydomes_;

	//�p�[�e�B�N��
	std::unique_ptr<Particle> particle1_ = nullptr;

	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pm_ = nullptr;

	//�J����
	std::unique_ptr<Camera> camera_ = nullptr;

	//���C�g
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;

	//�R���W�����}�l�[�W���[
	CollisionManager* colManager_ = nullptr;

	//�`���[�g���A���\���̐F
	DirectX::XMFLOAT3 infoColor_;//xyz=rgb

private:


	/*
	stagenum�̒l
	0~10		�X�e�[�W1
	11~20		�X�e�[�W2
	101~110		�`���[�g���A��
	*/
	//�X�e�[�W�ԍ���
	int stageNum_;
	//���@�e
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//�G�e
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	//���j���[�֌W
	//���j���[�ԍ�
	int menuCount_ = GPSPMI_Resume;
	//���������u�ԂɁ���������h���p�̃t���O
	bool isBack_ = false;
	//�F��ς���X�s�[�h
	float speedColor_ = 0.0f;
	//�F���]�t���O
	bool isColorReverse_ = false;

private:
	//�X�v���C�g�ǂݍ���
	void LoadSprite();
	//���f���ǂݍ���
	void LoadModel();
	/*
	�`���[�g���A���p�̃C�[�W���O(�X�v���C�g1�`6��)
	num=0 �X�^���o�C
	num=1 �C�[�W���O��
	*/
	void SettingTutorialEase(const int num, Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)const;

	//�`���[�g���A���X�v���C�g�̍X�V
	void UpdateTutorialSprite();

	//�`���[�g���A���p�̃X�v���C�g�`��(�X�v���C�g1�`6��)
	void DrawTutorialSprite(const Sprite* s1, const Sprite* s2,
		const Sprite* s3, const Sprite* s4, const Sprite* s5, const Sprite* s6)const;

	//�X�e�[�W���̃X�v���C�g�����[�h
	void LoadStageNameSprite();
	//�`���[�g���A�����̃X�v���C�g�����[�h
	void LoadTutorialSprite();
	//�ۉe�Z�b�g
	void SetUpCircleShadow(const DirectX::XMFLOAT3& pos);
};