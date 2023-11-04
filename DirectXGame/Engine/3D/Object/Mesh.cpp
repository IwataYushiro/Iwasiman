#include "Mesh.h"
#include "XYZ.h"
#include <d3dcompiler.h>
#include <cassert>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

/*

*	Mesh.cpp

*	形状(メッシュ)

*/

//静的メンバ変数の実体
ID3D12Device* Mesh::device_ = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device)
{
	Mesh::device_ = device;
	//マテリアル初期化
	Material::StaticInitialize(device);
}

void Mesh::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); ++itr)
	{
		//各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;
		//全頂点の法線を平均する
		XMVECTOR normal = {};
		for (unsigned short index : v)
		{
			normal += XMVectorSet(vertices_[index].normal.x, vertices_[index].normal.y, vertices_[index].normal.z, 0.0f);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		//共通法線を使用する全ての頂点データに書き込む
		for (unsigned short index : v)
		{
			vertices_[index].normal = { normal.m128_f32[XYZ_X],normal.m128_f32[XYZ_Y],normal.m128_f32[XYZ_Z] };
		}
	}
}

void Mesh::CreateBuffers()
{
	HRESULT result = S_FALSE;

	//std::vector<VertexPosNormalUv> realVertices;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース設定
	//リソースデスクのプリセット
	struct ResDescPreset
	{
		const UINT height = 1;
		const UINT16 arraysize = 1;
		const UINT16 mipLevels = 1;
		const UINT sampleCount = 1;

	};
	ResDescPreset resDescPreset;
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeVB;
	resourceDesc.Height = resDescPreset.height;
	resourceDesc.DepthOrArraySize = resDescPreset.arraysize;
	resourceDesc.MipLevels = resDescPreset.mipLevels;
	resourceDesc.SampleDesc.Count = resDescPreset.sampleCount;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	// 頂点バッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成(頂点、インデックスビューはデスクリプタ外で作る)
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	const int32_t startVerticesNum = 0;
	vbView_.StrideInBytes = sizeof(vertices_[startVerticesNum]);

	// リソース設定
	resourceDesc.Width = sizeIB;
	// インデックスバッファ生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&indexBuff_));

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {

		// 全インデックスに対して
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// 頂点バッファの設定
	const UINT viewsNum = 1;
	cmdList->IASetVertexBuffers(0, viewsNum, &vbView_);
	// インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView_);
	// シェーダリソースビューをセット
	const int32_t SRVNum = 2;
	cmdList->SetGraphicsRootDescriptorTable(SRVNum, material_->GetGpuHandle());

	//定数バッファビューのセット(マテリアル)
	ID3D12Resource* constBuff = material_->GetConstantBuffer();
	const int32_t constBuffMaterialNum = 1;
	cmdList->SetGraphicsRootConstantBufferView(constBuffMaterialNum, constBuff->GetGPUVirtualAddress());
	// 描画コマンド
	const UINT instanceCount = 1;
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), instanceCount, 0, 0, 0);

}
