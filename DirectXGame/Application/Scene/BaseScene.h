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

#include <map>
#include <memory>
#include <sstream>
#include <fstream>
//前方宣言
//シーンマネージャー
class SceneManager;

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

public://メンバ関数
	virtual~BaseScene() = default;

	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;
	//終了処理
	virtual void Finalize() = 0;
	//フェードインアウト
	virtual void FadeIn(const DirectX::XMFLOAT3& color) = 0;

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

	//イージングのセットコマンド
	std::stringstream setEasingCommands_;

private://メンバ変数
	//シーンマネージャー(借りてくるのでここでdeleteはダメゼッタイ)
	SceneManager* sceneManager_ = nullptr;

public://アクセッサ置き場
	virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager_ = sceneManager; }

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

	//イージングデータの読み込み
	void LoadEasingData(const std::string& fileName,Easing& ease)
	{
		//ディレクトリパスとファイル名を連結してフルパスを得る
		const std::string defaultEasingPath = "Resources/csv/Easing/";
		std::string fullPath = defaultEasingPath + fileName;

		//ワイド文字列に変換した際の文字列バッファサイズを計算
		int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

		//ワイド文字列に変換
		std::vector<wchar_t> wfilePath(filePathBufferSize);
		MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

		//ファイルを開く
		std::ifstream file;
		file.open(wfilePath.data());
		assert(file.is_open());

		//ファイルの内容を文字列ストリームにコピー
		setEasingCommands_ << file.rdbuf();

		//ファイルを閉じる
		file.close();

		//1行分の文字列を入れる関数
		std::string line;
		//コマンド実行ループ
		while (getline(setEasingCommands_, line))
		{
			//1行分の文字列をストリームに変換して解析しやすくする
			std::istringstream line_stream(line);

			std::string word;
			//,区切りで行の戦闘文字列を取得
			getline(line_stream, word, ',');

			// "//"=コメント
			if (word.find("//") == 0)continue;//コメント行を飛ばす
			//イージングのセット
			if (word.find("SET") == 0)
			{
				//x座標
				getline(line_stream, word, ',');
				float start = (float)std::atof(word.c_str());
				//y座標
				getline(line_stream, word, ',');
				float end = (float)std::atof(word.c_str());
				//z座標
				getline(line_stream, word, ',');
				float time = (float)std::atof(word.c_str());
				//イージングセット
				ease.SetEasing(start, end, time);
			}
		}
	}
};
