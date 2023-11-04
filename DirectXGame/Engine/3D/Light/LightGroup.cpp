#include "LightGroup.h"
#include <cassert>

using namespace DirectX;

/*

*	LightGroup.cpp

*	ライトグループ

*/

//静的メンバ変数の実体
ID3D12Device* LightGroup::device_ = nullptr;

void LightGroup::StaticInitialize(ID3D12Device* device)
{
	//再初期化チェック
	assert(!LightGroup::device_);
	//nullptrチェック
	assert(device);

	LightGroup::device_ = device;
}

LightGroup* LightGroup::Create()
{
	//3Dオブジェクトのインスタンスを生成
	LightGroup* instance = new LightGroup();
	//初期化
	instance->Initialize();
	//生成したインスタンスを返す
	return instance;
}

void LightGroup::Initialize()
{
	HRESULT result;

	//標準のライト設定
	DefaultLightSetting();
	//ヒープ設定
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

void LightGroup::Update()
{
	//値の更新があった時だけ定数バッファに転送
	if (dirty_)
	{
		TransferConstBuffer();
		dirty_ = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex)
{
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff_->GetGPUVirtualAddress());
}

void LightGroup::TransferConstBuffer()
{
	HRESULT result;
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		enum LightActiveNum
		{
			LAN_False=0,
			LAN_True=1,
		};
		//環境光
		constMap->ambientColor = ambientColor_;
		//平行光源
		for (int i = 0; i < DirLightNum; i++)
		{
			//lightが有効なら設定を転送
			if (dirLights_[i].IsActive())
			{
				constMap->dirLights[i].active = LAN_True;
				constMap->dirLights[i].lightV = -dirLights_[i].GetLightDir();
				constMap->dirLights[i].lightColor = dirLights_[i].GetLightColor();
			}
			//無効なら転送しない
			else
			{
				constMap->dirLights[i].active = LAN_False;
			}
		}
		//点光源
		for (int i = 0; i < PointLightNum; i++)
		{
			//lightが有効なら設定を転送
			if (pointLights_[i].IsActive())
			{
				constMap->pointLights[i].active = LAN_True;
				constMap->pointLights[i].lightPos = pointLights_[i].GetLightPos();
				constMap->pointLights[i].lightColor = pointLights_[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights_[i].GetLightAtten();
			}
			//無効なら転送しない
			else
			{
				constMap->pointLights[i].active = LAN_False;
			}
		}
		constBuff_->Unmap(0, nullptr);
	}
}

void LightGroup::DefaultLightSetting()
{
	//ライト番号
	enum LightNum
	{
		LN_Light0=0, 
		LN_Light1 = 1,
		LN_Light2 = 2,
	};
	//点光源番号
	enum PointLightNum
	{
		PLN_Light0=0,
	};
	//デフォルトプリセット
	struct DefaultLightPreset
	{
		//0番
		const XMVECTOR light0Dir = { 0.0f,-1.0f,0.0f,0.0f };
		const XMFLOAT3 light0Color = { 1.0f,1.0f,1.0f };
		const float pad0 = 0.0f;
		//1番
		const XMVECTOR light1Dir = { 0.5f,0.1f,0.2f,0.0f };
		const XMFLOAT3 light1Color = { 1.0f,1.0f,1.0f };
		const float pad1 = 0.0f;
		//2番
		const XMVECTOR light2Dir = { -0.5f,0.1f,-0.2f,0.0f };
		const XMFLOAT3 light2Color = { 1.0f,1.0f,1.0f };
		const float pad2 = 0.0f;

		//点光源
		//0番
		const XMFLOAT3 pointLight0Color = { 1.0f,1.0f,1.0f };
		const float padp0 = 0.0f;
		const XMFLOAT3 pointLight0Pos = { 0.0f,0.0f,0.0f };
		const float padp1 = 0.0f;
		const XMFLOAT3 pointLight0Atten = { 1.0f,1.0f,1.0f };
		const float padp2 = 0.0f;
	};
	DefaultLightPreset preset;

	dirLights_[LN_Light0].SetActive(true);
	dirLights_[LN_Light0].SetLightColor(preset.light0Color);
	dirLights_[LN_Light0].SetLightDir(preset.light0Dir);

	dirLights_[LN_Light1].SetActive(true);
	dirLights_[LN_Light1].SetLightColor(preset.light1Color);
	dirLights_[LN_Light1].SetLightDir(preset.light1Dir);

	dirLights_[LN_Light2].SetActive(true);
	dirLights_[LN_Light2].SetLightColor(preset.light2Color);
	dirLights_[LN_Light2].SetLightDir(preset.light2Dir);

	pointLights_[PLN_Light0].SetActive(false);
	pointLights_[PLN_Light0].SetLightColor(preset.pointLight0Color);
	pointLights_[PLN_Light0].SetLightPos(preset.pointLight0Pos);
	pointLights_[PLN_Light0].SetLightAtten(preset.pointLight0Atten);
}

void LightGroup::SetAmbientColor(const XMFLOAT3& color)
{
	ambientColor_ = color;
	dirty_ = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetDirLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightGroup::SetPointLightPos(int index, const XMFLOAT3& lightPos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightPos(lightPos);
	dirty_ = true;
}

void LightGroup::SetPointLightColor(int index, const XMFLOAT3& lightColor)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightColor(lightColor);
	dirty_ = true;

}

void LightGroup::SetPointLightAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetActive(active);
}
