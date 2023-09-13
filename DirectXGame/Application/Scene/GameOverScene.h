#pragma once
#include "Audio.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"
#include "Earth.h"

#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <DirectXMath.h>

#include "SceneManager.h"

//json���x���f�[�^
struct LevelData;

//�^�C�g���V�[��
class GameOverScene :public BaseScene
{
private:
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;

public://�����o�֐�
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

private://�����o�ϐ�
	Audio::SoundData BGM;
	Audio::SoundData doneSE;
	int stageNum;
	//Sprite
	Sprite* spriteGameOver_ = new Sprite();

	std::list<std::unique_ptr<Earth>> earths_;
	Model* modelEarth_ = nullptr;
	
	Model* modelEnemy1_ = nullptr;
	Model* modelEnemy1Power_ = nullptr;
	Model* modelEnemy1Guard_ = nullptr;
	Model* modelEnemy1Speed_ = nullptr;
	Model* modelEnemy1Death_ = nullptr;
	Model* modelEnemyBullet_ = nullptr;

	Model* modelEnemy2_ = nullptr;
	Model* modelEnemy2Power_ = nullptr;
	Model* modelEnemy2Guard_ = nullptr;
	Model* modelEnemy2Speed_ = nullptr;
	Model* modelEnemy2Death_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//���f���ǂݍ���
	void LoadModel();
};


