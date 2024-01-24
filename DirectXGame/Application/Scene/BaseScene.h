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

	//�C�[�W���O�̃Z�b�g�R�}���h
	std::stringstream setEasingCommands_;

private://�����o�ϐ�
	//�V�[���}�l�[�W���[(�؂�Ă���̂ł�����delete�̓_���[�b�^�C)
	SceneManager* sceneManager_ = nullptr;

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
	void LoadEasingData(const std::string& fileName,Easing& ease)
	{
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
		setEasingCommands_ << file.rdbuf();

		//�t�@�C�������
		file.close();

		//1�s���̕����������֐�
		std::string line;
		//�R�}���h���s���[�v
		while (getline(setEasingCommands_, line))
		{
			//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
			std::istringstream line_stream(line);

			std::string word;
			//,��؂�ōs�̐퓬��������擾
			getline(line_stream, word, ',');

			// "//"=�R�����g
			if (word.find("//") == 0)continue;//�R�����g�s���΂�
			//�C�[�W���O�̃Z�b�g
			if (word.find("SET") == 0)
			{
				//x���W
				getline(line_stream, word, ',');
				float start = (float)std::atof(word.c_str());
				//y���W
				getline(line_stream, word, ',');
				float end = (float)std::atof(word.c_str());
				//z���W
				getline(line_stream, word, ',');
				float time = (float)std::atof(word.c_str());
				//�C�[�W���O�Z�b�g
				ease.SetEasing(start, end, time);
			}
		}
	}
};
