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

/*

*	TitleScene.h

*	�^�C�g���V�[��

*/
class TitleScene :public BaseScene
{
public://�����o�֐�(�X�e�[�W�ԍ�)
	TitleScene(int stagenum);
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//��ԍX�V(�Q�[���J�n����Ƃ�)
	void UpdateIsStartGame();
	//��ԍX�V(�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ�)
	void UpdateIsStageSelect();
	//��ԍX�V(�X�^�[�g�ɖ߂����Ƃ�)
	void UpdateIsBack();
	//��ԍX�V(���j���[�̂Ƃ�) 
	void UpdateIsMenu();
	//�t�F�[�h�A�E�g(�F)
	void FadeOut(DirectX::XMFLOAT3 rgb);
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;

	//���x���f�[�^�ǂݍ���
	void LoadLVData(const std::string& stagePath);

private://�ÓI�����o�ϐ�
	//DirectX���
	static DirectXCommon* dxCommon_;
	
	//�C���v�b�g
	static Input* input_;
	//�I�[�f�B�I
	static Audio* audio_;
	//�V�[���}�l�[�W���[
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;
	

private://�����o�ϐ�

	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;
	//�J����
	Camera* camera_ = nullptr;

	//�I�����Ă��郁�j���[�\��
	int menuCount_;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();				//�^�C�g����ʃX�v���C�g
	Sprite* spriteTitleDone_ = new Sprite();			//�^�C�g������\���X�v���C�g
	Sprite* spriteMenu_ = new Sprite();					//�^�C�g�����j���[��ʃX�v���C�g
	Sprite* spriteMenuTutorial_ = new Sprite();			//�`���[�g���A���\���X�v���C�g
	Sprite* spriteMenuStageSelect_ = new Sprite();		//�X�e�[�W�Z���N�g�\���X�v���C�g
	Sprite* spriteMenuDone_ = new Sprite();				//�^�C�g�����j���[����\���X�v���C�g
	Sprite* spriteBack_ = new Sprite();					//�^�C�g�����j���[���^�C�g���ڍs�̃X�v���C�g
	Sprite* spriteFadeInOut_ = new Sprite();			//�t�F�[�h�C���A�E�g�X�v���C�g

	//json���x���f�[�^
	LevelData* levelData_ = nullptr;

	//���f��
	Model* modelPlayer_ = nullptr;						//���@���f��
	Model* modelSkydome_ = nullptr;						//�`���[�g���A���X�e�[�W���f��(�V��)
	Model* modelSkydomeStage1_ = nullptr;				//�X�e�[�W1���f��(�V��)
	Model* modelSkydomeStage2_ = nullptr;				//�X�e�[�W2���f��(�V��)
	Model* modelGround_ = nullptr;						//�����f��
	Model* modelGoal_ = nullptr;						//�S�[�����f��

	std::vector<Object3d*> objPlayers_;					//���@�I�u�W�F�N�g�z��
	std::vector<Object3d*> objSkydomes_;				//�V���I�u�W�F�N�g�z��
	std::vector<Object3d*> objGrounds_;					//���I�u�W�F�N�g�z��
	std::vector<Object3d*> objGoals_;					//�S�[���I�u�W�F�N�g�z��

	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;

	//�t���O��
	bool isMenu_ = false;				//�^�C�g�����j���[��ʂɂ���Ƃ�
	bool isBack_ = false;				//�^�C�g�����j���[����^�C�g���ɖ߂�Ƃ�
	bool isStartGame_ = false;			//�Q�[���J�n����Ƃ�
	bool isStageSelect_ = false;		//�X�e�[�W�Z���N�g�ɍs���Ƃ�
	bool isFadeOut_ = false;			//�t�F�[�h�A�E�g

	//�^�C�g�����^�C�g�����j���[�̃C�[�W���O
	Easing easeTitlePosX_[2] =
	{
		Easing(225.0f, -1300.0f, 1.0f),
		Easing(300.0f, -1300.0f, 1.0f)
	};

	//�^�C�g�����j���[�̏o���C�[�W���O
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),		//���j���[
		Easing(1300.0f, 0.0f, 1.2f),		//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),		//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),		//�X�y�[�X�őI��
		Easing(1300.0f, 900.0f, 1.8f),		//�߂�
	};

	//�^�C�g�����^�C�g�����j���[�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeMenu_[3]
	{
		Easing(0.0f, 21.0f, 1.8f),			//X
		Easing(1.0f, -4.0f, 1.8f),			//Y
		Easing(-110.0f, -60.0f, 1.8f),		//Z
	};
	//�^�C�g�����^�C�g�����j���[�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetMenu_[3]
	{
		Easing(0.0f, -100.0f, 1.8f),		//X
		Easing(0.0f, -10.0f, 1.8f),			//Y
		Easing(-10.0f, -62.0f, 1.8f),		//Z
	};

	//�^�C�g�����j���[���Q�[���J�n�̎��_�J�������[�N�C�[�W���O
	Easing easeEyeGameStart_[3]
	{
		Easing(21.0f, -22.0f, 1.0f),		//X
		Easing(-4.0f, -1.0f, 1.0f),			//Y
		Easing(-60.0f, -60.0f, 1.0f),		//Z
	};
	//�^�C�g�����j���[���Q�[���J�n�̒����_�J�������[�N�C�[�W���O
	Easing easeTargetGameStart_[3]
	{
		Easing(-100.0f, 50.0f, 1.0f),		//X
		Easing(-10.0f, -8.0f, 1.0f),		//Y
		Easing(-62.0f, -57.0f, 1.0f),		//Z
	};
	//�^�C�g�����j���[���X�e�[�W�Z���N�g�̎��@�ړ��C�[�W���O
	Easing easePlayerMove_[3]
	{
		Easing(0.0f, 150.0f, 2.0f),			//X
		Easing(-8.0f, 40.0f, 2.0f),			//Y
		Easing(-60.0f, -60.0f, 2.0f),		//Z
	};

	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb
	//�I������Ă��Ȃ��F
	const DirectX::XMFLOAT4 otherMenuColor_ = { 0.0f,0.0f,0.0f,1.0f };
	//�^�C�g�����j���[���^�C�g���֖߂�X�v���C�g�̐F
	const DirectX::XMFLOAT4 backTitleColor_ = { 0.0f,0.0f,0.1f,1.0f };

	//�F���]�t���O
	bool isColorReverse_ = false;
	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pm1_ = nullptr;

	//�J�n���̃|�W�V����
	DirectX::XMFLOAT3 startPos_;
	
	//�ǂ̃X�e�[�W�ɂ���̂����󂯎�邽�߂̕ϐ�
	int stageNum_;

public:
	//�F���ς�鏈��
	void UpdateChangeColor();

};
