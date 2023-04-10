#pragma once
#include "Framework.h"
#include "Audio.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"

#include "Player.h"
#include "Enemy.h"

class MyGame :public Framework
{
public:

	enum Scene { //�V�[��ID
		title,
		howtoplay,
		stage,
		clear,
		gameover,
	};

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//�I��
	void Finalize() override;

	//�����蔻��
	void ChackAllCollisions();

private:
	
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteHowToPlay_ = new Sprite();
	Sprite* spriteGameClear_ = new Sprite();
	Sprite* spriteGameOver_ = new Sprite();
	//Audio
	Audio* audio_ = new Audio();

	//imgui
	ImGuiManager* imguiManager_ = new ImGuiManager();

	//�T�E���h�ǂݍ���
	Audio::SoundData sound;

	//���f��
	Player* player_ = nullptr;
	Model* modelPlayer_ = nullptr;
	
	Enemy* enemy_ = nullptr;
	Model* modelEnemy_ = nullptr;
	
	//3D�I�u�W�F�N�g
	Object3d* object3DPlayer_ = nullptr;
	Object3d* object3DEnemy_ = nullptr;
	
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager * pm1_ = nullptr;

	Particle* particle2_ = nullptr;
	ParticleManager* pm2_ = nullptr;
	//�V�[��
	size_t scene_;

	//�Q�[���I���t���O
	bool EndGame_ = false;

};

