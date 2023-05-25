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

	//カメラ
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	camera_->SetTarget({ 90.0f,0.0f,0.0f });
	camera_->SetEye({ -20.0f,0.0f,0.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//3Dオブジェクト生成
	//object3DPlayer_ = Object3d::Create();
	//OBJファイルからモデルデータを読み込む
	//modelPlayer_ = Model::LoadFromOBJ("chr_sword",true);
	//オブジェクトにモデル紐付ける
	//object3DPlayer_->SetModel(modelPlayer_);
	////カメラも紐づけ
	//object3DPlayer_->SetCamera(camera_);
	//object3DPlayer_->SetPosition({ 0.0f,0.0f,-15.0f });
	//ライトを生成
	light_ = DirectionalLight::Create();
	light_->SetLightColor({ 1.0f,1.0f,1.0f });
	Object3d::SetLight(light_);

	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture/title.png");
	spriteTitle_->Initialize(spCommon_, titleTex);

	//FBX
	objF = ObjectFbx::Create();
	modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	objF->SetModelFBX(modelF);
	objF->SetCamera(camera_);
	objF->PlayAnimation();//更新で呼ぶと止まるから注意
	
	//パーティクル
	/*particle1_ = Particle::LoadFromParticleTexture("particle2.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);*/
	
}

void TitleScene::Update()
{

	//spriteTitle_->Update();
	//位置指定調整
	//pm1_->Active(particle1_, { -40.0f ,0.0f,0.0f } ,{ 20.0f ,20.0f,20.0f }, { 3.0f,3.0f,3.0f }, { 0.0f,0.001f,0.0f }, 3, { 3.0f, 0.0f });

	//横方向の風
	//pm1_->ActiveX(particle1_, { 30.0f ,30.0f,0.0f } ,{ 25.0f ,10.0f,0.0f }, { -4.2f,0.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
	//横方向の風(ワイド)
	//pm1_->ActiveX(particle1_,{ 30.0f ,30.0f,0.0f }, { 25.0f ,100.0f,0.0f }, { -4.2f,0.2f,0.0f }, { 0.0f,0.001f,0.0f }, 10, { 3.0f, 0.0f });
	//縦方向の風
	//pm1_->ActiveY(particle1_,{ 30.0f ,30.0f,0.0f }, { 10.0f ,25.0f,0.0f }, { 0.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
	//全方位にはじける(クリア演出に使えそう)
	//pm1_->ActiveZ(particle1_,{object3DPlayer_->GetPosition()}, {0.0f ,0.0f,25.0f}, {4.2f,4.2f,0.0f}, {0.0f,0.001f,0.0f}, 10, {3.0f, 0.0f});
	
	//ばらけるような
	//pm1_->ActiveY(particle1_,{ 30.0f ,30.0f,0.0f }, { 10.0f ,25.0f,0.0f }, { 4.2f,4.2f,0.0f }, { 0.0f,0.001f,0.0f }, 1, { 3.0f, 0.0f });
	
	//雪とか雨
	//pm1_->ActiveY(particle1_,{ 30.0f ,30.0f,0.0f }, { 150.0f ,100.0f,0.0f }, { 0.0f,-5.2f,0.0f }, { 0.0f,0.001f,0.0f }, 5, { 5.0f, 0.0f });
	if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		sceneManager_->ChangeScene("GAMEPLAY");
	}
	camera_->Update();
	light_->Update();
	//pm1_->Update();
	//object3DPlayer_->Update();
	
	objF->Update();
}

void TitleScene::Draw()
{
	//背景スプライト描画前処理
	spCommon_->PreDraw();
	//スプライト描画
	//spriteTitle_->Draw();


	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//object3DPlayer_->Draw();
	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	objF->Draw();
	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//前景スプライト

	//ImGuiの表示
	imguiManager_->Draw();
}

void TitleScene::Finalize()
{
	//スプライト
	delete spriteTitle_;
	//プレイヤー
	//delete object3DPlayer_;
	//delete modelPlayer_;
	//ライト
	delete light_;
	//パーティクル
	//delete particle1_;
	//delete pm1_;
	//FBX
	delete objF;
	delete modelF;
}

