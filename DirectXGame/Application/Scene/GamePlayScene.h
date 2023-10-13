#pragma once
#include "Audio.h"
#include "Camera.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"

#include "Goal.h"
#include "Player.h"
#include "EnemyFactory.h"
#include "GimmickFactory.h"

#include "SceneManager.h"
#include "CollisionPrimitive.h"
#include "Easing.h"

#include "Item.h"

#include <vector>
#include <map>
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
public://�����o�֐�
	//�R���X�g���N�^(�X�e�[�W���)
	GamePlayScene(int stagenum);

	//������
	void Initialize()override;
	//�X�V
	void Update() override;
	//��ԍX�V(�Q�[���v���C��)
	void UpdateIsPlayGame();
	//��ԍX�V(�|�[�Y��ʎ�)
	void UpdateIsPause();
	//��ԍX�V(�X�e�[�W�N���A��)
	void UpdateIsStageClear();
	//�`���[�g���A���X�V
	void UpdateTutorial();
	
	//�t�F�[�h�A�E�g(�F)
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//�t�F�[�h�C��(�F)
	void FadeIn(DirectX::XMFLOAT3 rgb);
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
	void AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet);
	//�G�e�ǉ�(�G�̒e)
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);

private://�ÓI�����o�ϐ�

	//DirectX���
	static DirectXCommon* dxCommon_;
	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;
	//�C���v�b�g
	static Input* input_;
	//�I�[�f�B�I
	static Audio* audio_;
	//�V�[���}�l�[�W���[
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;

private:

	//�T�E���h�ǂݍ���
	Audio::SoundData sound_;

	//�X�v���C�g	
	Sprite* spritePause_ = new Sprite();				//�|�[�Y���̃X�v���C�g
	Sprite* spritePauseInfo_ = new Sprite();			//�ǂ̃L�[�Ń|�[�Y����̂����������X�v���C�g
	Sprite* spritePauseResume_ = new Sprite();			//�|�[�Y���ɃQ�[���ĊJ���邩���������X�v���C�g
	Sprite* spritePauseHowToPlay_ = new Sprite();		//�|�[�Y���ɗV�ѕ����m�F���邩���������X�v���C�g
	Sprite* spritePauseStageSelect_ = new Sprite();		//�|�[�Y���ɃX�e�[�W�Z���N�g�֖߂邩���������X�v���C�g
	Sprite* spritePauseTitle_ = new Sprite();			//�|�[�Y���Ƀ^�C�g���֖߂邩���������X�v���C�g
	Sprite* spriteDone_ = new Sprite();					//����\���̃X�v���C�g
	Sprite* spriteFadeInOut_ = new Sprite();			//�t�F�[�h�C���A�E�g�̃X�v���C�g


	Sprite* spriteTutorialHTPMove_ = new Sprite();		//�`���[�g���A���̈ړ����@�X�v���C�g
	Sprite* spriteTutorialHTPDash_ = new Sprite();		//�`���[�g���A���̃_�b�V�����@�X�v���C�g
	Sprite* spriteTutorialHTPJump_ = new Sprite();		//�`���[�g���A���̃W�����v���@�X�v���C�g
	Sprite* spriteTutorialHTPMoveBack_ = new Sprite();	//�`���[�g���A���̎�O�A�����ړ����@�X�v���C�g
	Sprite* spriteTutorialHTPAttack_ = new Sprite();	//�`���[�g���A���̍U�����@�X�v���C�g

	Sprite* spriteTutorialInfo1_ = new Sprite();		//�`���[�g���A�����������X�v���C�g(�`���[�g���A��1��)
	Sprite* spriteTutorialInfo2_ = new Sprite();		//�`���[�g���A�����������X�v���C�g(�`���[�g���A��2��)
	Sprite* spriteTutorialInfo3_ = new Sprite();		//�`���[�g���A�����������X�v���C�g(�`���[�g���A��3��)
	Sprite* spriteTutorialInfo4_ = new Sprite();		//�`���[�g���A�����������X�v���C�g(�`���[�g���A��4��)

	//�`���[�g���A���\���̃C�[�W���O
	Easing easeInfoTutorial_[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),	//�ړ����@
		Easing(1300.0f, 500.0f, 1.2f),	//�_�b�V�����@
		Easing(1300.0f, 0.0f, 1.4f),	//�W�����v���@
		Easing(1300.0f, 500.0f, 1.6f),	//��O�A�����ړ����@
		Easing(1300.0f, 800.0f, 1.8f),	//�U�����@
		Easing(1300.0f, 0.0f, 2.0f),	//�Q�[����������
	};

	//�|�[�Y���j���[��ʏo���C�[�W���O
	Easing easePauseMenuPosX_[6] =
	{
		Easing(1300.0f, 100.0f, 0.5f),			//���j���[
		Easing(1300.0f, 50.0f, 0.6f),			//�ĊJ
		Easing(1300.0f, 50.0f, 0.7f),			//�V�ѕ��m�F
		Easing(1300.0f, 50.0f, 0.8f),			//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 50.0f, 0.9f),			//�^�C�g����
		Easing(1300.0f, 0.0f, 1.0f),			//�X�y�[�X�őI��
	};

	//�V�ѕ�������ʏo���C�[�W���O
	Easing easeHowToPlayPosX_[6] =
	{
		Easing(1300.0f, 0.0f, 0.5f),			//���j���[
		Easing(1300.0f, 0.0f, 0.6f),			//�ĊJ
		Easing(1300.0f, 0.0f, 0.7f),			//�V�ѕ��m�F
		Easing(1300.0f, 0.0f, 0.8f),			//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 0.9f),			//�^�C�g����
		Easing(1300.0f, 0.0f, 1.0f),			//�X�y�[�X�őI��
	};

	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 0.5f);
	//�|�[�Y�p�̃t�F�[�h�C���A�E�g�C�[�W���O
	Easing easeFadeInOutPause_ = Easing(0.8f, 0.0f, 0.5f);

	//�v���C����
	bool isGamePlay_ = true;
	//�V�ѕ�������ʎ���
	bool isHowToPlay_ = false;
	//�Q�[���v���C�V�[�����痣��邩
	bool isQuit_ = false;
	//�|�[�Y������
	bool isPause_ = false;
	//�N���A������
	bool isClear_ = false;
	//�Q�[���I�[�o�[�ɂȂ�����
	bool isGameover_ = false;
	//�t�F�[�h�A�E�g(�J�ڎ�)
	bool isFadeOutScene_ = false;
	//�t�F�[�h�A�E�g(�|�[�Y��)
	bool isFadeOutPause_ = false;
	//�t�F�[�h�C��(�|�[�Y��)
	bool isFadeInPause_ = false;
	//���f��
	//���@
	std::list<std::unique_ptr<Player>> players_;		//���@���X�g
	Model* modelPlayer_ = nullptr;						//���@���f��
	Model* modelPlayerBullet_ = nullptr;				//���@�e���f��

	//�G
	std::unique_ptr<AbstractEnemyFactory> enemyFactory_;	//�G�̍H��
	std::list<std::unique_ptr<BaseEnemy>> enemys_;			//�G���X�g
	Model* modelEnemy1_ = nullptr;							//�G���f��
	Model* modelEnemyBullet_ = nullptr;						//�G�e���f��
	Model* modelBoss1_ = nullptr;							//�X�e�[�W1�̃{�X���f��
	Model* modelBossCore1_ = nullptr;						//�X�e�[�W1�̃{�X�̎���ɂ���R�A�̃��f��

	//�M�~�b�N
	std::unique_ptr<AbstractGimmickFactory> gimmickFactory_;	//�M�~�b�N�̍H��
	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;			//�M�~�b�N���X�g
	Model* modelSpike_ = nullptr;								//�g�Q�̃��f��

	//�S�[��
	std::list<std::unique_ptr<Goal>> goals_;	//�S�[�����X�g
	Model* modelGoal_ = nullptr;				//�S�[�����f��

	//�A�C�e��
	std::list<std::unique_ptr<Item>> items_;	//�A�C�e�����X�g
	Model* modelItemJump_ = nullptr;			//�W�����v�͋����A�C�e�����f��
	Model* modelItemHeal_ = nullptr;			//���C�t�񕜃A�C�e�����f��
	Model* modelStageT_ = nullptr;				//�`���[�g���A���X�e�[�W���f��(�V��)
	Model* modelStage1_ = nullptr;				//�X�e�[�W1���f��(�V��)
	Model* modelStage2_ = nullptr;				//�X�e�[�W2���f��(�V��)

	//��
	Model* modelGround_ = nullptr;				//���̃��f��
	Model* modelBox_ = nullptr;					//AABB�����f��

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;

	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;
	//�I�u�W�F�N�g�z��
	std::vector<Object3d*> objects_;
	//�V���z��
	std::vector<Object3d*> skydomes_;

	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;

	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pm_ = nullptr;

	//�J����
	Camera* camera_ = nullptr;

	//���C�g
	LightGroup* lightGroup_ = nullptr;

	//�R���W�����}�l�[�W���[
	CollisionManager* colManager_ = nullptr;

	//�`���[�g���A���\���̐F
	DirectX::XMFLOAT3 infoColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb

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
	int menuCount_ = 0;
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
	void SettingTutorialEase(int num, Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);

	//�`���[�g���A���X�v���C�g�̍X�V
	void UpdateTutorialSprite();

	//�`���[�g���A���p�̃X�v���C�g�`��(�X�v���C�g1�`6��)
	void DrawTutorialSprite(Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);
};