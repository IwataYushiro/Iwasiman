#pragma once
#include "WinApp.h"
#include "AbstractSceneFactory.h"
#include "Audio.h"
#include "Camera.h"
#include "DirectXCommon.h"
#include "FbxLoader.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "SceneManager.h"
#include "SpriteCommon.h"
#include <memory>

namespace IwasiEngine
{
	/*

	*	Framework.h

	*	�Q�[���S�̃t���[�����[�N

	*/
	class Framework
	{
	public://�����o�֐�
		virtual ~Framework() = default;
		//������
		virtual void Initialize();
		//�X�V
		virtual void Update();
		//�`��
		virtual void Draw() = 0;
		//�I������
		virtual void Finalize();
		//�I���t���O�̎�t
		virtual bool IsEndRequest() { return EndGame_; }

		//���s
		void Run();

	protected:
		//WinApp
		WinApp* winApp_ = nullptr;
		//DXCommon
		DirectXCommon* dxCommon_ = nullptr;
		//SpriteCommon
		SpriteCommon* sprCommon_ = nullptr;
		//Audio
		Audio* audio_ = nullptr;
		//Input
		Input* input_ = nullptr;
		//�V�[��
		SceneManager* sceneManager_ = nullptr;
		//�V�[���t�@�N�g���[
		std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
		//imgui
		ImGuiManager* imguiManager_ = nullptr;

		//�Q�[���I���t���O
		bool EndGame_ = false;
	};
}