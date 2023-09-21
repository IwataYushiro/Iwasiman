#pragma once
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
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
	SpriteCommon* spCommon_=nullptr;
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

	Sprite* spriteMenu_ = new Sprite();
	Sprite* spriteMenuTutorial_ = new Sprite();
	Sprite* spriteMenuStageSelect_ = new Sprite();
	Sprite* spriteMenuDone_ = new Sprite();

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

	bool isStart = false;
	bool isMenu = false;
	Easing easeTitlePosX = Easing(0.0f, -1300.0f, 1.0f);

	Easing easeMenuPosX[4] = 
	{
		Easing(1300.0f, 0.0f, 1.0f),//���j���[
		Easing(1300.0f, 0.0f, 1.2f),//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),//�X�y�[�X�őI��
	};
	int MenuCount = 0;
	
	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
};
