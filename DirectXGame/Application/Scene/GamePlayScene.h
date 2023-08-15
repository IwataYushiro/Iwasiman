#pragma once
#include "Audio.h"
#include "Camera.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "Enemy.h"
#include "Goal.h"
#include "Player.h"

#include "SceneManager.h"
#include "CollisionPrimitive.h"
#include "Easing.h"


#include "ItemJump.h"
#include "ItemHeal.h"

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
	enum Scene { //�V�[��ID
		title,
		howtoplay,
		stage,
		clear,
		gameover,
	};

public:
	//������
	void Initialize() override;
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

	Sprite* spritePause_ = new Sprite();
	Sprite* spriteClear_ = new Sprite();
	Sprite* spritePauseInfo_ = new Sprite();
	Sprite* spriteGameover_ = new Sprite();
	
	Sprite* spriteItemJumpBar_ = new Sprite();
	//�|�[�Y������
	bool isPause_ = false;
	bool isclear = false;
	bool isGameover = false;

	//���f��
	std::list<std::unique_ptr<Player>> players_;
	Model* modelPlayer_ = nullptr;

	std::list<std::unique_ptr<Enemy>> enemys_;
	Model* modelEnemy_ = nullptr;

	std::list<std::unique_ptr<Goal>> goals_;
	Model* modelGoal_ = nullptr;

	std::list<std::unique_ptr<ItemJump>> jItems_;
	Model* modelItemJump_ = nullptr;

	std::list<std::unique_ptr<ItemHeal>> hItems_;
	Model* modelItemHeal_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;
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
	//���@�e
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	//�G�e
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//�C�[�W���O�}�l�[�W���[(������E��)
	Easing es = Easing(-(float)WinApp::GetInstance()->window_width, 0.0f, 1.0f);
	//���������u�ԂɁ���������h���p�̃t���O
	bool isBack = false;

private:
	//�X�v���C�g�ǂݍ���
	void LoadSprite();
	//���f���ǂݍ���
	void LoadModel();
};