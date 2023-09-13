#pragma once
#include "Audio.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"

#include "ParticleManager.h"
#include "Sprite.h"

#include "Player.h"
#include "Earth.h"

#include <vector>
#include <map>
#include <sstream>
#include <string>

#include "Easing.h"
#include "SceneManager.h"
#include <DirectXMath.h>
#include <chrono>

//json���x���f�[�^
struct LevelData;

//�^�C�g���V�[��
class TitleScene :public BaseScene
{
private:
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;
public://�萔
	//int MAX_TIME = 120;

public://�����o�֐�
	//������
	void Initialize() override;
	//�X�V
	void Update() override;
	//�`��
	void Draw() override;
	//�I��
	void Finalize() override;

	const XMFLOAT3 Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t);

	//���x���f�[�^�ǂݍ���
	void LoadLVData(const std::string& stagePath);

private://�ÓI�����o�ϐ�
	//DirectX���
	static DirectXCommon* dxCommon_;
	//�X�v���C�g���
	SpriteCommon* spCommon_=nullptr;
	//�C���v�b�g
	static Input* input_;
	//�I�[�f�B�I
	Audio* audio_ = nullptr;
	//�V�[���}�l�[�W���[
	static SceneManager* sceneManager_;
	//imgui
	static ImGuiManager* imguiManager_;
	//�J����
	static Camera* camera_;

private://�����o�ϐ�
	//��
	Audio::SoundData titleBGM;
	Audio::SoundData doneSE;
	Audio::SoundData startSE;

	//Sprite
	Sprite* spriteTitle_ = new Sprite();
	Sprite* spriteHTP_ = new Sprite();
	//���f��
	std::list<std::unique_ptr<Player>> players_;
	Model* modelPlayer_ = nullptr;
	Model* modelPlayerBullet_ = nullptr;

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

	std::list<std::unique_ptr<Earth>> earths_;
	Model* modelEarth_ = nullptr;

	LevelData* levelData = nullptr;

	Model* modelSkydome = nullptr;
	Model* modelGround = nullptr;
	Model* modelBox = nullptr;
	Model* modelRail = nullptr;
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;

	//���@�e
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;

	//���Ԍv��
	std::chrono::steady_clock::time_point startCount;	//�J�n����
	std::chrono::steady_clock::time_point nowCount;		//���ݎ���
	std::chrono::microseconds elapsedCount;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����
	float	maxTime = 2.0f;					//�S�̎���
	float	timeRate;
	//����_(���_)
	XMFLOAT3 startEye;
	XMFLOAT3 p1Eye;
	XMFLOAT3 p2Eye;
	XMFLOAT3 endEye;
	//����_(�����_)
	XMFLOAT3 startTarget;
	XMFLOAT3 p1Target;
	XMFLOAT3 p2Target;
	XMFLOAT3 endTarget;

	bool isStart = false;
	bool isHTP = false;
	Easing easeTitlePosY = Easing(0.0f, 800.0f, 1.0f);

	Easing easeHTPPosY = Easing(800.0f, 0.0f, 1.0f);
	//���C�g
	LightGroup* lightGroup_ = nullptr;
	//�p�[�e�B�N��
	Particle* particle1_ = nullptr;
	ParticleManager* pm1_ = nullptr;
	//���f���ǂݍ���
	void LoadModel();
};
