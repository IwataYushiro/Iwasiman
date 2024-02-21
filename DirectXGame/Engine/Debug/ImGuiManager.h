#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include <wrl.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	ImGuiManager.h

	*	ImGui�Ǘ�

	*/
	class ImGuiManager final
	{
	private://�G�C���A�X
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public://�V���O���g���C���X�^���X
		static ImGuiManager* GetInstance();

	public://�����o�֐�

		//������(Windows�A�v���P�[�V�����ADirectX���)
		void Initialize();

		//�X�V
		void Update();
		//��t�J�n
		void Begin();
		//��t�I��
		void End();

		//ImGui�̃T���v���֐��i�X�^�C���`�F���W�j
		void ImGuiStyleShowSample();

		//ImGui�̃T���v���֐��iMyFirstTool�j
		void ImGuiMyFirstToolColor();

		//�`��
		void Draw();

		//�I��
		void Finalize();

	private:
		//WinApp
		const WinApp* winApp_ = nullptr;
		//DXCommon
		const DirectXCommon* dxCommon_ = nullptr;
		//SRV�p�̃f�X�N���v�^�q�[�v
		ComPtr<ID3D12DescriptorHeap> srvHeap_;
	private:
		//�R���X�g���N�^�i�V���O���g���p�^�[���j
		ImGuiManager() = default;
		//�f�X�g���N�^�i�V���O���g���p�^�[���j
		~ImGuiManager() = default;
	public:
		//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
		ImGuiManager(const ImGuiManager& obj) = delete;
		// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
		ImGuiManager& operator=(const ImGuiManager& obj) = delete;
	};
}