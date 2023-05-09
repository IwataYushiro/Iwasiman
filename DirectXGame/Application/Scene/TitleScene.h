#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"

#include "ParticleManager.h"
#include "Sprite.h"

#include "SceneManager.h"

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

	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
};
