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

*	StageClearScene.h

*	�X�e�[�W�N���A�V�[��

*/
class StageClearScene :public BaseScene
{
public://�����o�֐�
	//�R���X�g���N�^(�X�e�[�W�ԍ�)
	StageClearScene(int stagenum);
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//��ԍX�V(�Q�[���J�n����Ƃ�)
	void UpdateIsNextStage();
	//��ԍX�V(�X�e�[�W�Z���N�g�֑J�ڂ���Ƃ�)
	void UpdateIsStageSelect();
	//��ԍX�V(�^�C�g���֖߂�Ƃ�)
	void UpdateIsQuitTitle();
	//��ԍX�V(���j���[�̂Ƃ�) 
	void UpdateIsMenu();
	//�`��
	void Draw() override;
	//�t�F�[�h�A�E�g
	void FadeOut(DirectX::XMFLOAT3 rgb);
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
	
	//�X�e�[�W�ԍ�
	int stageNum_;
	//���j���[�ԍ�
	int menuCount_ = 0;
	//Sprite
	Sprite* spriteStageClear_ = new Sprite();	//�X�e�[�W�N���A���̃X�v���C�g
	Sprite* spriteNextStage_ = new Sprite();	//���̃X�e�[�W�\���̃X�v���C�g
	Sprite* spriteStageSelect_ = new Sprite();	//�X�e�[�W�Z���N�g�\���̃X�v���C�g
	Sprite* spriteTitle_ = new Sprite();		//�^�C�g���\���̃X�v���C�g
	Sprite* spriteDone_ = new Sprite();			//����\���̃X�v���C�g
	Sprite* spriteFadeInOut_ = new Sprite();	//�t�F�[�h�C���A�E�g�̃X�v���C�g
	
	//json���x���f�[�^
	LevelData* levelData_ = nullptr;			

	//���f��
	Model* modelPlayer_ = nullptr;				//���@���f��
	Model* modelSkydome_ = nullptr;				//�`���[�g���A���X�e�[�W���f��(�V��)
	Model* modelSkydomeStage1_ = nullptr;		//�X�e�[�W1���f��(�V��)
	Model* modelSkydomeStage2_ = nullptr;		//�X�e�[�W2���f��(�V��)
	Model* modelGround_ = nullptr;				//���̃��f��
	Model* modelGoal_ = nullptr;				//�S�[�����f��

	std::vector<Object3d*> objPlayers_;			//���@�z��
	std::vector<Object3d*> objSkydomes_;		//�V���z��
	std::vector<Object3d*> objGrounds_;			//���z��
	std::vector<Object3d*> objGoals_;			//�S�[���z��

	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;

	//�t���O��
	bool isFinalStage_ = false;					//�ŏI�ʂ̏ꍇ
	bool isNextStage_ = false;					//���̃X�e�[�W�֍s���ꍇ
	bool isStageSelect_ = false;				//�X�e�[�W�Z���N�g�֍s���ꍇ
	bool isQuitTitle_ = false;					//�^�C�g���ɖ߂�ꍇ
	bool isFadeOut_ = false;					//�t�F�[�h�C���A�E�g

	//�C�[�W���O��
	//���j���[��ʏo���C�[�W���O
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),			//���j���[
		Easing(1300.0f, 0.0f, 1.2f),			//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),			//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),			//�^�C�g����
		Easing(1300.0f, 0.0f, 1.8f),			//�X�y�[�X�őI��
	};
	//���̃X�e�[�W�֍s���Ƃ��̎��_�C�[�W���O
	Easing easeEyeStageClear_[3]
	{
		Easing(0.0f, -22.0f, 1.8f),				//X
		Easing(1.0f, -1.0f, 1.8f),				//Y
		Easing(-110.0f, -60.0f, 1.8f),			//Z
	};
	//���̃X�e�[�W�֍s���Ƃ��̒����_�C�[�W���O
	Easing easeTargetStageClear_[3]
	{
		Easing(0.0f, 50.0f, 1.8f),				//X
		Easing(0.0f, -8.0f, 1.8f),				//Y
		Easing(-10.0f, -57.0f, 1.8f),			//Z
	};
	//�X�e�[�W�Z���N�g�֍s���Ƃ��̎��@�ړ��C�[�W���O
	Easing easePlayerMoveStageSelect_[3]
	{
		Easing(0.0f, 150.0f, 2.0f),				//X
		Easing(-8.0f, 40.0f, 2.0f),				//Y
		Easing(-60.0f, -60.0f, 2.0f),			//Z
	};
	//�t�F�[�h�C���A�E�g(false �t�F�[�h�C���Atrue �t�F�[�h�A�E�g)
	Easing easeFadeInOut_ = Easing(1.0f, 0.0f, 1.0f);

	//�I�𒆂̐F
	DirectX::XMFLOAT3 selectColor_ = { 0.0f,0.0f,0.0f };//xyz=rgb

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

public:
	//�F���ς�鏈��
	void UpdateChangeColor();

};
