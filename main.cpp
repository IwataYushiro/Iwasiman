#include "WinApp.h"
#include "Audio.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Object3d.h"
#include "SpriteCommon.h"
#include "Sprite.h"

using namespace DirectX;


void ImGuiStyleShowSample() {
	static char buf[50] = {};
	static float f = 0.0f;

	ImGui::Text("Hello, world %d", 184);

	if (ImGui::Button("Style Classic")) { ImGui::StyleColorsClassic(); }
	if (ImGui::Button("Style Light")) { ImGui::StyleColorsLight(); }
	if (ImGui::Button("Style Dack")) { ImGui::StyleColorsDark(); }

	ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
}

void ImGuiMyFirstToolColor()
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
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#pragma region ポインタ置き場
	WinApp* winApp = new WinApp();

	DirectXCommon* dxCommon = new DirectXCommon();

	SpriteCommon* sprCommon = new SpriteCommon();

	Sprite* sprite = new Sprite();

	Audio* audio = new Audio();

	Input* input = new Input();

	ImGuiManager* imguiManager = new ImGuiManager();

#pragma endregion

#pragma region Windows初期化
	winApp->Initialize();
#pragma endregion
	// DirectX初期化処理　ここから

	//DirectX初期化
	dxCommon->Initialize(winApp);

	//スプライト基盤
	sprCommon->Initialize(dxCommon);
	//入力
	input->Initialize(winApp);
	//imgui
	imguiManager->Initialize(winApp, dxCommon);

	// DirectX初期化処理　ここまで

	//一旦ここでimguiテスト
	
	// 描画初期化処理　ここから
#pragma region 描画初期化処理

	//音声データ
	audio->Initialize();

	Audio::SoundData sound = audio->SoundLordWave("Resources/TestMusic.wav");
	//音声再生呼び出し例
	audio->SoundPlayWave(audio->GetXAudio2(), sound);

	//ここでテクスチャを指定しよう
	UINT texindex = 00;
	sprCommon->LoadTexture(texindex, "texture.png");
	sprite->Initialize(sprCommon, texindex);

	//3Dオブジェクト関係
	Object3d::StaticInitialize(dxCommon->GetDevice(), winApp->window_width, winApp->window_height);

	//OBJファイルからモデルデータを読み込む
	Model* model = Model::LoadFromOBJ("triangle_mat");
	//3Dオブジェクト生成
	Object3d* object3d = Object3d::Create();
	//オブジェクトにモデル紐付ける
	object3d->SetModel(model);

#pragma endregion
	// 描画初期化処理　ここまで

	//ゲームループ
	while (true)
	{
		//Windowsのメッセージ処理
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		// DirectX毎フレーム処理　ここから

		// 更新処理ここから
		input->Update();
		//スプライト呼び出し例
		sprite->Update();

		//モデル呼び出し例
		object3d->Update();

		//ImGui呼び出し
		imguiManager->Begin();
		//ここからImGuiの表示項目を追加する
		ImGuiMyFirstToolColor();
		//表示項目ここまで
		imguiManager->End();

		// ここまで

		//描画処理ここから
		//描画前処理
		dxCommon->PreDraw();

		//背景スプライト
		//スプライト描画前処理
		sprCommon->PreDraw();
		//スプライト描画
		sprite->Draw();

		//モデル
		//モデル描画前処理
		object3d->PreDraw(dxCommon->GetCommandList());

		//モデル描画
		object3d->Draw();

		//モデル描画後処理
		object3d->PostDraw();

		//前景スプライト

		//ImGuiの表示
		imguiManager->Draw();
		//描画後処理
		dxCommon->PostDraw();
		// 描画処理ここまで

		// DirectX毎フレーム処理　ここまで

	}
	//終了処理
	imguiManager->Finalize();
	audio->Finalize();
	winApp->Finalize();

	//解放
	//各種音声
	audio->SoundUnLoad(&sound);
	//音声全体
	delete audio;

	//スプライト
	delete sprite;

	//モデル
	//3Dオブジェクト
	delete object3d;
	//3Dモデル
	delete model;

	//基盤系
	delete imguiManager;
	delete sprCommon;
	delete input;
	delete dxCommon;
	delete winApp;
	//コンソールへの文字出力
	OutputDebugStringA("Hello DilectX!!\n");

	return 0;
}

