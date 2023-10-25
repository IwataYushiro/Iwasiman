#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "Camera.h"
#include "Particle.h"

/*

*	ParticleManager.h

*	パーティクルマネージャー

*/
class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 頂点データ構造体
	//struct VertexPosNormalUv
	//{
	//	XMFLOAT3 pos; // xyz座標
	//	XMFLOAT3 normal; // 法線ベクトル
	//	XMFLOAT2 uv;  // uv座標
	//};


	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX matBillboard;	//ビルボード行列
	};
	
private: // 定数
	static const int DIVISION = 50;					// 分割数
	static const float RADIUS;				// 底面の半径
	static const float PRIZM_HEIGHT;			// 柱の高さ
	static const int PLANE_COUNT = DIVISION * 2 + DIVISION * 2;		// 面の数
	static const int VERTEX_COUNT = 1024;		// 頂点数
	
public: // 静的メンバ関数
	
	// 静的初期化(デバイス)
	static void StaticInitialize(ID3D12Device* device);

	// 描画前処理(コマンドリスト)
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	//描画後処理
	static void PostDraw();

	//生成
	static ParticleManager* Create();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList_;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate_;

private:// 静的メンバ関数

	// グラフィックパイプライン生成
	static void InitializeGraphicsPipeline();

public: // メンバ関数
	//初期化
	bool Initialize();
	//更新
	void Update();

	//描画
	void Draw();

	// パーティクル発射(パーティクル、初期座標、座標、速度、重力分布、一気に出す量、{開始スケール、終了スケール},色)
	void Active(Particle* p, const XMFLOAT3& setmove, const XMFLOAT3& setpos, const XMFLOAT3& setvel, const XMFLOAT3& setacc,
		const int& setnum, const XMFLOAT2& setscale,const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
	// パーティクル発射X軸(パーティクル、初期座標、座標、速度、重力分布、一気に出す量、{開始スケール、終了スケール},色)
	void ActiveX(Particle* p, const XMFLOAT3& setmove, const XMFLOAT3& setpos, const XMFLOAT3& setvel, const XMFLOAT3& setacc,
 const int& setnum, const XMFLOAT2& setscale, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
	// パーティクル発射Y軸(パーティクル、初期座標、座標、速度、重力分布、一気に出す量、{開始スケール、終了スケール},色)
	void ActiveY(Particle* p, const XMFLOAT3& setmove, const XMFLOAT3& setpos, const XMFLOAT3& setvel, const XMFLOAT3& setacc,
 const int& setnum, const XMFLOAT2& setscale, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
	// パーティクル発射Z軸(パーティクル、初期座標、座標、速度、重力分布、一気に出す量、{開始スケール、終了スケール},色)
	void ActiveZ(Particle* p, const XMFLOAT3& setmove, const XMFLOAT3& setpos, const XMFLOAT3& setvel, const XMFLOAT3& setacc,
 const int& setnum, const XMFLOAT2& setscale, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

private: // メンバ変数
	ComPtr<ID3D12Resource> constBuff_; // 定数バッファ
	// ローカルスケール
	XMFLOAT3 scale_ = { 1.0f,1.0f,1.0f };
	//一粒のパーティクル
	Particle* particle_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

public://アクセッサ置き場
	//パーティクルモデルセット
	void SetParticleModel(Particle* pmodel) { this->particle_ = pmodel; }
	//カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }
};

