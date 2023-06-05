#include "PostEffect.h"
#include <d3dx12.h>
#include "WinApp.h"

using namespace DirectX;

PostEffect::PostEffect()
{
}

void PostEffect::Initialize(SpriteCommon* spCommon, uint32_t textureIndex)
{
	assert(spCommon);
	this->spCommon_ = spCommon;

	//基盤クラスとしての初期化
	Sprite::Initialize(spCommon_, textureIndex);
	//テクスチャ生成
	CreateTexture();
	// SRV生成
	CreateSRV();
	// RTV生成

	// 深度バッファ生成

	// DSV生成
	
}

void PostEffect::CreateTexture()
{
	HRESULT result;
	//テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::window_width,
		(UINT)WinApp::window_height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	//テクスチャバッファ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));
	{

		//テクスチャ赤クリア
		//画素数(1280 x 720 = 921600ピクセル)
		const UINT pixelCount = WinApp::window_width * WinApp::window_height;
		//画像一枚分のデータサイズ
		const UINT rowPitch = sizeof(UINT) * WinApp::window_width;
		//画像全体のデータサイズ
		const UINT depthPitch = rowPitch * WinApp::window_height;
		//画像イメージ
		UINT* img = new UINT[pixelCount];
		for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

		//テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[] img;
	}
}

void PostEffect::CreateSRV()
{
	HRESULT result;
	//SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	//SRV用デスクリプタヒープ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//デスクリプタヒープにSRV作成
	spCommon_->GetDxCommon()->GetDevice()->CreateShaderResourceView(
		texBuff.Get()/*ビューと関連付けるバッファ*/, &srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//非表示
	if (GetInvisible())
	{
		return;
	}
	//パイプラインステートとルートシグネチャの設定
	spCommon_->GetDxCommon()->GetCommandList()->SetPipelineState(spCommon_->GetPipelineState());
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootSignature(spCommon_->GetRootSignature());
	//プリミティブ形状の設定コマンド
	spCommon_->GetDxCommon()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	spCommon_->GetDxCommon()->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//テクスチャコマンド
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());
	//頂点バッファビューの設定コマンド
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファビュー(CBVの設定コマンド)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	//描画コマンド
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);

}
