#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include <wrl.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

//ImGui�Ǘ�
class ImGuiManager
{
private://�G�C���A�X
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	
	//������
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	
	//�X�V
	void Update();
	//��t�J�n
	void Begin();
	//��t�I��
	void End();
	
	void ImGuiStyleShowSample();

	void ImGuiMyFirstToolColor();
	
	//�`��
	void Draw();
	
	//�I��
	void Finalize();

private:
	//WinApp
	WinApp* winApp_ = nullptr;
	//DXCommon
	DirectXCommon* dxCommon_ = nullptr;
	//SRV�p�̃f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> srvHeap_;
};
