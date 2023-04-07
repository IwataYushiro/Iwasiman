#include "ImGuiManager.h"
#include <cassert>

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon)
{
	HRESULT result;

	this->winApp_ = winApp;
	this->dxCommon_ = dxCommon;

	//ImGui�̃R���e�L�X�g�𐶐�
	ImGui::CreateContext();
	//ImGui�̃X�^�C�����w��
	ImGui::StyleColorsDark();
	
	//�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//����
	result = dxCommon_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	//Win32�̏����֐�
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	//DX12�̏����֐�
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(),
		static_cast<int>(dxCommon_->GetBuckBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);
	//�W���t�H���g�̒ǉ�
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

}

void ImGuiManager::Update()
{

	//ImGui�Ăяo��
	Begin();
	//��������ImGui�̕\�����ڂ�ǉ�����
	ImGuiStyleShowSample();
	//�\�����ڂ����܂�
	End();

}

void ImGuiManager::Begin()
{
	//imgui�t���[���J�n
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	//�`��O����
	ImGui::Render();
}
void ImGuiManager::ImGuiStyleShowSample() {
	static char buf[50] = {};
	static float f = 0.0f;

	ImGui::Text("Hello, world %d", 184);

	if (ImGui::Button("Style Classic")) { ImGui::StyleColorsClassic(); }
	if (ImGui::Button("Style Light")) { ImGui::StyleColorsLight(); }
	if (ImGui::Button("Style Dack")) { ImGui::StyleColorsDark(); }

	ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
}

void ImGuiManager::ImGuiMyFirstToolColor()
{
	// Create a window called "My First Tool", with a menu bar.
	static bool my_tool_active = true;
	static float my_color[4] = { 0.0f,0.0f,0.0f,0.0f };

	ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Edit a color stored as 4 floats
	ImGui::ColorEdit4("Color", my_color);

	// Generate samples and plot them
	float samples[100];
	for (int n = 0; n < 100; n++)
		samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
	ImGui::PlotLines("Samples", samples, 100);

	// Display contents in a scrolling region
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	ImGui::BeginChild("Scrolling");
	for (int n = 0; n < 50; n++)
		ImGui::Text("%04d: Some text", n);
	ImGui::EndChild();
	ImGui::End();
}
void ImGuiManager::Draw()
{
	//�R�}���h���X�g
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//�f�X�N���v�^�q�[�v�̔z����Z�b�g����R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�`��R�}���h�̎��s
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiManager::Finalize()
{
	//�㏈��
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//�f�X�N���v�^�q�[�v�J��
	srvHeap_.Reset();
}
