#include "DirectionalLight.h"
#include <cassert>

using namespace DirectX;

/*

*	DirectionalLight.cpp

*	ライト

*/

//静的メンバ変数の実態
ID3D12Device* DirectionalLight::device_ = nullptr;

void DirectionalLight::StaticInitialize(ID3D12Device* device)
{
	//再初期化チェック
	assert(!DirectionalLight::device_);
	//nullptrチェック
	assert(device);
	//静的メンバ変数のセット
	DirectionalLight::device_ = device;
}

DirectionalLight* DirectionalLight::Create()
{
	//3Dオブジェクトのインスタンスを生成
	DirectionalLight* instance = new DirectionalLight();
	//初期化
	instance->Initialize();
	//生成したインスタンスを返す
	return instance;
}

void DirectionalLight::Initialize()
{

	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
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

	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferData) + resDescPreset.width) & ~resDescPreset.width;//256バイトアラインメント
	cbResourseDesc.Height = resDescPreset.height;
	cbResourseDesc.DepthOrArraySize = resDescPreset.arraysize;
	cbResourseDesc.MipLevels = resDescPreset.mipLevels;
	cbResourseDesc.SampleDesc.Count = resDescPreset.sampleCount;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device_->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	//定数バッファへデータ転送
	TransferConstBuffer();
}

void DirectionalLight::Update()
{
	//値の更新があった時だけ定数バッファに転送する
	if (dirty_)
	{
		TransferConstBuffer();
		dirty_ = false;
	}
}

void DirectionalLight::Draw(ID3D12GraphicsCommandList* cmdList, const UINT rootParameterIndex)
{
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff_->GetGPUVirtualAddress());
}

void DirectionalLight::TransferConstBuffer()
{
	HRESULT result;
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->lightV = -lightDir_;
		constMap->lightColor = lightColor_;
		constBuff_->Unmap(0, nullptr);
	}

}

void DirectionalLight::SetLightDir(const XMVECTOR& lightdir)
{
	//正規化してセット
	this->lightDir_ = XMVector3Normalize(lightdir);
	dirty_ = true;
}

void DirectionalLight::SetLightColor(const XMFLOAT3& lightcolor)
{
	//正規化してセット
	this->lightColor_ = lightcolor;
	dirty_ = true;
}
