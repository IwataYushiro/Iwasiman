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

//�X�e�[�W�N���A�V�[��
class StageClearScene :public BaseScene
{
public://�����o�֐�
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
	//�X�e�[�W�ԍ�
	int stageNum_;
	//���j���[�ԍ�
	int menuCount_ = 0;
	//Sprite
	Sprite* spriteStageClear_ = new Sprite();
	Sprite* spriteNextStage_ = new Sprite();
	Sprite* spriteStageSelect_ = new Sprite();
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteDone_ = new Sprite();
	Sprite* spriteFadeInOut_ = new Sprite();

	LevelData* levelData_ = nullptr;

	//���f��
	Model* modelPlayer_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelSkydomeStage1_ = nullptr;
	Model* modelSkydomeStage2_ = nullptr;
	Model* modelGround_ = nullptr;
	Model* modelGoal_ = nullptr;

	std::vector<Object3d*> objPlayers_;
	std::vector<Object3d*> objSkydomes_;
	std::vector<Object3d*> objGrounds_;
	std::vector<Object3d*> objGoals_;

	std::map<std::string, Model*> models_;
	//�t���O��
	bool isFinalStage_ = false;//�ŏI�ʂ̏ꍇ
	bool isNextStage_ = false;
	bool isStageSelect_ = false;
	bool isQuitTitle_ = false;
	bool isFadeOut_ = false;

	//�C�[�W���O��
	Easing easeMenuPosX_[5] =
	{
		Easing(1300.0f, 0.0f, 1.0f),//���j���[
		Easing(1300.0f, 0.0f, 1.2f),//�`���[�g���A����
		Easing(1300.0f, 0.0f, 1.4f),//�X�e�[�W�Z���N�g��
		Easing(1300.0f, 0.0f, 1.6f),//�^�C�g����
		Easing(1300.0f, 0.0f, 1.8f),//�X�y�[�X�őI��
	};
	Easing easeEyeGameStart_[3]
	{
		Easing(0.0f, -22.0f, 1.8f),//X
		Easing(1.0f, -1.0f, 1.8f),//Y
		Easing(-110.0f, -60.0f, 1.8f),//Z
	};
	Easing easeTargetGameStart_[3]
	{
		Easing(0.0f, 50.0f, 1.8f),//X
		Easing(0.0f, -8.0f, 1.8f),//Y
		Easing(-10.0f, -57.0f, 1.8f),//Z
	};

	Easing easePlayerMoveStageSelect_[3]
	{
		Easing(0.0f, 150.0f, 2.0f),//X
		Easing(-8.0f, 40.0f, 2.0f),//Y
		Easing(-60.0f, -60.0f, 2.0f),//Z
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
	ParticleManager* pm1_ = nullptr;

	//�J�n���̃|�W�V����
	DirectX::XMFLOAT3 startPos_;

public:
	//�F���ς�鏈��
	void UpdateChangeColor();

};
