#pragma once
#include "WinApp.h"
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

//ImGuiŠÇ—
class ImGuiManager
{
public:
	
	//‰Šú‰»
	void Initialize(WinApp* winApp);
	//XV
	void Update();
	//I—¹
	void Finalize();

private:
	//WinApp
	WinApp* winApp_ = nullptr;
};
