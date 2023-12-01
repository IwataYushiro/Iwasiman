#include "Material.h"
#include <DirectXTex.h>
#include <cassert>

using namespace DirectX;
using namespace std;

/*

*	Material.cpp

*	マテリアル管理

*/

// 静的メンバ変数の実体
ID3D12Device* Material::device_ = nullptr;

Material* Material::Create()
{
	Material* mat = new Material;

	mat->Initialize();

	return mat;
}

void Material::StaticInitialize(ID3D12Device* device)
{
	Material::device_ = device;
}

void Material::LoadTexture(const std::string& directoryPath, const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	HRESULT result = S_FALSE;

	
	// テクスチャなし
	if (textureFilename.size() == 0) {
		textureFilename = "white1x1.png";
	}
	// シェーダリソースビュー作成
	cpuDescHandleSRV_ = cpuHandle;
	gpuDescHandleSRV_ = gpuHandle;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	//ファイルパスを結合
	string filepath = directoryPath + textureFilename;

	//ユニコード文字列に変換する
	const int maxPathNum = 128;
	wchar_t wfilePath[maxPathNum];
	const int cbMultiByte = -1;
	MultiByteToWideChar(CP_ACP, 0,
		filepath.c_str(), cbMultiByte, wfilePath, _countof(wfilePath));
	// WICテクスチャのロード
	result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	
	// リソース設定
	D3D12_RESOURCE_DESC texresDesc{};
	texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;
	texresDesc.Height = (UINT)metadata.height;
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	const UINT sampleCountNum = 1;
	texresDesc.SampleDesc.Count = sampleCountNum;
	
	// テクスチャ用バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr, IID_PPV_ARGS(&texBuff_));
	assert(SUCCEEDED(result));

	// テクスチャバッファにデータ転送
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0); // 生データ抽出
		result = texBuff_->WriteToSubresource(
			(UINT)i,
			nullptr,              // 全領域へコピー
			img->pixels,          // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texBuff_->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	const UINT tex2DMipLevelsNum = 1;
	srvDesc.Texture2D.MipLevels = tex2DMipLevelsNum;

	device_->CreateShaderResourceView(texBuff_.Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		cpuDescHandleSRV_
	);
}

void Material::Update()
{
	//定数バッファへ転送
	constMap_->ambient = ambient;
	constMap_->diffuse = diffuse;
	constMap_->specular = specular;
	constMap_->alpha = alpha;
}

void Material::Initialize()
{
	//定数バッファ生成
	CreateConstBuffer();
}

void Material::CreateConstBuffer()
{
	HRESULT result;
	//定数バッファ(マテリアル)
	//ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソース設定
	//リソースデスクのプリセット
	struct ResDescPreset
	{
		const UINT64 width = 0xff;
		const UINT height = 1;
		const UINT16 arraysize = 1;
		const UINT16 mipLevels = 1;
		const UINT sampleCount = 1;

	};
	ResDescPreset resDescPreset;
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = (sizeof(ConstBufferDataB1) + resDescPreset.width) & ~resDescPreset.width;
	resourceDesc.Height = resDescPreset.height;
	resourceDesc.DepthOrArraySize = resDescPreset.arraysize;
	resourceDesc.MipLevels = resDescPreset.mipLevels;
	resourceDesc.SampleDesc.Count = resDescPreset.sampleCount;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	// 定数バッファへデータ転送
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}
