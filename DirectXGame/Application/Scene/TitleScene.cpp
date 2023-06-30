#include "TitleScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>


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
	camera_->SetEye({ -10.0f,2.0f,0.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//3Dオブジェクト生成
	object3DPlayer_ = Object3d::Create();
	//OBJファイルからモデルデータを読み込む
	modelPlayer_ = Model::LoadFromOBJ("player",true);
	//オブジェクトにモデル紐付ける
	object3DPlayer_->SetModel(modelPlayer_);
	////カメラも紐づけ
	object3DPlayer_->SetCamera(camera_);
	object3DPlayer_->SetPosition({ 10.0f,0.0f,-9.0f });

	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile("testScene");

	// モデル読み込み
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelFighter = Model::LoadFromOBJ("chr_sword", true);
	modelSphere = Model::LoadFromOBJ("sphere", true);

	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("chr_sword", modelFighter));
	models.insert(std::make_pair("sphere", modelSphere));

	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// モデルを指定して3Dオブジェクトを生成
		Object3d* newObject = Object3d::Create();
		//オブジェクトにモデル紐付ける
		newObject->SetModel(model);

		// 座標
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		// 回転角
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, objectData.rot);
		newObject->SetRotation(rot);

		// 座標
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, objectData.scale);
		newObject->SetScale(scale);

		newObject->SetCamera(camera_);
		// 配列に登録
		objects.push_back(newObject);
	}


	//ライトを生成
	light_ = DirectionalLight::Create();
	light_->SetLightColor({ 1.0f,1.0f,1.0f });
	Object3d::SetLight(light_);

	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture.png");
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

	spriteTitle_->Update();
	
	if (input_->TriggerKey(DIK_SPACE))
	{
		camera_->Reset();
		sceneManager_->ChangeScene("GAMEPLAY");
	}

	for (auto& object : objects) {
		object->Update();
	}
	camera_->Update();
	light_->Update();
	//pm1_->Update();
	object3DPlayer_->Update();
	
	objF->Update();
}

void TitleScene::Draw()
{
	//背景スプライト描画前処理
	spCommon_->PreDraw();
	//スプライト描画
	spriteTitle_->Draw();


	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	object3DPlayer_->Draw();
	for (auto& object : objects) {
		object->Draw();
	}
	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	objF->Draw();
	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//前景スプライト

	
}

void TitleScene::Finalize()
{
	//スプライト
	delete spriteTitle_;
	//プレイヤー
	delete object3DPlayer_;
	delete modelPlayer_;
	//レベルデータ用オブジェクト
	for (Object3d*& object : objects)
	{
		delete object;
	}
	delete modelSkydome;
	delete modelGround;
	delete modelFighter;
	delete modelSphere;

	//ライト
	delete light_;
	//パーティクル
	//delete particle1_;
	//delete pm1_;
	//FBX
	delete objF;
	delete modelF;
}

