#include "TitleScene.h"
#include "FbxLoader.h"
#include "LevelLoaderJson.h"
#include <cassert>
#include <iomanip>
#include "TouchableObject.h"
#include "CollisionAttribute.h"
using namespace DirectX;

DirectXCommon* TitleScene::dxCommon_ = DirectXCommon::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
Audio* TitleScene::audio_ = Audio::GetInstance();
SceneManager* TitleScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* TitleScene::imguiManager_ = ImGuiManager::GetInstance();
Camera* TitleScene::camera_ = Camera::GetInstance();


void TitleScene::Initialize()
{
	spCommon_ = SpriteCommon::GetInstance();
	//オーディオ
	audio_->Initialize();

	//カメラ
	//camera_->SetEye({ 0.0f,0.0f,-150.0f });
	//camera_->SetTarget({ 0.0f,20.0f,0.0f });

	//camera_->SetTarget({ 90.0f,0.0f,0.0f });
	//camera_->SetEye({ -10.0f,2.0f,0.0f });


	LoadModel();

	//レベルデータ読み込み
	LoadLVData("title");
	XMFLOAT3 eye = camera_->GetEye();
	eye = { 0.0f, 6.0f, -365.0f };
	camera_->SetEye(eye);
	// 注視点座標
	XMFLOAT3 target = camera_->GetEye();
	target = { 0.0f,6.0f,-260.0f };
	camera_->SetTarget(target);

	//制御点
	startEye = eye;
	p1Eye = { 0.0f ,105.0f,-250.0f };
	p2Eye = { 0.0f ,60.0f,100.0f };
	endEye = { 0.0f ,6.0f,-115.0f };

	startTarget = target;
	p1Target = { 0.0f ,105.0f,-150.0f };
	p2Target = { 0.0f ,60.0f,100.0f };
	endTarget = { 0.0f ,5.0f,0.0f };

	//ライトを生成
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_);

	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture/title3.png");
	spriteTitle_->Initialize(spCommon_, titleTex);
	UINT htpTex = 01;
	spCommon_->LoadTexture(htpTex, "texture/howtoplay.png");
	spriteHTP_->Initialize(spCommon_, htpTex);

	spriteTitle_->SetPosition({ 0.0f,easeTitlePosY.start });
	spriteHTP_->SetPosition({ 0.0f,easeHTPPosY.start });
	//パーティクル
	/*particle1_ = Particle::LoadFromParticleTexture("particle2.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);*/
	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間
	elapsedCount;	//経過時間 経過時間=現在時間-開始時間
	maxTime = 2.0f;					//全体時間
	timeRate;

	isStart = false;
}

void TitleScene::Update()
{

	if (isStart)
	{
		isHTP = false;
		easeHTPPosY.ease_out_expo();
		spriteHTP_->SetPosition({ 0.0f,easeHTPPosY.num_X });
		//時間
		//現在時間を取得する
		nowCount = std::chrono::steady_clock::now();
		//前回記録からの経過時間を取得する
		elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>(nowCount - startCount);

		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

		timeRate = min(elapsed / maxTime, 1.0f);
		camera_->SetEye(Bezier3(startEye, p1Eye, p2Eye, endEye, timeRate));
		camera_->SetTarget(Bezier3(startTarget, p1Target, p2Target, endTarget, timeRate));

		if (camera_->GetEye().z == endEye.z && camera_->GetTarget().z == endTarget.z)
		{
			camera_->Reset();
			sceneManager_->ChangeScene("GAMEPLAY", 1);
		}

	}
	else if (isHTP)
	{
		easeTitlePosY.ease_out_expo();
		easeHTPPosY.ease_out_expo();
		spriteTitle_->SetPosition({ 0.0f,easeTitlePosY.num_X });
		spriteHTP_->SetPosition({ 0.0f,easeHTPPosY.num_X });
		if (input_->TriggerKey(DIK_SPACE))
		{
			easeHTPPosY.Standby(true);
			isStart = true;
			startCount = std::chrono::steady_clock::now();
		}
	}
	else
	{

		for (std::unique_ptr<Player>& player : players_)player->Update();
		if (input_->TriggerKey(DIK_SPACE))
		{
			easeTitlePosY.Standby(false);
			easeHTPPosY.Standby(false);
		
			isHTP = true;
		}
	}

	for (std::unique_ptr<Earth>& earth : earths_)earth->Update();

	for (Object3d*& object : objects) object->Update();


	spriteTitle_->Update();
	spriteHTP_->Update();

	camera_->Update();
	lightGroup_->Update();
	//pm1_->Update();

	imguiManager_->Begin();

	imguiManager_->End();

}

void TitleScene::Draw()
{
	//背景スプライト描画前処理

	//エフェクト描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//pm1_->Draw();
	//エフェクト描画後処理
	ParticleManager::PostDraw();


	//モデル描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//モデル描画
	for (std::unique_ptr<Player>& player : players_)player->Draw();
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Draw();
	for (std::unique_ptr<Earth>& earth : earths_)earth->Draw();
	for (auto& object : objects)object->Draw();
	//モデル描画後処理
	Object3d::PostDraw();

	//前景スプライト
	spCommon_->PreDraw();
	//スプライト描画
	if (!isStart)spriteTitle_->Draw();
	spriteHTP_->Draw();
	
}

void TitleScene::Finalize()
{
	//スプライト
	delete spriteTitle_;
	delete spriteHTP_;
	//モデル

	for (Object3d*& object : objects)delete object;
	objects.clear();

	//3Dモデル
	delete modelPlayer_;
	delete modelPlayerBullet_;
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
	delete modelEnemyBullet_;
	delete modelSkydome;
	delete modelGround;
	delete modelBox;
	delete modelEarth_;
	delete modelRail;

	models.clear();

	//ライト
	delete lightGroup_;
	//パーティクル
	//delete particle1_;
	//delete pm1_;

}

const XMFLOAT3 TitleScene::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	XMFLOAT3 ans;
	ans.x = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.x + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.x + 3 * (1.0f - t) * t * t * p2.x + t * t * t * p3.x;
	ans.y = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.y + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.y + 3 * (1.0f - t) * t * t * p2.y + t * t * t * p3.y;
	ans.z = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.z + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.z + 3 * (1.0f - t) * t * t * p2.z + t * t * t * p3.z;

	return ans;
}

void TitleScene::LoadLVData(const std::string& stagePath)
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
		//プレイヤー
		if (objectData.objectType.find("PLAYER") == 0)
		{
			//プレイヤー初期化
			std::unique_ptr<Player> newplayer;

			newplayer = Player::Create(model, modelPlayerBullet_, nullptr);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newplayer->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newplayer->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newplayer->SetScale(scale);

			newplayer->SetCamera(camera_);
			newplayer->Update();
			//リストに登録
			players_.push_back(std::move(newplayer));
		}
		else if (objectData.objectType.find("EARTH") == 0)
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

void TitleScene::LoadModel()
{
	// モデル読み込み
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelPlayerBullet_ = Model::LoadFromOBJ("playerbullet");
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
	modelEnemyBullet_ = Model::LoadFromOBJ("enemybullet");
	modelSkydome = Model::LoadFromOBJ("skydome");
	modelGround = Model::LoadFromOBJ("ground");
	modelBox = Model::LoadFromOBJ("sphere2", true);
	modelRail = Model::LoadFromOBJ("rail");
	modelEarth_ = Model::LoadFromOBJ("earth");


	models.insert(std::make_pair("player", modelPlayer_));
	models.insert(std::make_pair("playerbullet", modelPlayerBullet_));
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
	models.insert(std::make_pair("enemybullet", modelEnemyBullet_));
	models.insert(std::make_pair("skydome", modelSkydome));
	models.insert(std::make_pair("ground", modelGround));
	models.insert(std::make_pair("sphere2", modelBox));
	models.insert(std::make_pair("rail", modelRail));
	models.insert(std::make_pair("earth", modelEarth_));
}


