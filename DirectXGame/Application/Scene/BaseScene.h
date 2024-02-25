#pragma once
#include <DirectXMath.h>
#include "Audio.h"
#include "Easing.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Object3d.h"
#include "ObjectFbx.h"
#include "LevelLoaderJson.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "PostEffect.h"
#include "XYZ.h"

#include <map>
#include <memory>

//前方宣言
//シーンマネージャー
class SceneManager;
//jsonレベルデータ
struct LevelData;

/*

*	BaseScene.h (cpp X)

*	シーン基盤

*/
class BaseScene
{
protected://エイリアス
	using DirectXCommon = IwasiEngine::DirectXCommon;
	using Input = IwasiEngine::Input;
	using Audio = IwasiEngine::Audio;
	using ImGuiManager = IwasiEngine::ImGuiManager;
	using Sprite = IwasiEngine::Sprite;
	using SpriteCommon = IwasiEngine::SpriteCommon;
	using Model = IwasiEngine::Model;
	using Object3d = IwasiEngine::Object3d;
	using Camera = IwasiEngine::Camera;
	using Easing = IwasiEngine::Easing;
	using LevelData = IwasiEngine::LevelData;
	using LightGroup = IwasiEngine::LightGroup;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;
	using PostEffect = IwasiEngine::PostEffect;

public://メンバ関数
	virtual~BaseScene() = default;

	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;
	//ポストエフェクト描画
	virtual void DrawPostEffect() = 0;
	//終了処理
	virtual void Finalize() = 0;
	//フェードインアウト
	virtual void FadeIn(const DirectX::XMFLOAT3& color) = 0;
	//イージングのロード
	virtual void LoadEasing() = 0;

protected://継承メンバ変数
	//現在ステージのポジション
	const DirectX::XMFLOAT2 stageInfoNowPos_ = { 300.0f,100.0f };
	//ローディング中のポジション
	const DirectX::XMFLOAT2 loadPos_ = { 350.0f,600.0f };
	//ステージ名のポジション
	const DirectX::XMFLOAT2 stageNamePos_ = { 630.0f,500.0f };
	//色サンプル
	const DirectX::XMFLOAT3 white_ = { 1.0f,1.0f,1.0f };	//白
	const DirectX::XMFLOAT3 black_ = { 0.0f,0.0f,0.0f };	//黒
	const DirectX::XMFLOAT3 deepRed_ = { 0.5f,0.2f,0.2f };	//濃い赤
	//アンカーポイントのプリセット
	const DirectX::XMFLOAT2 ANCHOR_POINT_CENTRAL = { 0.5f,0.5f };//中央

	//マッピングモデル
	std::map<std::string, Model*> models_;
	//演出スキップ用カウント
	bool skip_ = false;
	//ポストエフェクト
	std::unique_ptr<PostEffect> postEffect_;

	//jsonレベルデータ
	std::unique_ptr<LevelData> levelData_ = nullptr;

private://メンバ変数
	//シーンマネージャー(借りてくるのでここでdeleteはダメゼッタイ)
	SceneManager* sceneManager_ = nullptr;
	//ダーティフラグ
	bool dirty_ = false;

public://アクセッサ置き場
	//シーンマネージャーのセット
	virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager_ = sceneManager; }
	//ポストエフェクト変更
	void ChangePostEffect(const std::string& post = "None") { postEffect_->CreateGraphicsPipelineState(post); }
	//ポストエフェクトカラー変更
	void ChangePostEffectColor(const DirectX::XMFLOAT4 color) { postEffect_->SetColor(color); }
	//ポストエフェクトカラー(ペラポリゴン自体の)変更
	void ChangePostEffectTextureColor(const DirectX::XMFLOAT4 color) { postEffect_->SetTextureColor(color); }
	//ポストエフェクトパワー変更
	void ChangePostEffectPower(const float power) { postEffect_->SetPower(power); }

protected://継承メンバ関数
	//モデル読み込みとマッピングの共通化
	void ModelMapping(std::unique_ptr<Model>& model, const std::string& modelName, bool smoothing = false)
	{
		//モデルを読み込む
		model = Model::LoadFromOBJ(modelName, smoothing);				//自機モデル
		//マップに登録
		models_.insert(std::make_pair(modelName, model.get()));
	}
	//スペースを押すと演出をスキップする関数
	void SkipDirectionOnSpace(const DirectX::XMFLOAT3& color)
	{
		//スペースを押すとスキップするようにした
		if (Input::GetInstance()->TriggerKey(DIK_SPACE))skip_ = true;
		//演出スキップ
		if (skip_)FadeIn(color);
	}
	//ポストエフェクトの値のテストツール
	void TestPostEffect(const std::string& path)
	{
		//ポストエフェクトの読み込みは一回だけ
		if (!dirty_)
		{
			postEffect_->Initialize(path);
			dirty_ = true;
		}
		//ImGuiに渡す用の変数
		//エフェクトの色
		float icolor[XYZW_Num] = { postEffect_->GetColor().x,postEffect_->GetColor().y,
			postEffect_->GetColor().z, postEffect_->GetColor().w };
		//素材そのものの色(ペラポリゴンの色)
		float itexcolor[XYZW_Num] = { postEffect_->GetTextureColor().x,postEffect_->GetTextureColor().y,
			postEffect_->GetTextureColor().z, postEffect_->GetTextureColor().w };
		//パワー
		float ipower = postEffect_->GetPower();

		//ウィンドウポジション
		struct ImGuiWindowPosition
		{
			const float X = 100.0f;
			const float Y = 0.0f;
		};
		ImGuiWindowPosition iPos;
		//ウィンドウサイズ
		struct ImguiWindowSize
		{
			const float width = 300.0f;
			const float height = 150.0f;
		};
		ImguiWindowSize iSize;
		//調整はスライダーで
		ImGui::Begin("PostEffectTest");
		ImGui::SetWindowPos(ImVec2(iPos.X, iPos.Y));
		ImGui::SetWindowSize(ImVec2(iSize.width, iSize.height));
		ImGui::SliderFloat4("color", icolor, 0.0f, 1.0f);
		ImGui::SliderFloat4("texturecolor", itexcolor, 0.0f, 1.0f);
		ImGui::SliderFloat("power", &ipower, 0.0f, 1.0f);
		ImGui::End();

		//値を適応
		postEffect_->SetColor({ icolor[XYZW_X],icolor[XYZW_Y],icolor[XYZW_Z],icolor[XYZW_W] });
		postEffect_->SetTextureColor({ itexcolor[XYZW_X],itexcolor[XYZW_Y],itexcolor[XYZW_Z],itexcolor[XYZW_W] });
		postEffect_->SetPower(ipower);
		postEffect_->Update();
	}

};

