#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "Model.h"
#include "Camera.h"
#include "LightGroup.h"
#include <unordered_map>
#include "CollisionInfo.h"

class BaseCollider;

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://コンストラクタ等
	Object3d() = default;
	virtual ~Object3d();

public: // サブクラス
	

	// 定数バッファ用データ構造体
	struct ConstBufferDataB0
	{
		XMMATRIX viewproj;		//ビュープロジェクション行列
		XMMATRIX world;			//ワールド行列
		XMFLOAT3 cameraPos;		//カメラ座標(ワールド座標)
	};
	
private: // 定数
	static const int division = 50;					// 分割数
	static const float radius;				// 底面の半径
	static const float prizmHeight;			// 柱の高さ
	static const int planeCount = division * 2 + division * 2;		// 面の数
	static const int vertexCount = planeCount * 3;		// 頂点数

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static Object3d* Create();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device_;
	
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	
	static ComPtr<ID3DBlob> rootSigBlob;
	static ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	//ライト
	static LightGroup* lightGroup_;

	

private:// 静的メンバ関数
	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

public: // メンバ関数
	virtual bool Initialize();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	//衝突時のコールバック
	virtual void OnCollision(const CollisionInfo& info) {}
protected: // メンバ変数
	//モデル
	Model* model_ = nullptr;
	//カメラ
	Camera* camera_;
	//ビルボード
	bool isBillboard_ = false;
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffB0; 
	ConstBufferDataB0* constMap0 = nullptr;
	// 色
	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	// ローカルスケール
	XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
	// ローカル座標
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	// ローカルワールド変換行列
	XMMATRIX matWorld;
	// 親オブジェクト
	Object3d* parent = nullptr;
	
	//クラス名
	const char* name = nullptr;
	//コライダー
	BaseCollider* collider = nullptr;
	
public: //アクセッサ置き場
	//モデル
	void SetModel(Model* model) { this->model_ = model; }
	//スケール
	const XMFLOAT3& GetScale() const { return scale; }
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	//回転
	const XMFLOAT3& GetRotation() const { return rotation; }
	void SetRotation(const XMFLOAT3& rotation) { this->rotation = rotation; }
	//ワールド座標
	const XMMATRIX& GetWorld() const { return matWorld; }
	void SetWorld(const XMMATRIX& matWorld) { this->matWorld = matWorld; }

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetPosition() const { return position; }

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const XMFLOAT3& position) { this->position = position; }
	
	//カメラ
	void SetCamera( Camera* camera) { this->camera_ = camera; }
	//ライト
	static void SetLightGroup(LightGroup* lightGroup) { Object3d::lightGroup_ = lightGroup; }
	//ビルボード
	void SetBillboard(bool isBillboard) { this->isBillboard_ = isBillboard; }

	//コライダーのセット
	void SetCollider(BaseCollider* collider);
	//モデルの取得
	inline Model* GetModel() { return model_; }
	
};

