#include "SpriteCommon.h"

#include <d3dcompiler.h>
#include <cassert>
#include <string>
#include <DirectXTex.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace IwasiEngine;

/*

*	SpriteCommon.cpp

*	スプライト基盤

*/

//デフォルトテクスチャ格納ディレクトリ
std::string SpriteCommon::kDefaultTextureDirectoryPath_ = "Resources/";

SpriteCommon* SpriteCommon::GetInstance()
{
	static SpriteCommon instance;
	return &instance;
}

void SpriteCommon::Initialize()
{
	HRESULT result;
	this->dxCommon_ = DirectXCommon::GetInstance();

	//頂点シェーダ読み込み
	result = D3DCompileFromFile(
		L"Resources/shader/Sprite/SpriteVS.hlsl",					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード出来るように
		"main", "vs_5_0",									//エントリーポイント名、シェーダモデル
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0,
		&vsBlob_, &errorBlob_);
	//エラーだと
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());
		std::copy_n((char*)errorBlob_->GetBufferPointer(), errorBlob_->GetBufferSize(), error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダ読み込み
	result = D3DCompileFromFile(
		L"Resources/shader/Sprite/SpritePS.hlsl",					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード出来るように
		"main", "ps_5_0",									//エントリーポイント名、シェーダモデル
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0,
		&psBlob_, &errorBlob_);

	//エラーだと
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());
		std::copy_n((char*)errorBlob_->GetBufferPointer(), errorBlob_->GetBufferSize(), error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	//グラフィックスパイプライン
	//設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダ設定
	pipelineDesc.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob_->GetBufferSize();

	//サンプルマスク
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定

	//ラスタライザ
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;	//深度クリッピングを有効化

	//ブレンドステート
	const int defaultRenderTargetNum = 0;
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[defaultRenderTargetNum];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA全てのチャンネルを描画

	//ブレンド共通設定(これ＋合成で動く)
	blenddesc.BlendEnable = true;					//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100％使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を0％使う
	//半透明合成(アルファブレンディング　デフォルト)
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
	/*
	//加算合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;					//加算
	blenddesc.SrcBlend = D3D12_BLEND_ONE;					//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_ONE;					//デストのアルファ値

	//減算合成
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;		//減算
	blenddesc.SrcBlend = D3D12_BLEND_ONE;					//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_ONE;					//デストのアルファ値

	//色反転
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;					//加算
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;		//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_ZERO;					//デストのアルファ値

	*/

	//頂点レイアウト
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//プリミティブ形状
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他
	const UINT renderTargetNum = 1;
	const UINT sampleDescCount = 1;
	pipelineDesc.NumRenderTargets = renderTargetNum;								//描画対象は1つ
	pipelineDesc.RTVFormats[defaultRenderTargetNum] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = sampleDescCount;								//1ピクセルにつき1回サンプリング

	//デスクリプタレンジの設定
	const UINT descriptorNum = 1;
	enum DescriptoeRangeNum
	{
		DRN_SRV0 = 0,
	};
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = descriptorNum;								//一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = DRN_SRV0;							//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	//ルートパラメータ設定
	const UINT CBDM_Register= 0;//マテリアル定数バッファのレジスタ
	const UINT CBDT_Register= 1;//座標定数バッファのレジスタ

	D3D12_ROOT_PARAMETER rootParams[RPI_Num] = {};
	//定数バッファ0番
	rootParams[RPI_ConstBuffMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー(種類)
	rootParams[RPI_ConstBuffMaterial].Descriptor.ShaderRegister = CBDM_Register;						//定数バッファ番号
	rootParams[RPI_ConstBuffMaterial].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[RPI_ConstBuffMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える
	//テクスチャレジスタ0番
	rootParams[RPI_TexBuff].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[RPI_TexBuff].DescriptorTable.pDescriptorRanges = &descriptorRange;			//デスクリプタレンジ
	rootParams[RPI_TexBuff].DescriptorTable.NumDescriptorRanges = descriptorNum;						//デスクリプタレンジ数
	rootParams[RPI_TexBuff].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える
	//定数バッファ1番
	rootParams[RPI_ConstBuffTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー(種類)
	rootParams[RPI_ConstBuffTransform].Descriptor.ShaderRegister = CBDT_Register;						//定数バッファ番号
	rootParams[RPI_ConstBuffTransform].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[RPI_ConstBuffTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える
	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//横繰り返し(タイリング)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//縦繰り返し(タイリング)
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//奥行繰り返し(タイリング)
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;												//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//ピクセルシェーダーからのみ使用可能

	//ルートシグネチャ
	//設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;		//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);			//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	const UINT staticSamplersNum = 1;
	rootSignatureDesc.NumStaticSamplers = staticSamplersNum;

	// ルートシグネチャのシリアライズ
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob_, &errorBlob_);
	assert(SUCCEEDED(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(
		0, rootSigBlob_->GetBufferPointer(), rootSigBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature_.Get();

	//パイプラインステート生成
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.NumDescriptors = kMaxSRVCount_;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように

	//設定をもとにSRV用デスクリプタヒープを生成
	result = dxCommon_->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

}

//テクスチャ読み込み
void SpriteCommon::LoadTexture(const uint32_t index, const std::string& fileName)
{
	HRESULT result;

	//ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = kDefaultTextureDirectoryPath_ + fileName;

	//ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	//ワイド文字列に変換
	std::vector<wchar_t> wfilePath(filePathBufferSize);
	const int cbMultiByte = -1;
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), cbMultiByte, wfilePath.data(), filePathBufferSize);

	//画像ファイルの用意
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath.data(),	//Resourcesフォルダのtexture.png
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);
	
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;							//幅
	textureResourceDesc.Height = (UINT)metadata.height;				//高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	const UINT TRDSampleDescCount = 1;
	textureResourceDesc.SampleDesc.Count = TRDSampleDescCount;

	//テクスチャバッファの生成
	result = dxCommon_->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuffs_[index]));
	assert(SUCCEEDED(result));
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texBuffs_[index]->WriteToSubresource(
			(UINT)i,
			nullptr,							//全領域へコピー
			img->pixels,						//元データアドレス
			(UINT)img->rowPitch,				//1ラインサイズ
			(UINT)img->slicePitch				//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	
	//SRVヒープのハンドルを取得
	incrementSize_ = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();

	srvHandle_.ptr += index * incrementSize_;
	
	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};				//設定構造体
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	 dxCommon_->GetDevice()->
		CreateShaderResourceView(texBuffs_[index].Get(), &srvDesc, srvHandle_);

}

void SpriteCommon::PreDraw()
{
//パイプラインステートとルートシグネチャの設定
	dxCommon_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブ形状の設定コマンド
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get()};
	dxCommon_->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void SpriteCommon::SetTextureCommands(const uint32_t index)
{
	//SRVヒープの先頭ハンドルを取得
	srvGpuHandle_ = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	
	srvGpuHandle_.ptr += index * incrementSize_;

	// SRVヒープの先頭にあるSRVルートパラメータ1番に設定5
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(RPI_TexBuff, srvGpuHandle_);
}