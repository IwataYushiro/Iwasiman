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

std::unique_ptr<LightGroup> LightGroup::Create()
{
	//3Dオブジェクトのインスタンスを生成
	std::unique_ptr<LightGroup> instance = std::make_unique<LightGroup>();
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

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList,const UINT rootParameterIndex)
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
		//スポットライト
		for (int i = 0; i < SpotLightNum; i++)
		{
			//lightが有効なら設定を転送
			if (spotLights_[i].IsActive())
			{
				constMap->spotLights[i].active = LAN_True;
				constMap->spotLights[i].lightv = spotLights_[i].GetLightDir();
				constMap->spotLights[i].lightPos = spotLights_[i].GetLightPos();
				constMap->spotLights[i].lightColor = spotLights_[i].GetLightColor();
				constMap->spotLights[i].lightatten = spotLights_[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos =
					spotLights_[i].GetLightFactorAngleCos();
			}
			//無効なら転送しない
			else
			{
				constMap->spotLights[i].active = LAN_False;
			}
		}
		//丸影
		for (int i = 0; i < CircleShadowNum; i++)
		{
			//lightが有効なら設定を転送
			if (circleShadows_[i].IsActive())
			{
				constMap->circleShadows[i].active = LAN_True;
				constMap->circleShadows[i].dir = circleShadows_[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows_[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight = circleShadows_[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten = circleShadows_[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos = circleShadows_[i].GetFactorAngleCos();
			}
			//無効なら転送しない
			else
			{
				constMap->circleShadows[i].active = LAN_False;
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

void LightGroup::SetDirLightActive(const int index, const bool active)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(const int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetDirLightColor(const int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightGroup::SetPointLightPos(const int index, const XMFLOAT3& lightPos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightPos(lightPos);
	dirty_ = true;
}

void LightGroup::SetPointLightColor(const int index, const XMFLOAT3& lightColor)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightColor(lightColor);
	dirty_ = true;

}

void LightGroup::SetPointLightAtten(const int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetPointLightActive(const int index,const bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetActive(active);
}

void LightGroup::SetSpotLightDir(const int index, const XMVECTOR& lightDir)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightDir(lightDir);
	dirty_ = true;
}

void LightGroup::SetSpotLightPos(const int index, const XMFLOAT3& lightPos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightPos(lightPos);
	dirty_ = true;
}

void LightGroup::SetSpotLightColor(const int index, const XMFLOAT3& lightColor)
{
	assert(0 <= index && index < SpotLightNum);
	
	spotLights_[index].SetLightColor(lightColor);
	dirty_ = true;
}

void LightGroup::SetSpotLightAtten(const int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetSpotLightFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightFactorAngleCos(lightFactorAngleCos);
	dirty_ = true;
}

void LightGroup::SetSpotLightActive(const int index, const bool active)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetActive(active);
}

void LightGroup::SetCircleShadowDir(const int index, const XMVECTOR& lightDir)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetDir(lightDir);
	dirty_ = true;
}

void LightGroup::SetCircleShadowCasterPos(const int index, const XMFLOAT3& casterPos)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetCasterPos(casterPos);
	dirty_ = true;
}

void LightGroup::SetCircleShadowDistanceCasterLight(const int index, const float distanceCasterLight)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetDistanceCasterLight(distanceCasterLight);
	dirty_ = true;
}

void LightGroup::SetCircleShadowAtten(const int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetCircleShadowFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetFactorAngleCos(lightFactorAngleCos);
	dirty_ = true;
}

void LightGroup::SetCircleShadowActive(const int index, const bool active)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetActive(active);
}
