#pragma once
#include <DirectXMath.h>
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "LevelLoaderJson.h"
#include "Sprite.h"
#include "ParticleManager.h"

#include <map>
#include <memory>
#include <sstream>
#include <fstream>
//�O���錾
//�V�[���}�l�[�W���[
class SceneManager;

/*

*	BaseScene.h (cpp X)

*	�V�[�����

*/
class BaseScene
{
protected://�G�C���A�X
	using DirectXCommon = IwasiEngine::DirectXCommon;
	using Input = IwasiEngine::Input;
	using Audio = IwasiEngine::Audio;
	using ImGuiManager = IwasiEngine::ImGuiManager;
	using Sprite = IwasiEngine::Sprite;
	using SpriteCommon = IwasiEngine::SpriteCommon;
	using Model = IwasiEngine::Model;
	using Object3d = IwasiEngine::Object3d;
	using Camera = IwasiEngine::Camera;
	using Easing = IwasiEngine::Easing;
	using LevelData = IwasiEngine::LevelData;
	using LightGroup = IwasiEngine::LightGroup;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;

public://�����o�֐�
	virtual~BaseScene() = default;

	//������
	virtual void Initialize() = 0;
	//�X�V
	virtual void Update() = 0;
	//�`��
	virtual void Draw() = 0;
	//�I������
	virtual void Finalize() = 0;
	//�t�F�[�h�C���A�E�g
	virtual void FadeIn(const DirectX::XMFLOAT3& color) = 0;
	//�C�[�W���O�̃��[�h
	virtual void LoadEasing() = 0;

protected://�p�������o�ϐ�
	//���݃X�e�[�W�̃|�W�V����
	const DirectX::XMFLOAT2 stageInfoNowPos_ = { 300.0f,100.0f };
	//���[�f�B���O���̃|�W�V����
	const DirectX::XMFLOAT2 loadPos_ = { 350.0f,600.0f };
	//�X�e�[�W���̃|�W�V����
	const DirectX::XMFLOAT2 stageNamePos_ = { 630.0f,500.0f };
	//�F�T���v��
	const DirectX::XMFLOAT3 white_ = { 1.0f,1.0f,1.0f };	//��
	const DirectX::XMFLOAT3 black_ = { 0.0f,0.0f,0.0f };	//��
	const DirectX::XMFLOAT3 deepRed_ = { 0.5f,0.2f,0.2f };	//�Z����
	//�A���J�[�|�C���g�̃v���Z�b�g
	const DirectX::XMFLOAT2 ANCHOR_POINT_CENTRAL = { 0.5f,0.5f };//����

	//�}�b�s���O���f��
	std::map<std::string, Model*> models_;
	//���o�X�L�b�v�p�J�E���g
	bool skip_ = false;

private://�����o�ϐ�
	//�V�[���}�l�[�W���[(�؂�Ă���̂ł�����delete�̓_���[�b�^�C)
	SceneManager* sceneManager_ = nullptr;
	//�C�[�W���O�̔z��p
	enum EasingSetNumM
	{
		//0�`9��10�܂őΉ�
		ESN_0,
		ESN_1,
		ESN_2,
		ESN_3,
		ESN_4,
		ESN_5,
		ESN_6,
		ESN_7,
		ESN_8,
		ESN_9,
		ESN_MAX,
	};

public://�A�N�Z�b�T�u����
	virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager_ = sceneManager; }

protected://�p�������o�֐�
	//���f���ǂݍ��݂ƃ}�b�s���O�̋��ʉ�
	void ModelMapping(std::unique_ptr<Model>& model, const std::string& modelName, bool smoothing = false)
	{
		//���f����ǂݍ���
		model = Model::LoadFromOBJ(modelName, smoothing);				//���@���f��
		//�}�b�v�ɓo�^
		models_.insert(std::make_pair(modelName, model.get()));
	}
	//�X�y�[�X�������Ɖ��o���X�L�b�v����֐�
	void SkipDirectionOnSpace(const DirectX::XMFLOAT3& color)
	{
		//�X�y�[�X�������ƃX�L�b�v����悤�ɂ���
		if (Input::GetInstance()->TriggerKey(DIK_SPACE))skip_ = true;
		//���o�X�L�b�v
		if (skip_)FadeIn(color);
	}

	//�C�[�W���O�f�[�^�̓ǂݍ���
	void LoadEasingData(const std::string& fileName, Easing& ease, const uint32_t easeArrayNum = 0)
	{
		//�X�g���[���̏�����
		std::stringstream setEasingCommands;
		setEasingCommands.str("");//�o�b�t�@�̃N���A
		setEasingCommands.clear(std::stringstream::goodbit);//��ԃN���A

		//�f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
		const std::string defaultEasingPath = "Resources/csv/Easing/";
		std::string fullPath = defaultEasingPath + fileName;

		//���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
		int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

		//���C�h������ɕϊ�
		std::vector<wchar_t> wfilePath(filePathBufferSize);
		MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

		//�t�@�C�����J��
		std::ifstream file;
		file.open(wfilePath.data());
		assert(file.is_open());

		//�t�@�C���̓��e�𕶎���X�g���[���ɃR�s�[
		setEasingCommands << file.rdbuf();

		//�t�@�C�������
		file.close();

		//SETNum�̃o�b�t�@
		const char* set = SetNumStr(easeArrayNum);

		//1�s���̕����������֐�
		std::string line;
		//�R�}���h���s���[�v
		while (getline(setEasingCommands, line))
		{
			//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
			std::istringstream line_stream(line);

			std::string word;
			//,��؂�ōs�̐퓬��������擾
			getline(line_stream, word, ',');

			// "//"=�R�����g
			if (word.find("//") == 0)continue;//�R�����g�s���΂�
			//�ő�10��ނ̃C�[�W���O�ɑΉ�
			for (uint32_t i = 0; i < ESN_MAX; i++)SetEasingCommand(set, i, easeArrayNum, ease, line_stream, word);
		}
	}
	//�C�[�W���O�̃R�}���h����l�̃Z�b�g
	void SetEasingCommand(const char* findWord, const uint32_t num, const uint32_t easeArrayNum, Easing& ease, std::istream& stream, std::string& word)
	{
		//�C�[�W���O�̃Z�b�g
		if (word.find(findWord) == 0 && num == easeArrayNum)
		{
			//x���W
			getline(stream, word, ',');
			float start = static_cast<float>(std::atof(word.c_str()));
			//y���W
			getline(stream, word, ',');
			float end = static_cast<float>(std::atof(word.c_str()));
			//z���W
			getline(stream, word, ',');
			float time = static_cast<float>(std::atof(word.c_str()));
			//�C�[�W���O�Z�b�g
			ease.SetEasing(start, end, time);
		}
	}
	//SET�̕��������߂�
	const char* SetNumStr(const uint32_t easeArrayNum)
	{
		//�z��ԍ��ɂ���ĕԂ�������͈Ⴄ
		if (easeArrayNum == ESN_0)return "SET0";
		else if (easeArrayNum == ESN_1)return "SET1";
		else if (easeArrayNum == ESN_2)return "SET2";
		else if (easeArrayNum == ESN_3)return "SET3";
		else if (easeArrayNum == ESN_4)return "SET4";
		else if (easeArrayNum == ESN_5)return "SET5";
		else if (easeArrayNum == ESN_6)return "SET6";
		else if (easeArrayNum == ESN_7)return "SET7";
		else if (easeArrayNum == ESN_8)return "SET8";
		else if (easeArrayNum == ESN_9)return "SET9";
		else return nullptr;
	}
};

