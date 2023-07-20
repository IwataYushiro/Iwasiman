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
#include "SceneManager.h"
#include "CollisionPrimitive.h"


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

private://�ÓI�����o�ϐ�
	//DirectX���
	static DirectXCommon* dxCommon_;
	//�X�v���C�g���
	static SpriteCommon* spCommon_;
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

	//���f��
	Player* player_ = nullptr;
	Model* modelPlayer_ = nullptr;

	Enemy* enemy_ = nullptr;
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
	ParticleManager* pm1_ = nullptr;

	Particle* particle2_ = nullptr;
	ParticleManager* pm2_ = nullptr;
	//���C�g
	LightGroup* lightGroup_ = nullptr;

	//�Փ˃}�l�[�W��
	CollisionManager* colManager_ = nullptr;

};