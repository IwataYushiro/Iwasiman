#pragma once
#include "Audio.h"
#include "DirectionalLight.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"

#include <map>

#include "SceneManager.h"

//json���x���f�[�^
struct LevelData;

//�^�C�g���V�[��
class TitleScene :public BaseScene
{
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

private://�����o�ϐ�
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	//FBX
	ModelFbx* modelF = nullptr;
	ObjectFbx* objF = nullptr;

	//���f��
	Model* modelPlayer_ = nullptr;
	Object3d* object3DPlayer_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;
	Model* modelGround = nullptr;
	Model* modelFighter = nullptr;
	Model* modelSphere = nullptr;

	Object3d* objSkydome = nullptr;
	Object3d* objGround = nullptr;
	Object3d* objFighter = nullptr;
	Object3d* objSphere = nullptr;
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	//���C�g
	DirectionalLight* light_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
};
