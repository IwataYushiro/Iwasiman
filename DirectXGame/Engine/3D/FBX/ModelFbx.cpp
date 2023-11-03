#include "ModelFbx.h"

/*

*	ModelFbx.cpp

*	FBXモデル

*/

//静的メンバ変数の実体
ID3D12Device* ModelFbx::device_ = nullptr;

ModelFbx::~ModelFbx()
{
	//Fbxシーン開放
	fbxScene_->Destroy();
}

void ModelFbx::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
	assert(cmdList);

	// 頂点バッファの設定
	const UINT viewsNum = 1;
	cmdList->IASetVertexBuffers(0, viewsNum, &vbView_);
	// インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView_);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// シェーダリソースビューをセット
	const int32_t texBuffSRV = 1;
	cmdList->SetGraphicsRootDescriptorTable(texBuffSRV,
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart());
	
	// 描画コマンド
	const UINT instanceCount = 1;
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), instanceCount, 0, 0, 0);

}

void ModelFbx::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;
	//頂点データ全体のサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin) * vertices_.size());
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	//頂点バッファへのデータ転送
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	const int32_t startVerticesNum = 0;
	vbView_.StrideInBytes = sizeof(vertices_[startVerticesNum]);

	//頂点インデックス全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());
	// リソース設定
	resourceDesc.Width = sizeIB;
	resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);

	// インデックスバッファ生成
	result = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
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

	//テクスチャ画像データ
	const DirectX::Image* img = scratchImg_.GetImage(0, 0, 0);
	assert(img);
	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata_.format,
		metadata_.width,
		(UINT)metadata_.height,
		(UINT16)metadata_.arraySize,
		(UINT16)metadata_.mipLevels);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES texHeapProps =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// テクスチャ用バッファの生成
	result = device->CreateCommittedResource(
		&texHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff_));
	// テクスチャバッファにデータ転送
	result = texbuff_->WriteToSubresource(
		0,
		nullptr,              // 全領域へコピー
		img->pixels,          // 元データアドレス
		(UINT)img->rowPitch,  // 1ラインサイズ
		(UINT)img->slicePitch // 1枚サイズ
	);

	// SRV用のデスクリプタヒープを生成
	const UINT descriptorNum = 1;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = descriptorNum; // シェーダーリソースビュー1つ
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeapSRV_));//生成
	
	// SRVのシェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff_->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	const UINT tex2DMipLevelNum = 1;
	srvDesc.Texture2D.MipLevels = tex2DMipLevelNum;

	device->CreateShaderResourceView(texbuff_.Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		descHeapSRV_->GetCPUDescriptorHandleForHeapStart()
	);

}
