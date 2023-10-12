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
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;

	//���x���f�[�^�ǂݍ���(�X�e�[�W�t�@�C���p�X)
	void LoadLVData(const std::string& stagePath);

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

	//�|�[�Y������
	bool isPause_ = false;
	//�N���A������
	bool isclear_ = false;
	//�Q�[���I�[�o�[�ɂȂ�����
	bool isGameover_ = false;

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