#pragma once
#include <array>
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>

/*

*	SpriteCommon.h

*	スプライト基盤

*/
class SpriteCommon final
{
public:
	//エイリアステンプレートとかで色々省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://シングルトンインスタンス
	static SpriteCommon* GetInstance();

public://構造体、列挙体
	//ルートパラメータインデックス
	enum RootParameterIndex
	{
		RPI_ConstBuff0 = 0,
		RPI_TexBuff0 = 1,
		RPI_ConstBuff1 = 2,
		RPI_Num = 3,
	};

public://メンバ関数

	//初期化(DirectX基盤)
	void Initialize(DirectXCommon* dxCommon);

	//テクスチャ読み込み(テクスチャインデックス、使用スプライト)
	void LoadTexture(uint32_t index, const std::string& fileName);

	//描画前処理
	void PreDraw();

	//描画用テクスチャコマンドの発行(テクスチャインデックス)
	void SetTextureCommands(uint32_t index);

private://メンバ変数

	//DirectX基盤
	DirectXCommon* dxCommon_ = nullptr;

	//デフォルトテクスチャ格納ディレクトリ
	static std::string kDefaultTextureDirectoryPath_;

	//SRVの最大個数
	static const size_t kMaxSRVCount_ = 2056;

	//テクスチャバッファの生成
	std::array<ComPtr<ID3D12Resource>,kMaxSRVCount_> texBuffs_;

	//サイズの問い合わせ
	UINT incrementSize_;
	
	//設定をもとにSRV用デスクリプタヒープを生成
	ComPtr<ID3D12DescriptorHeap> srvHeap_;
	//SRVヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
	//SRVGPUヒープのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_;


	//シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob_ = nullptr;		//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;		//ピクセルシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob_ = nullptr;	//エラーオブジェクト

	//パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState_;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob_ = nullptr;

public://アクセッサ

	//DirectX基盤ゲット
	DirectXCommon* GetDxCommon() { return dxCommon_; }
	
	//SRVヒープゲット
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return srvHandle_; }
	//SRVGPUヒープゲット
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() { return srvGpuHandle_; }
	//テクスチャバッファゲット
	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return texBuffs_[index].Get(); }
	//パイプラインステートゲット
	ID3D12PipelineState* GetPipelineState() { return pipelineState_.Get(); }
	//ルートシグネチャゲット
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
private:
	//コンストラクタ（シングルトンパターン）
	SpriteCommon() = default;
	//デストラクタ（シングルトンパターン）
	~SpriteCommon() = default;
public:
	//コピーコンストラクタの防止（シングルトンパターン）
	SpriteCommon(const SpriteCommon& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	SpriteCommon& operator=(const SpriteCommon& obj) = delete;
};
