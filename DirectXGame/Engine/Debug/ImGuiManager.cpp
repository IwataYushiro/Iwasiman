#include "ImGuiManager.h"
#include <cassert>

using namespace IwasiEngine;
/*

*	ImGuiManager.cpp

*	ImGui管理

*/

ImGuiManager* ImGuiManager::GetInstance()
{
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(const WinApp* winApp, const DirectXCommon* dxCommon)
{
	HRESULT result;

	this->winApp_ = winApp;
	this->dxCommon_ = dxCommon;

	//ImGuiのコンテキストを生成
	ImGui::CreateContext();
	//ImGuiのスタイルを指定
	ImGui::StyleColorsDark();
	
	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//生成
	result = dxCommon_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	//Win32の初期関数
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	//DX12の初期関数
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(),
		static_cast<int>(dxCommon_->GetBuckBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);
	//標準フォントの追加
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

}

void ImGuiManager::Update()
{

	//ImGui呼び出し
	Begin();
	//ここからImGuiの表示項目を追加する
	//ImGuiStyleShowSample();
	//表示項目ここまで
	End();

}

void ImGuiManager::Begin()
{
	//imguiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	//描画前準備
	ImGui::Render();
}
void ImGuiManager::ImGuiStyleShowSample() {
	const int32_t maxBuff = 50;
	static char buf[maxBuff] = {};
	const float defaultFloatNum = 0.0f;
	static float f = defaultFloatNum;

	const int32_t defaultIntTextNum = 184;
	ImGui::Text("Hello, world %d", defaultIntTextNum);

	if (ImGui::Button("Style Classic")) { ImGui::StyleColorsClassic(); }
	if (ImGui::Button("Style Light")) { ImGui::StyleColorsLight(); }
	if (ImGui::Button("Style Dack")) { ImGui::StyleColorsDark(); }

	ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
	enum MinMaxNum
	{
		MMN_Min=0,
		MMN_Max=1,
		MMN_Num=2,
	};

	const float minMax[MMN_Num] = { 0.0f,1.0f };
	ImGui::SliderFloat("float", &f, minMax[MMN_Min], minMax[MMN_Max]);
}

void ImGuiManager::ImGuiMyFirstToolColor()
{
	// Create a window called "My First Tool", with a menu bar.
	enum ColorPatternIndex
	{
		CPI_R=0,
		CPI_G = 1,
		CPI_B = 2,
		CPI_A = 3,
		CPI_Num = 4,
	};
	static bool my_tool_active = true;
	const float defaultColor[CPI_Num] = { 0.0f,0.0f,0.0f,0.0f };
	static float my_color[CPI_Num] = { defaultColor[CPI_R],defaultColor[CPI_G] ,
		defaultColor[CPI_B],defaultColor[CPI_A] };

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
	const int32_t samplesNum = 100;
	const float roopMaxNum = 100.0f;
	float samples[samplesNum];
	for (float n = 0.0f; n < roopMaxNum; n++)
	{
		const float sampleCalculation = sinf(n * 0.2f + (float)ImGui::GetTime() * 1.5f);
		samples[(int)n] = sampleCalculation;
	}
		
	ImGui::PlotLines("Samples", samples, samplesNum);

	// Display contents in a scrolling region
	const ImVec4 defaultTextColor = { 1.0f,1.0f,1.0f,1.0f };
	const int32_t roopText = 50;
	ImGui::TextColored(defaultTextColor, "Important Stuff");
	ImGui::BeginChild("Scrolling");
	for (int n = 0; n < roopText; n++)
		ImGui::Text("%04d: Some text", n);
	ImGui::EndChild();
	ImGui::End();
}
void ImGuiManager::Draw()
{
	//コマンドリスト
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドの実行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiManager::Finalize()
{
	//後処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//デスクリプタヒープ開放
	srvHeap_.Reset();
}
