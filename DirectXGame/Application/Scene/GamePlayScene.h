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

//�Q�[���v���C�V�[��

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
	Audio::SoundData sound_;

	//�X�v���C�g
	Sprite* spritePause_ = new Sprite();
	Sprite* spritePauseInfo_ = new Sprite();

	Sprite* spriteTutorialHTPMove_ = new Sprite();
	Sprite* spriteTutorialHTPDash_ = new Sprite();
	Sprite* spriteTutorialHTPJump_ = new Sprite();

	Sprite* spriteTutorialHTPMoveBack_ = new Sprite();

	Sprite* spriteTutorialHTPAttack_ = new Sprite();


	Sprite* spriteTutorialInfo1_ = new Sprite();
	Sprite* spriteTutorialInfo2_ = new Sprite();
	Sprite* spriteTutorialInfo3_ = new Sprite();
	Sprite* spriteTutorialInfo4_ = new Sprite();

	Easing easeInfo_[6] =
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
	bool isclear_ = false;
	bool isGameover_ = false;

	//���f��
	std::list<std::unique_ptr<Player>> players_;
	Model* modelPlayer_ = nullptr;
	Model* modelPlayerBullet_ = nullptr;

	std::unique_ptr<AbstractEnemyFactory> enemyFactory_;

	std::list<std::unique_ptr<BaseEnemy>> enemys_;
	Model* modelEnemy1_ = nullptr;
	Model* modelEnemyBullet_ = nullptr;

	Model* modelBoss1_ = nullptr;
	Model* modelBossCore1_ = nullptr;

	std::unique_ptr<AbstractGimmickFactory> gimmickFactory_;

	std::list<std::unique_ptr<BaseGimmick>> gimmicks_;
	Model* modelSpike_ = nullptr;

	std::list<std::unique_ptr<Goal>> goals_;
	Model* modelGoal_ = nullptr;

	std::list<std::unique_ptr<Item>> items_;
	Model* modelItemJump_ = nullptr;
	Model* modelItemHeal_ = nullptr;

	LevelData* levelData_ = nullptr;

	Model* modelStageT_ = nullptr;
	Model* modelStage1_ = nullptr;
	Model* modelStage2_ = nullptr;

	Model* modelGround_ = nullptr;
	Model* modelBox_ = nullptr;

	std::map<std::string, Model*> models_;
	std::vector<Object3d*> objects_;
	std::vector<Object3d*> skydomes_;

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
	0~10		�X�e�[�W1
	11~20		�X�e�[�W2
	101~110		�`���[�g���A��
	*/

	int stageNum_;
	//���@�e
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//�G�e
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//�|�[�Y�̃C�[�W���O(������E��)
	Easing easePause_ = Easing(-(float)WinApp::GetInstance()->window_width, 0.0f, 1.0f);
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