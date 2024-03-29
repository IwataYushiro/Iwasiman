#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <forward_list>
#include <memory>

namespace IwasiEngine//IwasiEngineのネームスペース
{
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

	private:
		//プリセット
		struct PresetParticle
		{
			//スケール
			const float scale = 1.0f;
			//初期値
			const float s_scale = 1.0f;
			//最終値
			const float e_scale = 0.0f;

			//カラー
			//初期値
			const XMFLOAT4 s_color = { 1.0f,1.0f,1.0f,1.0f };
			//最終値
			const XMFLOAT4 e_color = { 0.0f,0.0f,0.0f,0.0f };
		};

	public://サブクラス
		//ルートパラメータインデックス
		enum RootParameterIndex
		{
			RPI_ConstBuffTransform = 0,
			RPI_TexBuff = 1,
			RPI_Num = 2,
		};
		struct VertexPosScale
		{
			XMFLOAT3 pos; // xyz座標
			float scale;  //スケール
			XMFLOAT4 color;//色
		};

		//パーティクル一粒
		struct OneParticle
		{
			//DirectX::を省略
			using XMFLOAT3 = DirectX::XMFLOAT3;
			//プリセット
			PresetParticle preset;

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
			float scale = preset.scale;
			//初期値
			float s_scale = preset.s_scale;
			//最終値
			float e_scale = preset.e_scale;

			//カラー
			XMFLOAT4 color = {};
			//初期値
			XMFLOAT4 s_color = preset.s_color;
			//最終値
			XMFLOAT4 e_color = preset.e_color;

		};

	private://定数
		static const uint32_t VERTEX_COUNT = 2048;		// 頂点数

	public://メンバ関数

		//OBJファイルからパーティクルモデルを読み込む(使用パーティクルモデル)
		static std::unique_ptr<Particle> LoadFromParticleTexture(const std::string& fileName);
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

		/*パーティクルの追加(寿命、初期座標、速度、重力加速度
		、開始時スケール、終了時スケール, 開始色,終了色)*/
		void Add(const int life, const XMFLOAT3& position, const XMFLOAT3& velocity, const XMFLOAT3& accel,
			const float start_scale, const float end_scale, const XMFLOAT4& start_color, const XMFLOAT4& end_color);

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
}
//オペレーター
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
