#include "Particle.h"
#include <DirectXTex.h>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>

using namespace DirectX;
using namespace std;
using namespace IwasiEngine;

/*

*	Particle.cpp

*	一粒のパーティクル

*/

//静的メンバ変数の実体
ID3D12Device* Particle::device_ = nullptr;
std::string Particle::defaultTextureDirectoryPath_ = "Resources/particle/";

std::unique_ptr<Particle> Particle::LoadFromParticleTexture(const std::string& fileName)
{
	//新たなModel型のインスタンスのメモリを確保
	std::unique_ptr<Particle> particle = std::make_unique<Particle>();

	//デスクリプタヒープ生成
	particle->InitializeDescriptorHeap();

	//OBJファイルからのデータ読み込み
	particle->LoadTexture(fileName);

	//読み込んだデータを元に各種バッファ生成
	particle->CreateBuffers();

	return particle;
}

void Particle::InitializeDescriptorHeap()
{

	HRESULT result = S_FALSE;

	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = 1; // シェーダーリソースビュー1つ
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//生成
	if (FAILED(result)) {
		assert(0);
	}

	// デスクリプタサイズを取得
	descriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

void Particle::LoadTexture(const std::string& fileName)
{
	HRESULT result;

	//ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = defaultTextureDirectoryPath_ + fileName;

	const int cbMultiByte = -1;
	//ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), cbMultiByte, nullptr, 0);

	//ワイド文字列に変換
	std::vector<wchar_t> wfilePath(filePathBufferSize);
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
	const UINT sampleDescCount = 1;
	textureResourceDesc.SampleDesc.Count = sampleDescCount;

	//テクスチャバッファの生成
	result = device_->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff_));
	assert(SUCCEEDED(result));

	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texBuff_->WriteToSubresource(
			(UINT)i,
			nullptr,							//全領域へコピー
			img->pixels,						//元データアドレス
			(UINT)img->rowPitch,				//1ラインサイズ
			(UINT)img->slicePitch				//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}


	//SRVヒープのハンドルを取得
	cpuDescHandleSRV_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap_->GetCPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);
	gpuDescHandleSRV_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap_->GetGPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};				//設定構造体
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	device_->CreateShaderResourceView(texBuff_.Get(), &srvDesc, cpuDescHandleSRV_);

}

void Particle::CreateBuffers()
{
	HRESULT result = S_FALSE;

	std::vector<VertexPosScale> realVertices;

	UINT sizeVB = static_cast<UINT>(sizeof(vertices_));

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPosScale* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices_, sizeof(vertices_));
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(vertices_);
	const int32_t startVerticesNum = 0;
	vbView_.StrideInBytes = sizeof(vertices_[startVerticesNum]);

	
}

void Particle::Update()
{
	HRESULT result;
	//寿命が尽きたパーティクルを全削除
	particles_.remove_if([](OneParticle& x) {return x.frame >= x.num_frame; });
	//全パーティクル更新
	for (std::forward_list<OneParticle>::iterator it = particles_.begin();
		it != particles_.end(); it++)
	{
		//経過フレーム数をカウント
		it->frame++;
		//速度に加速度を加算
		it->velocity = it->velocity + it->accel;
		//速度による移動
		it->position = it->position + it->velocity;
		

		//進行度を0~1の範囲に換算
		float f = (float)it->frame / it->num_frame;
		//スケールの線形補間
		it->scale = (it->e_scale - it->s_scale) * f;
		it->scale += it->s_scale;
		
		//R色の線形補間
		it->color.x = (it->e_color.x - it->s_color.x) * f;
		it->color.x += it->s_color.x;

		//G色の線形補間
		it->color.y = (it->e_color.y - it->s_color.y) * f;
		it->color.y += it->s_color.y;

		//B色の線形補間
		it->color.z = (it->e_color.z - it->s_color.z) * f;
		it->color.z += it->s_color.z;

		//アルファ値の線形補間
		it->color.w = (it->e_color.w - it->s_color.w) * f;
		it->color.w += it->s_color.w;
	}
	//頂点バッファへデータ転送
	VertexPosScale* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		//パーティクルの情報を1つずつ反映
		for (std::forward_list<OneParticle>::iterator it = particles_.begin();
			it != particles_.end(); it++)
		{
			//座標
			vertMap->pos = it->position;
			//スケール
			vertMap->scale = it->scale;
			//色
			vertMap->color = it->color;
			//次の頂点へ
			vertMap++;
		}
		vertBuff_->Unmap(0, nullptr);
	}
	
}

void Particle::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
	assert(device_);
	assert(cmdList);

	// 頂点バッファの設定
	const UINT viewsNum = 1;
	cmdList->IASetVertexBuffers(0, viewsNum, &vbView_);
	
	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(RPI_TexBuff, gpuDescHandleSRV_);
	
	// 描画コマンド
	const UINT instanceCount = 1;
	cmdList->DrawInstanced((UINT)std::distance(particles_.begin(), particles_.end()), instanceCount, 0, 0);

}

void Particle::Add(const int life, const XMFLOAT3& position, const XMFLOAT3& velocity, const XMFLOAT3& accel,
	const float start_scale, const float end_scale, const XMFLOAT4& start_color, const XMFLOAT4& end_color)
{
	//リストに要素を追加
	particles_.emplace_front();
	//追加した要素の参照
	OneParticle& p = particles_.front();
	//値セット
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.num_frame = life;

	p.s_scale = start_scale;
	p.e_scale = end_scale;
	p.scale = p.s_scale;

	p.s_color = start_color;
	p.e_color = end_color;
	p.color = p.s_color;
}

const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	XMFLOAT3 result;

	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}