#include "PostEffect.h"
#include <d3dx12.h>
#include "WinApp.h"
#include <d3dcompiler.h>
#include "Input.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

/*

*	PostEffect.cpp

*	ポストエフェクト

*/

//静的メンバ変数の実体
const float PostEffect::clearColor_[PostEffect::ClearColorRGBW::CCRGBW_Num] = { 0.25f,0.5f,0.1f,0.0f };

const std::string PostEffect::baseDirectory_ = "Resources/shader/PostEffect/";
const std::string PostEffect::directoryVS_ = "VS.hlsl";
const std::string PostEffect::directoryPS_ = "PS.hlsl";

PostEffect::PostEffect()
{
}

void PostEffect::Initialize(SpriteCommon* spCommon, const std::string& fileName)
{
	assert(spCommon);
	this->spCommon_ = spCommon;

	//頂点バッファ
	CreateVertexBuffer();
	//定数バッファ
	CreateConstBuffer();
	//テクスチャ生成
	CreateTexture();
	// SRV生成
	CreateSRV();
	// RTV生成
	CreateRTV();
	// 深度バッファ生成
	CreateDepthBuffer();
	// DSV生成
	CreateDSV();
	//グラフィックスパイプライン
	CreateGraphicsPipelineState(fileName);
}

void PostEffect::Update()
{
}

void PostEffect::CreateVertexBuffer()
{
	HRESULT result;
	
	//サイズ
	//UINT sizeVB = static_cast<UINT>(sizeof(verticesPost[0]) * _countof(verticesPost));

	//頂点バッファ
	D3D12_HEAP_PROPERTIES heapProp{};	//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU転送用

	const CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Vertex) * verticesCount);
	//リソース設定
	//頂点バッファ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));

	assert(SUCCEEDED(result));

	//データ転送
	Vertex* vertMapPost = nullptr;
	//GPU上のバッファに対応した仮想メモリを取得
	result = vertBuff_->Map(0, nullptr, (void**)&vertMapPost);
	assert(SUCCEEDED(result));

	//全頂点に対して
	for (int i = 0; i < _countof(verticesPost_); i++)
	{
		vertMapPost[i] = verticesPost_[i];	//座標をコピー
	}
	//繋がりを解除
	vertBuff_->Unmap(0, nullptr);

	//頂点バッファビュー
	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeof(Vertex) * verticesCount;
	//頂点一つ分のサイズ
	vbView_.StrideInBytes = sizeof(Vertex);
}

void PostEffect::CreateConstBuffer()
{
	//定数バッファ
	//マテリアル
	CreateConstBufferMaterialPost();
	constMapMaterialPost_->color = this->color_;
	

	//変換行列
	CreateConstBufferTransformPost();
	constMapTransformPost_->mat = XMMatrixIdentity();
	
	constBuffMaterialPost_->Unmap(0, nullptr);
	constBuffTransformPost_->Unmap(0, nullptr);
}

void PostEffect::CreateConstBufferMaterialPost()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	
	const CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff);
	//定数バッファの生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&buffer,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterialPost_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffMaterialPost_->Map(0, nullptr, (void**)&constMapMaterialPost_);//マッピング
	assert(SUCCEEDED(result));
}

void PostEffect::CreateConstBufferTransformPost()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	const CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff);
	//リソース設定
	//定数バッファの生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&buffer, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransformPost_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffTransformPost_->Map(0, nullptr, (void**)&constMapTransformPost_);//マッピング
	assert(SUCCEEDED(result));
}

void PostEffect::CreateTexture()
{
	HRESULT result;
	//テクスチャリソース設定
	const CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::WINDOW_WIDTH,
		(UINT)WinApp::WINDOW_HEIGHT,
		resDescPreset_.arraysize, resDescPreset_.mipLevels, resDescPreset_.sampleCount,
		resDescPreset_.sampleQuality, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	CD3DX12_HEAP_PROPERTIES CHP = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	CD3DX12_CLEAR_VALUE CCV = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);

	//テクスチャバッファ生成
	for (int i = 0; i < TBC_Num; i++)
	{
		result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
			&CHP,
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CCV,
			IID_PPV_ARGS(&texBuff_[i]));
		assert(SUCCEEDED(result));
		{
			//テクスチャ赤クリア
			//画素数(1280 x 720 = 921600ピクセル)
			const UINT pixelCount = WinApp::WINDOW_WIDTH * WinApp::WINDOW_HEIGHT;
			//画像一枚分のデータサイズ
			const UINT rowPitch = sizeof(UINT) * WinApp::WINDOW_WIDTH;
			//画像全体のデータサイズ
			const UINT depthPitch = rowPitch * WinApp::WINDOW_HEIGHT;
			//画像イメージ
			UINT* img = new UINT[pixelCount];
			const UINT imgColor = 0xff0000ff;
			for (int j = 0; j < pixelCount; j++) { img[j] = imgColor; }

			//テクスチャバッファにデータ転送
			result = texBuff_[i]->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
			assert(SUCCEEDED(result));
			delete[] img;
		}
	}
}

void PostEffect::CreateSRV()
{
	HRESULT result;
	//SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	const UINT descriptorNum = 2;
	srvDescHeapDesc.NumDescriptors = descriptorNum;
	//SRV用デスクリプタヒープ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV_));
	assert(SUCCEEDED(result));

	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	const UINT mipLevelNum = 1;
	srvDesc.Texture2D.MipLevels = mipLevelNum;
	for (int i = 0; i < descriptorNum; i++)
	{
		//デスクリプタヒープにSRV作成
		spCommon_->GetDxCommon()->GetDevice()->CreateShaderResourceView(
		texBuff_[i].Get()/*ビューと関連付けるバッファ*/, &srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descHeapSRV_->GetCPUDescriptorHandleForHeapStart(),i,
		spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	}
}

void PostEffect::CreateRTV()
{
	HRESULT result;
	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	const UINT descriptorNum = 2;
	rtvDescHeapDesc.NumDescriptors = descriptorNum;
	//RTV用デスクリプタヒープ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV_));
	assert(SUCCEEDED(result));

	//レンダターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//シェーダーの計算結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < descriptorNum; i++)
	{
		//デスクリプタヒープにRTV作成
		spCommon_->GetDxCommon()->GetDevice()->CreateRenderTargetView(
			texBuff_[i].Get(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV_->GetCPUDescriptorHandleForHeapStart(),i,
				spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	}
}

void PostEffect::CreateDepthBuffer()
{
	HRESULT result;

	//深度バッファリソース設定
	const CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::WINDOW_WIDTH,
		WinApp::WINDOW_HEIGHT,
		resDescPreset_.arraysize, resDescPreset_.mipLevels, resDescPreset_.sampleCount,
		resDescPreset_.sampleQuality,D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		

	const CD3DX12_HEAP_PROPERTIES CHP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_CLEAR_VALUE CCV = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	//深度バッファ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&CHP,
		D3D12_HEAP_FLAG_NONE, &depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CCV,
		IID_PPV_ARGS(&depthBuff_));
	assert(SUCCEEDED(result));
}

void PostEffect::CreateDSV()
{
	HRESULT result;

	//DSV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc{};
	dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	const UINT descriptorNum = 1;
	dsvDescHeapDesc.NumDescriptors = descriptorNum;
	//DSV用デスクリプタヒープ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&dsvDescHeapDesc, IID_PPV_ARGS(&descHeapDSV_));
	assert(SUCCEEDED(result));

	//デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	spCommon_->GetDxCommon()->GetDevice()->CreateDepthStencilView(
	depthBuff_.Get(), &dsvDesc, descHeapDSV_->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::CreateGraphicsPipelineState(const std::string& fileName)
{
	HRESULT result;
	//頂点シェーダー
	//ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPathV = baseDirectory_ + fileName + "/" + fileName + directoryVS_;
	//Resources/shader/PostEffect/ Test / Test VS.hlsl

	const int cbMultiByte = -1;
	//ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSizeV = MultiByteToWideChar(CP_ACP, 0, fullPathV.c_str(), cbMultiByte, nullptr, 0);

	//ワイド文字列に変換
	std::vector<wchar_t> wfilePathV(filePathBufferSizeV);
	MultiByteToWideChar(CP_ACP, 0, fullPathV.c_str(), cbMultiByte, wfilePathV.data(), filePathBufferSizeV);

	//ピクセルシェーダー
	//ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPathP = baseDirectory_ + fileName + "/" + fileName + directoryPS_;

	//ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSizeP = MultiByteToWideChar(CP_ACP, 0, fullPathP.c_str(), cbMultiByte, nullptr, 0);

	//ワイド文字列に変換
	std::vector<wchar_t> wfilePathP(filePathBufferSizeP);
	MultiByteToWideChar(CP_ACP, 0, fullPathP.c_str(), cbMultiByte, wfilePathP.data(), filePathBufferSizeP);


	//シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob = nullptr;		//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;		//ピクセルシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;	//エラーオブジェクト

	//頂点シェーダ読み込み
	result = D3DCompileFromFile(
		wfilePathV.data(),					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード出来るように
		"main", "vs_5_0",									//エントリーポイント名、シェーダモデル
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0,
		&vsBlob, &errorBlob);
	//エラーだと
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダ読み込み
	result = D3DCompileFromFile(
		wfilePathP.data(),					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード出来るように
		"main", "ps_5_0",									//エントリーポイント名、シェーダモデル
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0,
		&psBlob, &errorBlob);

	//エラーだと
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), error.begin());
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
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスク
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定

	//ラスタライザ
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;	//深度クリッピングを有効化
	//デプスステンシルステート
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書き
	//ブレンドステート
	const int defaultRenderTargetNum = 0;
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[defaultRenderTargetNum];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA全てのチャンネルを描画

	//ブレンド共通設定(これ＋合成で動く)
	blenddesc.BlendEnable = false;					//ブレンドを有効にする
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
	//深度バッファのフォーマット
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

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
		DRN_SRV1=1,
		DRN_Num=2,
	};

	D3D12_DESCRIPTOR_RANGE descriptorRange[DRN_Num]{};
	descriptorRange[DRN_SRV0].NumDescriptors = descriptorNum;								//一度の描画に使うテクスチャが1枚なので1
	descriptorRange[DRN_SRV0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[DRN_SRV0].BaseShaderRegister = DRN_SRV0;							//テクスチャレジスタ0番
	descriptorRange[DRN_SRV0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRange[DRN_SRV1].NumDescriptors = descriptorNum;								//一度の描画に使うテクスチャが1枚なので1
	descriptorRange[DRN_SRV1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[DRN_SRV1].BaseShaderRegister = DRN_SRV1;							//テクスチャレジスタ1番
	descriptorRange[DRN_SRV1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//ルートパラメータ設定
	const UINT CBDM_Register = 0;//マテリアル定数バッファのレジスタ
	const UINT CBDT_Register = 1;//座標定数バッファのレジスタ

	D3D12_ROOT_PARAMETER rootParams[RPI_Num] = {};
	//定数バッファ0番
	rootParams[RPI_ConstBuffMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー(種類)
	rootParams[RPI_ConstBuffMaterial].Descriptor.ShaderRegister = CBDM_Register;						//定数バッファ番号
	rootParams[RPI_ConstBuffMaterial].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[RPI_ConstBuffMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える
	//テクスチャレジスタ0番
	rootParams[RPI_TexBuff0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[RPI_TexBuff0].DescriptorTable.pDescriptorRanges = &descriptorRange[DRN_SRV0];		//デスクリプタレンジ
	rootParams[RPI_TexBuff0].DescriptorTable.NumDescriptorRanges = descriptorNum;						//デスクリプタレンジ数
	rootParams[RPI_TexBuff0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える
	//定数バッファ1番
	rootParams[RPI_ConstBuffTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファビュー(種類)
	rootParams[RPI_ConstBuffTransform].Descriptor.ShaderRegister = CBDT_Register;						//定数バッファ番号
	rootParams[RPI_ConstBuffTransform].Descriptor.RegisterSpace = 0;							//デフォルト値
	rootParams[RPI_ConstBuffTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//全てのシェーダから見える
	//テクスチャレジスタ1番
	rootParams[RPI_TexBuff1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//種類
	rootParams[RPI_TexBuff1].DescriptorTable.pDescriptorRanges = &descriptorRange[DRN_SRV1];				//デスクリプタレンジ
	rootParams[RPI_TexBuff1].DescriptorTable.NumDescriptorRanges = descriptorNum;					//デスクリプタレンジ数
	rootParams[RPI_TexBuff1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
	//テクスチャサンプラーの設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;					//横繰り返し(タイリング)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;					//縦繰り返し(タイリング)
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
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = spCommon_->GetDxCommon()->GetDevice()->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature_.Get();

	//パイプラインステート生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

}

void PostEffect::Draw([[maybe_unused]] ID3D12GraphicsCommandList* cmdList)
{
	//パイプラインステートとルートシグネチャの設定
	spCommon_->GetDxCommon()->GetCommandList()->SetPipelineState(pipelineState_.Get());
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブ形状の設定コマンド
	spCommon_->GetDxCommon()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV_.Get() };
	spCommon_->GetDxCommon()->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//テクスチャコマンド
	spCommon_->GetDxCommon()->GetCommandList()->
		SetGraphicsRootDescriptorTable(RPI_TexBuff0, CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart(),TBC_TexBuff0,
			spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	spCommon_->GetDxCommon()->GetCommandList()->
		SetGraphicsRootDescriptorTable(RPI_TexBuff1, CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV_->GetGPUDescriptorHandleForHeapStart(), TBC_TexBuff1,
			spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//頂点バッファビューの設定コマンド
	const UINT viewsNum = 1;
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, viewsNum, &vbView_);

	//定数バッファビュー(CBVの設定コマンド)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(RPI_ConstBuffMaterial, constBuffMaterialPost_->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(RPI_ConstBuffTransform, constBuffTransformPost_->GetGPUVirtualAddress());
	//描画コマンド
	const UINT instanceCount = 1;
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(verticesPost_), instanceCount, 0, 0);

}

void PostEffect::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	const int CRBNum = 2;
	for (int i = 0; i < CRBNum; i++)
	{
		CD3DX12_RESOURCE_BARRIER CRB = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		//リソースバリア変更(シェーダーリソース→描画可能)
		const UINT barrierNum = 1;
		cmdList->ResourceBarrier(barrierNum,&CRB);
	}
	//RTV用デスクリプタヒープのハンドルを取得
	const int RTVNum = 2;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[RTVNum];
	for (int i = 0; i < RTVNum; i++)
	{
		rtvHs[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV_->GetCPUDescriptorHandleForHeapStart(), i,
			spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}
	//DSV用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(RTVNum, rtvHs, false, &dsvH);
	
	CD3DX12_VIEWPORT viewports[RTVNum];
	CD3DX12_RECT scissorRects[RTVNum];
	for (int i = 0; i < RTVNum; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f,WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
		scissorRects[i] = CD3DX12_RECT(0, 0, WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
	}
	//ビューポート設定
	cmdList->RSSetViewports(RTVNum, viewports);
	//シザー設定
	cmdList->RSSetScissorRects(RTVNum, scissorRects);
	for (int i = 0; i < RTVNum; i++)
	{
		//全画面クリア
		cmdList->ClearRenderTargetView(rtvHs[i], clearColor_, 0, nullptr);
	}
	//深度バッファクリア
	const float depthNum = 1.0f;
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, depthNum, 0, 0, nullptr);
}

void PostEffect::PostDraw(ID3D12GraphicsCommandList* cmdList)
{
	const int CRBNum = 2;
	for (int i = 0; i < CRBNum; i++)
	{
		CD3DX12_RESOURCE_BARRIER CRB = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		//リソースバリア変更(描画可能→シェーダーリソース)
		const UINT barrierNum = 1;
		cmdList->ResourceBarrier(barrierNum, &CRB);
	}
}
