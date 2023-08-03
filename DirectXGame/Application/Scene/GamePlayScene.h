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
#include <vector>
#include <map>
#include <sstream>
#include <chrono>
#include "SceneManager.h"
#include "CollisionPrimitive.h"
#include "EasingManager.h"

//json���x���f�[�^
struct LevelData;
//�O���錾
class Player;
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
	void LoadLVData();

	//�G�����f�[�^�̓ǂݍ���
	void LoadEnemyPopData();
	//�G�����R�}���h�̍X�V
	void UpdateEnemyPopCommands();

public:
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
	
	//�|�[�Y������
	bool isPause_ = false;
	bool isclear = false;
	bool isGameover = false;

	//���f��
	Player* player_ = nullptr;
	Model* modelPlayer_ = nullptr;

	std::list<std::unique_ptr<Enemy>> enemys_;
	Model* modelEnemy_ = nullptr;

	Goal* goal_ = nullptr;
	Model* modelGoal_ = nullptr;

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
	//�G�e
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//�G�����R�}���h
	std::stringstream enemyPopCommands;

	EasingManager es = { -(float)WinApp::GetInstance()->window_width,0.0f,1.0f };
	

private:
	//�X�v���C�g�ǂݍ���
	void LoadSprite();
};