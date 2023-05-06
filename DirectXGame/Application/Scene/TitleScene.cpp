#include "TitleScene.h"
#include "FbxLoader.h"

DirectXCommon* TitleScene::dxCommon_ = DirectXCommon::GetInstance();
SpriteCommon* TitleScene::spCommon_ = SpriteCommon::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
Audio* TitleScene::audio_ = Audio::GetInstance();
SceneManager* TitleScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* TitleScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* TitleScene::camera_ = Camera::GetInstance();


void TitleScene::Initialize()
{
	//オーディオ
	audio_->Initialize();

	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture/title.png");
	spriteTitle_->Initialize(spCommon_, titleTex);

	FbxLoader::GetInstance()->LoadModelFromFile("spheref");
	//デバイスセット
	ObjectFbx::SetDevice(dxCommon_->GetDevice());
	//カメラセット
	ObjectFbx::SetCamera(camera_);
	//グラフィックスパイプライン生成
	ObjectFbx::CreateGraphicsPipeline();
}

void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GAMEPLAY");
	}
	spriteTitle_->Update();
}

void TitleScene::Draw()
{
	//背景スプライト描画前処理
	spCommon_->PreDraw();
	//スプライト描画
	spriteTitle_->Draw();


	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//エフェクト描画後処理
	ParticleManager::PostDraw();


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//モデル描画後処理
	Object3d::PostDraw();

	//前景スプライト

	//ImGuiの表示
	imguiManager_->Draw();
}

void TitleScene::Finalize()
{
	//スプライト
	delete spriteTitle_;
}
