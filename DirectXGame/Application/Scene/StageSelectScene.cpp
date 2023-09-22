#include "StageSelectScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include "TouchableObject.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

DirectXCommon* StageSelectScene::dxCommon_ = DirectXCommon::GetInstance();
Input* StageSelectScene::input_ = Input::GetInstance();
Audio* StageSelectScene::audio_ = Audio::GetInstance();
SceneManager* StageSelectScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* StageSelectScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* StageSelectScene::camera_ = Camera::GetInstance();

StageSelectScene::StageSelectScene()
{
}

void StageSelectScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	//カメラ
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//camera_->SetTarget({ 90.0f,0.0f,0.0f });
	//camera_->SetEye({ -10.0f,2.0f,0.0f });

	// 視点座標
	camera_->SetEye({ 0.0f, 5.0f, -100.0f });
	// 注視点座標
	camera_->SetTarget({ 0.0f,0.0f,0.0f });

	//レベルデータ読み込み
	//LoadLVData("enemytest");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT MenuTex = 01;
	spCommon_->LoadTexture(MenuTex, "texture/stageselect.png");
	spriteMenu_->Initialize(spCommon_, MenuTex);
	spriteMenu_->SetPosition({ easeMenuPosX[0].start,0.0f });

	UINT MenuTutorialTex = 02;
	spCommon_->LoadTexture(MenuTutorialTex, "texture/titlemenut.png");
	spriteStage_->Initialize(spCommon_, MenuTutorialTex);
	spriteStage_->SetPosition({ easeMenuPosX[1].start,150.0f });

	UINT MenuDoneTex = 04;
	spCommon_->LoadTexture(MenuDoneTex, "texture/titlemenud.png");
	spriteDone_->Initialize(spCommon_, MenuDoneTex);
	spriteDone_->SetPosition({ easeMenuPosX[3].start,550.0f });

	objStage = Object3d::Create();
	modelStage = Model::LoadFromOBJ("skydome");
	objStage->SetModel(modelStage);
	objStage->SetCamera(camera_);
	objStage->SetScale({ 7.0f,7.0f,7.0f });

	//FBX
	//objF = ObjectFbx::Create();
	//modelF = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	//objF->SetModelFBX(modelF);
	//objF->SetCamera(camera_);
	//objF->PlayAnimation();//更新で呼ぶと止まるから注意

	//パーティクル
	/*particle1_ = Particle::LoadFromParticleTexture("particle2.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);*/
}

void StageSelectScene::Update()
{
	if (MenuCount <= 0)MenuCount = 0;
	else if (MenuCount >= 1)MenuCount = 1;

	rot.y += 0.5f;

	objStage->SetRotation(rot);

	spriteMenu_->Update();
	spriteStage_->Update();
	spriteDone_->Update();

	camera_->Update();
	lightGroup_->Update();
	objStage->Update();
}

void StageSelectScene::Draw()
{

	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	
	objStage->Draw();

	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());

	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	spCommon_->PreDraw();
	//前景スプライト
	//スプライト描画	
	spriteMenu_->Draw();
	spriteStage_->Draw();
	spriteDone_->Draw();
}

void StageSelectScene::Finalize()
{
	//音声
	audio_->Finalize();
	//sprite
	delete spriteMenu_;
	delete spriteStage_;
	delete spriteDone_;
	//ステージ
	delete objStage;
	delete modelStage;
	//ライト
	delete lightGroup_;
}

void StageSelectScene::LoadLVData(const std::string& stagePath)
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile(stagePath);
	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData->objects) {

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}
		
		//地形
		// モデルを指定して3Dオブジェクトを生成
		TouchableObject* newObject = TouchableObject::Create(model, false);
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

}
