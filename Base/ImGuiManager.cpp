#include "ImGuiManager.h"

void ImGuiManager::Initialize(WinApp* winApp)
{
	this->winApp_ = winApp;
	//ImGuiのコンテキストを生成
	ImGui::CreateContext();
	//ImGuiのスタイルを指定
	ImGui::StyleColorsDark();
	//Win32の初期関数
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
}

void ImGuiManager::Update()
{
}

void ImGuiManager::Finalize()
{
}
