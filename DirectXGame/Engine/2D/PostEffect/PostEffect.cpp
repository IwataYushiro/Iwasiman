#include "PostEffect.h"
#include <d3dx12.h>
#include "WinApp.h"

using namespace DirectX;

PostEffect::PostEffect()
{
}

void PostEffect::Initialize()
{
	HRESULT result;
	//基盤クラスとしての初期化
	Sprite::Initialize(spCommon_);
	//テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
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

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//非表示
	if (GetInvisible())
	{
		return;
	}
	//パイプラインステートとルートシグネチャの設定
	spCommon_->PreDraw();
	//テクスチャコマンド
	spCommon_->SetTextureCommands(textureIndex_);
	//頂点バッファビューの設定コマンド
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファビュー(CBVの設定コマンド)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	//描画コマンド
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);

}
