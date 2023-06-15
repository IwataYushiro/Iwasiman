#pragma once
#include "ModelFbx.h"
#include "Camera.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include <Windows.h>
#include <wrl.h>

class ObjectFbx
{
protected://エイリアス
	//Microsoft::WRL::を省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://定数
	//ボーンの最大数
	static const int MAX_BONES = 32;

public://サブクラス
	
	//定数バッファ構造体(座標変換行列)
	struct ConstBufferDataTransform
	{
		XMMATRIX viewProj;		//ビュプロ行列
		XMMATRIX world;			//ワールド行列
		XMFLOAT3 cameraPos;		//カメラ座標
	};
	//スキニング情報
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};


public://静的メンバ関数
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);
	// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	// 描画後処理
	static void PostDraw();
	// 3Dオブジェクト生成
	static ObjectFbx* Create();

	//セッター
	static void SetDevice(ID3D12Device* device) { ObjectFbx::device_ = device; }
	
private:
	//グラフィックスパイプライン生成
	static void CreateGraphicsPipeline();

private://静的メンバ変数
	//デバイス
	static ID3D12Device* device_;
	
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList_;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature_;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelineState_;

public://メンバ関数

	//初期化
	bool Initialize();
	//更新
	void Update();
	//描画
	void Draw();
	//アニメーション開始
	void PlayAnimation();

protected://メンバ変数
	//ローカルスケール
	XMFLOAT3 scale_ = { 1.0f,1.0f,1.0f };
	// ローカル回転角
	XMFLOAT3 rotation_ = { 0.0f,0.0f,0.0f };
	// ローカル座標
	XMFLOAT3 position_ = { 0.0f,0.0f,0.0f };
	// ローカルワールド行列
	XMMATRIX matWorld_ = {};
	// モデル
	ModelFbx* modelF_ = nullptr;
	//定数バッファ
	ComPtr<ID3D12Resource> constBufferTransform;
	//スキン
	ComPtr<ID3D12Resource> constBufferSkin;

	//1フレームの時間
	FbxTime frameTime;
	// アニメーション開始時間
	FbxTime startTime;
	// アニメーション終了時間
	FbxTime endTime;
	//アニメーション現在時間
	FbxTime currentTime;
	//アニメーション再生中フラグ
	bool isPlayAnimation = false;

	//カメラ
	Camera* camera_ = nullptr;
public://アクセッサ置き場
	void SetModelFBX(ModelFbx* modelF) { this->modelF_ = modelF; }
	void SetCamera(Camera* camera) { this->camera_ = camera; }

};
