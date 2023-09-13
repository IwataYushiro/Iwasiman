#include "GameClearScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "TouchableObject.h"

using namespace DirectX;

DirectXCommon* GameClearScene::dxCommon_ = DirectXCommon::GetInstance();
Input* GameClearScene::input_ = Input::GetInstance();
Audio* GameClearScene::audio_ = Audio::GetInstance();
SceneManager* GameClearScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GameClearScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* GameClearScene::camera_ = Camera::GetInstance();


void GameClearScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	
	// 視点座標
	camera_->SetEye({ 0.0f, 6.0f, -365.0f });
	// 注視点座標
	camera_->SetTarget({ 0.0f,6.0f,-260.0f });

	LoadModel();
	//レベルデータ読み込み
	LoadLVData("clear");

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT StageTex = 00;
	spCommon_->LoadTexture(StageTex, "texture/gameclear.png");
	spriteGameClear_->Initialize(spCommon_, StageTex);
	//音
BGM = audio_->SoundLoadWave("Resources/sound/bgm/ending.wav");
	doneSE = audio_->SoundLoadWave("Resources/sound/se/done.wav");

	audio_->SoundPlayWave(audio_->GetXAudio2(), BGM, false);

}

void GameClearScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		audio_->SoundPlayWave(audio_->GetXAudio2(), doneSE, false);
		sceneManager_->ChangeScene("TITLE");
		
	}
	for (std::unique_ptr<Earth>& earth : earths_)earth->Update();

	for (Object3d*& object : objects) object->Update();

	spriteGameClear_->Update();
	camera_->Update();
	lightGroup_->Update();

}

void GameClearScene::Draw()
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
	spriteGameClear_->Draw();



}

void GameClearScene::Finalize()
{
	//音声
	audio_->Finalize();
	audio_->SoundUnLoad(&BGM);
	audio_->SoundUnLoad(&doneSE);

	for (Object3d*& object : objects)delete object;
	objects.clear();
	delete modelSkydome;
	delete modelEarth_;
	models.clear();

	delete lightGroup_;
	//スプライト
	delete spriteGameClear_;

}

void GameClearScene::LoadLVData(const std::string& stagePath)
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

void GameClearScene::LoadModel()
{
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelEarth_ = Model::LoadFromOBJ("earth");
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("earth", modelEarth_));

}


