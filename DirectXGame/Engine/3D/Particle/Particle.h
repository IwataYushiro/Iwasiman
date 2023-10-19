#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <forward_list>

/*

*	Particle.h

*	一粒のパーティクル

*/
class Particle
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス

	struct VertexPosScale
	{
		XMFLOAT3 pos; // xyz座標
		float scale;  //スケール
	};

	//パーティクル一粒
	struct OneParticle
	{
		//DirectX::を省略
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//座標
		XMFLOAT3 position = {};
		//速度
		XMFLOAT3 velocity = {};
		//加速度
		XMFLOAT3 accel = {};
		//現在frame
		int frame = 0;
		//終了frame
		int num_frame = 0;

		//スケール
		float scale = 1.0f;
		//初期値
		float s_scale = 1.0f;
		//最終値
		float e_scale = 0.0f;
	};

private://定数
	static const int VERTEX_COUNT = 1024;		// 頂点数

public://メンバ関数

	//OBJファイルからパーティクルモデルを読み込む(使用パーティクルモデル)
	static Particle* LoadFromParticleTexture(const std::string& fileName);
	// デスクリプタヒープの初期化
	void InitializeDescriptorHeap();
	// テクスチャ読み込み(テクスチャファイルネーム)
	void LoadTexture(const std::string& fileName);

	//各種バッファ生成
	void CreateBuffers();
	//更新
	void Update();
	// 描画(コマンドリスト)
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//パーティクルの追加(寿命、初期座標、速度、重力加速度、開始時スケール、終了時スケール)
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel,
		float start_scale, float end_scale);

private://メンバ変数
	// デバイス
	static ID3D12Device* device_;
	//デフォルトテクスチャ格納ディレクトリ
	static std::string defaultTextureDirectoryPath_;

	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize_;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap_;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff_;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	//頂点データ配列
	VertexPosScale vertices_[VERTEX_COUNT];

	std::forward_list<OneParticle> particles_;

public://アクセッサ置き場
	//デバイスセット
	static void SetDevice(ID3D12Device* device) { Particle::device_ = device; }

};
//オペレーター
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
