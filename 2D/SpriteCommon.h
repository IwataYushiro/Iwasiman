#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

//スプライト基盤クラス

class SpriteCommon
{
public:
	//エイリアステンプレートとかで色々省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://メンバ関数

	~SpriteCommon();
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//テクスチャ読み込み
	
	//描画前処理
	void PreDraw();

private://メンバ変数

	DirectXCommon* dxCommon_ = nullptr;

	//SRVの最大個数
	static const size_t kMaxSRVCount = 2056;

	//テクスチャバッファの生成
	ComPtr<ID3D12Resource> texbuff = nullptr;
	// 画像イメージデータ配列
	XMFLOAT4* imageData;

	//設定をもとにSRV用デスクリプタヒープを生成
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	//SRVヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;


	//シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト

	//パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;

public://アクセッサ

	DirectXCommon* GetDxCommon() { return dxCommon_; }
	ComPtr<ID3D12Resource> GetTextureBuffer() { return texbuff; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return srvHandle; }
};
