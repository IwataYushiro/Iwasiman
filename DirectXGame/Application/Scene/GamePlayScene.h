#pragma once
#include "Audio.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "ParticleManager.h"

#include "Player.h"
#include "Enemy.h"

#include "SceneManager.h"

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
	//�����蔻��
	void ChackAllCollisions();

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

	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteHowToPlay_ = new Sprite();
	Sprite* spriteGameClear_ = new Sprite();
	Sprite* spriteGameOver_ = new Sprite();

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
	ParticleManager* pm1_ = nullptr;

	Particle* particle2_ = nullptr;
	ParticleManager* pm2_ = nullptr;
	//���C�g
	DirectionalLight* light_ = nullptr;

};