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
	TitleScene(int stagenum);
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
	
	int menuCount_;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteTitleDone_ = new Sprite();
	Sprite* spriteMenu_ = new Sprite();
	Sprite* spriteMenuTutorial_ = new Sprite();
	Sprite* spriteMenuStageSelect_ = new Sprite();
	Sprite* spriteMenuDone_ = new Sprite();
	Sprite* spriteBack_ = new Sprite();

	//FBX
	//ModelFbx* modelF = nullptr;
	//ObjectFbx* objF = nullptr;

	LevelData* levelData = nullptr;

	//���f��
	Model* modelPlayer_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelSkydomeStage1_ = nullptr;
	Model* modelSkydomeStage2_ = nullptr;
	Model* modelGround_ = nullptr;

	std::vector<Object3d*> objPlayers_;
	std::vector<Object3d*> objSkydomes_;
	std::vector<Object3d*> objGrounds_;

	std::map<std::string, Model*> models_;

	bool isStart_ = false;
	bool isMenu_ = false;
	bool isBack_ = false;
	Easing easeTitlePosX_[2] =
	{
		Easing(0.0f, -1300.0f, 1.0f),
		Easing(0.0f, -1300.0f, 1.0f)
	};
		

	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//���j���[
		Easing(1300.0f, 0.0f, 1.2f),//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),//�X�y�[�X�őI��
		Easing(1300.0f, 900.0f, 1.8f),//�߂�
	};

	Easing easeEye_[3]
	{
		Easing(0.0f, 21.0f, 1.8f),//X
		Easing(1.0f, -4.0f, 1.8f),//Y
		Easing(-110.0f, -60.0f, 1.8f),//Z
	};
	Easing easeTarget_[3]
	{
		Easing(0.0f, -100.0f, 1.8f),//X
		Easing(0.0f, -10.0f, 1.8f),//Y
		Easing(-10.0f, -62.0f, 1.8f),//Z
	};
	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb

	const DirectX::XMFLOAT4 otherMenuColor_ = { 0.0f,0.0f,0.0f,1.0f };
	const DirectX::XMFLOAT4 backTitleColor_ = { 0.0f,0.0f,0.1f,1.0f };

	//�F���]�t���O
	bool isColorReverse_ = false;
	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;

	//�J�n���̃|�W�V����
	DirectX::XMFLOAT3 startPos_;
	//X�l�������܂ŗ����烋�[�v
	const float returnPos_ = -120.0f;
	
	//�ǂ̃X�e�[�W�ɂ���̂����󂯎�邽�߂̕ϐ�
	int stageNum_;

public:
	//�F���ς�鏈��
	void UpdateChangeColor();

};
