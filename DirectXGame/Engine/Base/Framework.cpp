#include "Framework.h"
#include "DirectionalLight.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "ParticleManager.h"

void Framework::Initialize()
{
	//WinApp
	winApp_ = WinApp::GetInstance();
	//DXCommon
	dxCommon_ = DirectXCommon::GetInstance();
	//SpriteCommon
	sprCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_ = Audio::GetInstance();
	//Input
	input_ = Input::GetInstance();
	//SceneManager
	sceneManager_ = SceneManager::GetInstance();
	//imgui
	imguiManager_ = ImGuiManager::GetInstance();
	//カメラ
	camera_ = Camera::GetInstance();
	
	//WinApp初期化
	winApp_->Initialize();
	//DirectX初期化
	dxCommon_->Initialize(winApp_);
	//スプライト基盤
	sprCommon_->Initialize(dxCommon_);
	//入力
	input_->Initialize(winApp_);
	//imgui
	imguiManager_->Initialize(winApp_, dxCommon_);
	//FBX
	FbxLoader::GetInstance()->Initialize(dxCommon_->GetDevice());
	
	Object3d::StaticInitialize(dxCommon_->GetDevice());
	ObjectFbx::StaticInitialize(dxCommon_->GetDevice());
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());
	//ライト
	DirectionalLight::StaticInitialize(dxCommon_->GetDevice());
	
	

}

void Framework::Update()
{
	//Windowsのメッセージ処理
	if (winApp_->ProcessMessage())
	{
		//ゲーム終了
		EndGame_ = true;
	}
	//ポストエフェクト
	//postEffect_->Update();

	//入力の更新
	input_->Update();
	sceneManager_->Update();
	imguiManager_->Begin();
#ifdef _DEBUG
	camera_->DebugCamera();
	
#endif // DEBUG
ImGui::Begin("Post Loop");
	ImGui::SetWindowPos(ImVec2(0, 600));
	ImGui::SetWindowSize(ImVec2(800, 100));
	ImGui::Text("0 change");
	ImGui::Text(" test -> gaussianblur -> bloom -> Glare -> sepia -> cold -> ");
	ImGui::Text(" nagapozi -> grayscale -> mosaic -> uv shift -> uv shift blur -> posteffect off");

	ImGui::End();
	imguiManager_->End();
	
}

void Framework::Finalize()
{
	//scene
	sceneManager_->Finalize();
	delete sceneFactory_;
	//FBX
	FbxLoader::GetInstance()->Finalize();
	//imgui
	imguiManager_->Finalize();
	//WinApp
	winApp_->Finalize();
}


void Framework::Run()
{
	//初期化
	Initialize();

	//ゲームループ
	while (true)
	{
		Update();

		//終了リクエストが来たらループを抜ける
		if (IsEndRequest())
		{
			//ゲームループを抜ける
			break;
		}
		//描画処理ここから
		Draw();
		// 描画処理ここまで

		// DirectX毎フレーム処理　ここまで

	}
	//終了処理
	Finalize();
}