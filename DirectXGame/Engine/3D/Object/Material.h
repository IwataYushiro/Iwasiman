#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	Material.h

	*	マテリアル管理

	*/
	class Material
	{
	private: // エイリアス
		// Microsoft::WRL::を省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::を省略
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public://サブクラス
		//定数バッファ
		struct ConstBufferDataB1
		{
			XMFLOAT3 ambient;	// アンビエント係数
			float pad1;			// パディング
			XMFLOAT3 diffuse;	// ディフューズ係数
			float pad2;			// パディング
			XMFLOAT3 specular;	// スペキュラー係数
			float alpha;		// アルファ値
		};

	public://静的メンバ関数
		//生成
		static Material* Create();
		//静的初期化(デバイス)
		static void StaticInitialize(ID3D12Device* device);

	private://静的メンバ変数
		static ID3D12Device* device_;

	private://プリセット
		const XMFLOAT3 presetAmbient_ = { 0.3f,0.3f,0.3f };				//アンビエント影響度のプリセット
		const XMFLOAT3 presetDiffuse_ = { 0.0f,0.0f,0.0f };				//ディフューズ影響度のプリセット
		const XMFLOAT3 presetSpecular_ = { 0.0f,0.0f,0.0f };				//スペキュラー影響度のプリセット
		const float presetAlpha_ = 1.0f;								//アルファ値のプリセット

	public://共有メンバ変数
		std::string name;				//マテリアル名
		XMFLOAT3 ambient;				//アンビエント影響度
		XMFLOAT3 diffuse;				//ディフューズ影響度
		XMFLOAT3 specular;				//スペキュラー影響度
		float alpha;					//アルファ値
		std::string textureFilename;	//テクスチャファイル名

	public://メンバ関数	

		// テクスチャ読み込み(ディレクトリパス、CPUハンドル、GPUハンドル)
		void LoadTexture(
			const std::string& directoryPath, const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
			const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
		// 更新
		void Update();

	private://メンバ変数
		ComPtr<ID3D12Resource> constBuff_; // 定数バッファ
		// テクスチャバッファ
		ComPtr<ID3D12Resource> texBuff_;
		// シェーダリソースビューのハンドル(CPU)
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
		// シェーダリソースビューのハンドル(CPU)
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
		//マッピング
		ConstBufferDataB1* constMap_ = nullptr;
		//ディレクトリパス
		std::wstring directoryPath_;
		//ファイル名
		std::wstring fileName_;
		//ファイル拡張子
		std::wstring fileExt_;

	private://カプセル化メンバ関数
		//コンストラクタ
		Material() {
			ambient = presetAmbient_;
			diffuse = presetDiffuse_;
			specular = presetSpecular_;
			alpha = presetAlpha_;
			cpuDescHandleSRV_ = {};
			gpuDescHandleSRV_ = {};
		}

		//初期化
		void Initialize();
		//定数バッファ生成
		void CreateConstBuffer();
		//フォルダパスとファイル名を分離
		void SeparateFilePath(const std::wstring& filePath);

	public://アクセッサ置き場
		// 定数バッファゲット
		ID3D12Resource* GetConstantBuffer() { return constBuff_.Get(); }
		//ハンドル関係
		//CPUハンドルゲット
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV_; }
		//GPUハンドルゲット
		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV_; }


	};
}