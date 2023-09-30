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

//�Q�[���v���C
class GamePlayScene :public BaseScene
{
public://�\���̗�
	GamePlayScene(int stagenum);
	enum Scene { //�V�[��ID
		tutorial,
		stage1,
		stage2,
	};

public:

	//������
	void Initialize()override;
	//�X�V
	void Update() override;
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;

	//���x���f�[�^�ǂݍ���
	void LoadLVData(const std::string& stagePath);

public:
	//���@�e�ǉ�
	void AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet);
	//�G�e�ǉ�
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
	//�J����
	static Camera* camera_;
private:

	//�T�E���h�ǂݍ���
	Audio::SoundData sound;

	//�X�v���C�g
	Sprite* spritePause_ = new Sprite();
	Sprite* spritePauseInfo_ = new Sprite();

	Sprite* spriteTutorialHTPMove = new Sprite();
	Sprite* spriteTutorialHTPDash = new Sprite();
	Sprite* spriteTutorialHTPJump = new Sprite();

	Sprite* spriteTutorialHTPMoveBack = new Sprite();

	Sprite* spriteTutorialHTPAttack = new Sprite();


	Sprite* spriteTutorialInfo1 = new Sprite();
	Sprite* spriteTutorialInfo2 = new Sprite();
	Sprite* spriteTutorialInfo3 = new Sprite();
	Sprite* spriteTutorialInfo4 = new Sprite();

	Easing easeInfo[6] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//���j���[
		Easing(1300.0f, 500.0f, 1.2f),//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 500.0f, 1.6f),//�X�y�[�X�őI��
		Easing(1300.0f, 800.0f, 1.8f) ,
		Easing(1300.0f, 0.0f, 2.0f),
	};

	//�|�[�Y������
	bool isPause_ = false;
	bool isclear = false;
	bool isGameover = false;

	//���f��
	std::list<std::unique_ptr<Player>> players_;
	Model* modelPlayer_ = nullptr;
	Model* modelPlayerBullet_ = nullptr;

	std::unique_ptr<AbstractEnemyFactory> enemyFactory;

	std::list<std::unique_ptr<BaseEnemy>> enemys_;
	Model* modelEnemy1_ = nullptr;
	Model* modelEnemyBullet_ = nullptr;

	Model* modelBoss1_ = nullptr;
	Model* modelBossCore1_ = nullptr;

	std::unique_ptr<AbstractGimmickFactory> gimmickFactory;

	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;
	Model* modelSpike_ = nullptr;

	std::list<std::unique_ptr<Goal>> goals_;
	Model* modelGoal_ = nullptr;

	std::list<std::unique_ptr<Item>> items_;
	Model* modelItemJump_ = nullptr;
	Model* modelItemHeal_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelStageT = nullptr;
	Model* modelStage1 = nullptr;
	Model* modelStage2 = nullptr;

	Model* modelGround = nullptr;
	Model* modelBox = nullptr;

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm_ = nullptr;

	Particle* particle2_ = nullptr;

	//���C�g
	LightGroup* lightGroup_ = nullptr;

	//�Փ˃}�l�[�W��
	CollisionManager* colManager_ = nullptr;

private:
	/*
	stagenum�̒l
	1~9		�X�e�[�W1
	10~19	�X�e�[�W2
	100~109	�`���[�g���A��
	*/
	int stageNum;
	//���@�e
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//�G�e
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//�C�[�W���O�}�l�[�W���[(������E��)
	Easing es = Easing(-(float)WinApp::GetInstance()->window_width, 0.0f, 1.0f);
	//���������u�ԂɁ���������h���p�̃t���O
	bool isBack = false;
	//�F��ς���X�s�[�h
	float speedColor = 0.0f;
	//�F���]�t���O
	bool isColorReverse_ = false;
private:
	//�X�v���C�g�ǂݍ���
	void LoadSprite();
	//���f���ǂݍ���
	void LoadModel();
	/*
	�`���[�g���A���p�̃C�[�W���O
	num=0 �X�^���o�C
	num=1 �C�[�W���O��
	*/
	void SettingTutorialEase(int num, Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);
	void UpdateTutorialSprite();
	//�`���[�g���A���p�̃X�v���C�g�`��
	void DrawTutorialSprite(Sprite* s1, Sprite* s2,
		Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6);
};