#include "GameOverScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "TouchableObject.h"

using namespace DirectX;

DirectXCommon* GameOverScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();
Audio* GameOverScene::audio_ = Audio::GetInstance();
SceneManager* GameOverScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameOverScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* GameOverScene::camera_ = Camera::GetInstance();


void GameOverScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	XMFLOAT3 eye = camera_->GetEye();
	eye = { 0.0f, 6.0f, -365.0f };
	camera_->SetEye(eye);
	// 注視点座標
	XMFLOAT3 target = camera_->GetEye();
	target = { 0.0f,6.0f,-260.0f };
	camera_->SetTarget(target);

	LoadModel();
	//レベルデータ読み込み
	LoadLVData("gameover");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT StageTex = 00;
	spCommon_->LoadTexture(StageTex, "texture/gameover.png");
	spriteGameOver_->Initialize(spCommon_, StageTex);

	BGM = audio_->SoundLoadWave("Resources/sound/bgm/gameover.wav");
	doneSE = audio_->SoundLoadWave("Resources/sound/se/done.wav");

	audio_->SoundPlayWave(audio_->GetXAudio2(), BGM, false);

}

void GameOverScene::Update()
{


	if (input_->TriggerKey(DIK_SPACE))
	{
		audio_->SoundPlayWave(audio_->GetXAudio2(), doneSE, false);
		sceneManager_->ChangeScene("TITLE");

	}


	for (std::unique_ptr<Earth>& earth : earths_)earth->Update();

	for (Object3d*& object : objects) object->Update();

	spriteGameOver_->Update();
	camera_->Update();
	lightGroup_->Update();

}

void GameOverScene::Draw()
{

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	for (std::unique_ptr<Earth>& earth : earths_)earth->Draw();
	for (auto& object : objects)object->Draw();
	//モデル描画後処理
	Object3d::PostDraw();

	//Fbxモデル描画前処理
	ObjectFbx::PreDraw(dxCommon_->GetCommandList());


	//Fbxモデル描画後処理
	ObjectFbx::PostDraw();

	//前景スプライト
//背景スプライト描画前処理
	spCommon_->PreDraw();
	//スプライト描画
	spriteGameOver_->Draw();



}

void GameOverScene::Finalize()
{
	//音声
	audio_->Finalize();
	audio_->SoundUnLoad(&BGM);
	audio_->SoundUnLoad(&doneSE);

	for (Object3d*& object : objects)delete object;
	objects.clear();
	delete modelEnemy1_;
	delete modelEnemy1Power_;
	delete modelEnemy1Guard_;
	delete modelEnemy1Speed_;
	delete modelEnemy1Death_;
	delete modelEnemy2_;
	delete modelEnemy2Power_;
	delete modelEnemy2Guard_;
	delete modelEnemy2Speed_;
	delete modelEnemy2Death_;
	delete modelSkydome;
	delete modelEarth_;
	models.clear();


	delete lightGroup_;
	//スプライト
	delete spriteGameOver_;

}

void GameOverScene::LoadLVData(const std::string& stagePath)
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

		if (objectData.objectType.find("EARTH") == 0)
		{
			//ゴール初期化
			std::unique_ptr<Earth> newearth;
			newearth = Earth::Create(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newearth->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newearth->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newearth->SetScale(scale);

			newearth->SetCamera(camera_);
			newearth->Update();
			//リストに登録
			earths_.push_back(std::move(newearth));
		}
		//地形
		else
		{
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

}

void GameOverScene::LoadModel()
{
	modelEnemy1_ = Model::LoadFromOBJ("enemy1");
	modelEnemy1Power_ = Model::LoadFromOBJ("enemy1p");
	modelEnemy1Guard_ = Model::LoadFromOBJ("enemy1g");
	modelEnemy1Speed_ = Model::LoadFromOBJ("enemy1s");
	modelEnemy1Death_ = Model::LoadFromOBJ("enemy1d");
	modelEnemy2_ = Model::LoadFromOBJ("enemy2");
	modelEnemy2Power_ = Model::LoadFromOBJ("enemy2p");
	modelEnemy2Guard_ = Model::LoadFromOBJ("enemy2g");
	modelEnemy2Speed_ = Model::LoadFromOBJ("enemy2s");
	modelEnemy2Death_ = Model::LoadFromOBJ("enemy2d");
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelEarth_ = Model::LoadFromOBJ("earth2");

	models.insert(std::make_pair("enemy1", modelEnemy1_));
	models.insert(std::make_pair("enemy1p", modelEnemy1Power_));
	models.insert(std::make_pair("enemy1g", modelEnemy1Guard_));
	models.insert(std::make_pair("enemy1s", modelEnemy1Speed_));
	models.insert(std::make_pair("enemy1d", modelEnemy1Death_));
	models.insert(std::make_pair("enemy2", modelEnemy2_));
	models.insert(std::make_pair("enemy2p", modelEnemy2Power_));
	models.insert(std::make_pair("enemy2g", modelEnemy2Guard_));
	models.insert(std::make_pair("enemy2s", modelEnemy2Speed_));
	models.insert(std::make_pair("enemy2d", modelEnemy2Death_));
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("earth2", modelEarth_));

}


